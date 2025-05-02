#include "IotApplication.h"

#include "s2f/apps/services/SimServiceReq_m.h"
#include "inet/applications/tcpapp/GenericAppMsg_m.h"
#include "s2f/architecture/net/protocol/RestfulRequest_m.h"
#include "s2f/architecture/net/protocol/RestfulResponse_m.h"
#include "inet/common/TimeTag_m.h"

Define_Module(IotApplication);

using namespace hypervisor;
using namespace iot;

simsignal_t roundTripTime = cComponent::registerSignal("roundTripTime");

void IotApplication::initialize() {
    // Init the super-class
    AppBase::initialize();

    processingMIs = par("processingMIs");
    listeningPort = par("listeningPort");

    setReturnCallback(this);

    cModule *edgeTile = getModuleByPath(par("parentPath"));
    cModule *actuator = edgeTile->getSubmodule("actuator", 0);
    int vectorSize = actuator->getVectorSize();

    for (int i = 0; i < vectorSize; i++) {
        actuator = edgeTile->getSubmodule("actuator", i);
        actuators.push_back(L3AddressResolver().addressOf(actuator));
        EV << "Detected actuator " << i << " with address: " << actuators.back()
                  << "\n";
    }

    // Record times
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void IotApplication::processSelfMessage(cMessage *msg) {
    if (msg->getKind() == EXEC_START) {
        // Open connection on listening port for starting the simulation
        udpSocket = open(listeningPort, SOCK_DGRAM, par("localInterface"));
    } else if (msg->getKind() == POLL) {
        sendPoll();
    }
}

void IotApplication::handleResolutionFinished(const L3Address ip,
        bool resolved) {
    if (!resolved)
        error("Unexpected unavailability of service: %s", serviceName);

    endpointIp = ip;

    EV << "Resolving service: " << serviceName << " yielded: " << endpointIp
              << "\n";
    EV << "Starting connection with service: ..." << "\n";

    tcpSocket = open(-1, SOCK_STREAM, par("externalInterface"));
    connect(tcpSocket, endpointIp, 443);
}

void IotApplication::handleConnectReturn(int sockFd, bool connected) {
    recordScalar("connected to endpoint", 1.0);
    if (!connected)
        error("Unable to connect to endpoint");

    if (sockFd == tcpSocket)
        sendPoll();
    //serviceUp = true;
}

void IotApplication::handleDataArrived(int sockFd, Packet *p) {

    if (sockFd == udpSocket) {
        EV << "Recieved packet: " << p->getName() << "\n";
        numPings++;
        if (numPings > (int) par("pingsThreshold") && serviceUp) {
            execute(processingMIs);
            numPings = 0;
        }
    } else if (sockFd == tcpSocket) {

        if (!serviceUp) {
            auto httpResponse = p->peekData<RestfulResponse>();
            if (httpResponse->getResponseCode() == ResponseCode::OK){
                EV_INFO << "Service " << serviceName << " is up\n";
                serviceUp = true;
            }else {
                EV_INFO << "Service " << serviceName
                               << " is not ready yet, waiting to poll again\n";
                event->setKind(POLL);
                scheduleAt(simTime() + par("pollingInterval"), event);
            }
        } else {
            EV << "Got response from the server\n";
            chronometer = simTime().dbl() - chronometer;
            emit(packetReceivedSignal, p);
            emit(roundTripTime, chronometer);

            if (actuators.size() > 0)
                sendActuator();
        }
    }

    delete p;
}

void IotApplication::returnExec(simtime_t timeElapsed, SM_CPU_Message *sm) {
    EV << "Processing finished, sending to server ..." << "\n";
    sendServer();
}

void IotApplication::sendServer() {
    Packet *packet = new Packet("Controller data");
    auto payload = makeShared<RestfulRequest>();

    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setChunkLength(B(par("synthSize")));
    packet->insertData(payload);

    emit(packetSentSignal, packet);
    chronometer = simTime().dbl();

    _send(tcpSocket, packet);
}

void IotApplication::sendPoll() {
    // Send a request to the service
    auto packet = new Packet("Request (Probe)");
    auto request = makeShared<RestfulRequest>();
    request->setVerb(Verb::HEAD);
    request->setHost(serviceName);
    request->setPath("/");
    packet->insertData(request);
    _send(tcpSocket, packet);
}

void IotApplication::sendActuator() {
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

    _send(udpSocket, packet, actuators[randAcutator].toIpv4().getInt(),
            listeningPort);
}

void IotApplication::handleParameterChange(const char *parameterName) {
    if (strcmp(parameterName, "serviceName") == 0
            && !opp_isempty(par("serviceName")) && !serviceUp){
        serviceName = par("serviceName");
        resolve(serviceName);
    }
}

bool IotApplication::handlePeerClosed(int sockFd) {
    error("Here");
    return true;
}

void IotApplication::finish() {
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << " + Total simulation time (simulated):"
                   << (simTime().dbl() - simStartTime.dbl()) << " seconds "
                   << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds"
                   << '\n';
    // EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << '\n';

    // Finish the super-class
    actuators.clear();
    AppBase::finish();
}
