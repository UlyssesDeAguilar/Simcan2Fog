#include "IotApplication.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "inet/common/TimeTag_m.h"
#include "Applications/Services/SimServiceReq_m.h"

Define_Module(IotApplication);

using namespace hypervisor;
using namespace iot;

simsignal_t roundTripTime = cComponent::registerSignal("roundTripTime");

void IotApplication::initialize()
{
    // Init the super-class
    UserAppBase::initialize();

    processingMIs = par("processingMIs");
    listeningPort = par("listeningPort");

    setReturnCallback(this);

    cModule *edgeTile = getModuleByPath(parentPath);
    endpointName = edgeTile->par("serviceName");
    cModule *actuator = edgeTile->getSubmodule("actuator", 0);
    int vectorSize = actuator->getVectorSize();

    // Get the resolver
    resolver = check_and_cast<DnsResolver*>(getModuleByPath(nsPath)->getSubmodule("dnsResolver"));
    
    for (int i = 0; i < vectorSize; i++)
    {
        actuator = edgeTile->getSubmodule("actuator", i);
        actuators.push_back(L3AddressResolver().addressOf(actuator));
        EV << "Detected actuator" << i << "with address:" << actuators.back();
    }

    // Record times
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void IotApplication::processSelfMessage(cMessage *msg)
{
    // Open connection on listening port for starting the simulation
    udpSocket = open(listeningPort, SOCK_DGRAM);

    // Resolve the service name
    resolve(endpointName);
}

void IotApplication::handleResolverReturned(uint32_t ip)
{
    Enter_Method_Silent();
    if (ip == 0)
        error("Unexpected unavailability of service: %s", endpointName);

    endpointIp.set(ip);

    EV << "Resolving service: " << endpointName << " yielded: " << endpointIp << "\n";
    EV << "Starting connection with service: ..." << "\n";
    tcpSocket = open(-1, SOCK_STREAM);
    connect(tcpSocket, endpointIp.getInt(), 443);
}

void IotApplication::handleConnectReturn(int sockFd, bool connected)
{
    recordScalar("connected to endpoint", 1.0);
    if (!connected)
        error("Unable to connect to endpoint");

    serviceUp = true;
}

void IotApplication::handleDataArrived(int sockFd, Packet *p)
{
    if (sockFd == udpSocket)
    {
        EV << "Recieved packet: " << p->getName() << "\n";
        numPings++;
        if (numPings > (int)par("pingsThreshold") && serviceUp)
        {
            execute(processingMIs);
            numPings = 0;
        }
    }
    else if (sockFd == tcpSocket)
    {
        EV << "Got response from the server\n";

        chronometer = simTime().dbl() - chronometer;
        emit(packetReceivedSignal, p);
        emit(roundTripTime, chronometer);
        
        if (actuators.size() > 0)
            sendActuator();
    }

    delete p;
}

void IotApplication::returnExec(simtime_t timeElapsed, SM_CPU_Message *sm)
{
    EV << "Processing finished, sending to server ..." << "\n";
    sendServer();
}

void IotApplication::sendServer()
{
    Packet *packet = new Packet("Controller command");
    auto payload = new SimServiceReq();

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setChunkLength(B(par("synthSize")));
    payload->setServerClose(par("serverClosesOnReturn"));
    payload->setExpectedReplyLength(B(par("replySize")));
    payload->setReplyDelay(par("replyDelay"));
    payload->setVmType(par("vmType"));
    packet->insertAtBack(Ptr<GenericAppMsg>(payload));

    emit(packetSentSignal, packet);
    chronometer = simTime().dbl();

    _send(tcpSocket, packet);
}

void IotApplication::sendActuator()
{
    int randAcutator = intuniform(0, actuators.size() - 1);
    int on = intuniform(0, 1);

    EV << "Sending command to actuator\n";
    // Send command to actuator
    Packet *packet = new Packet("Controller command");
    const auto &payload = makeShared<IotPayload>();
    payload->setChunkLength(B(20));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setTag("Controller command");
    payload->setValue(on);
    payload->setUnit("");
    packet->insertAtBack(payload);

    _send(udpSocket, packet, actuators[randAcutator].toIpv4().getInt(), listeningPort);
}

bool IotApplication::handlePeerClosed(int sockFd) 
{
    error("Here");
    return true; 
}

void IotApplication::finish()
{
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << " + Total simulation time (simulated):" << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds" << '\n';
    // EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << '\n';

    // Finish the super-class
    actuators.clear();
    UserAppBase::finish();
}