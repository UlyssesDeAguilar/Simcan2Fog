#include "IotSensorModel.h"

#include "s2f/architecture/nodes/iot/IotPayload_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/TagBase_m.h"
#include "inet/common/TimeTag_m.h"
#include "inet/common/lifecycle/ModuleOperations.h"
#include "inet/common/packet/Packet.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/transportlayer/contract/udp/UdpControlInfo_m.h"

using namespace inet;
using namespace s2f::iot;

Define_Module(IotSensorModel);

IotSensorModel::~IotSensorModel()
{
    cancelAndDelete(selfMsg);
}

void IotSensorModel::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);

        localPort = par("localPort");
        destPort = par("destPort");
        startTime = par("startTime");
        stopTime = par("stopTime");

        value = &par("value");
        tag = par("tag");
        unit = par("unit");

        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");
        selfMsg = new cMessage("sendTimer");
    }
}

void IotSensorModel::finish()
{
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    ApplicationBase::finish();
}

void IotSensorModel::sendPacket()
{
    std::ostringstream str;
    str << tag << "-" << numSent;
    Packet *packet = new Packet(str.str().c_str());
    
    const auto &payload = makeShared<IotPayload>();
    
    payload->setChunkLength(B(par("messageLength")));
    payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
    payload->setTag(tag);
    payload->setValue(*value);
    payload->setUnit(unit);
    packet->insertAtBack(payload);
    
    emit(packetSentSignal, packet);
    socket.sendTo(packet, controllerAddress, destPort);
    numSent++;
}

void IotSensorModel::processStart()
{
    socket.setOutputGate(gate("socketOut"));
    const char *localAddress = par("localAddress");
    socket.bind(*localAddress ? L3AddressResolver().resolve(localAddress) : L3Address(), localPort);
    socket.setCallback(this);

    cModule *controller = getModuleByPath(par("controllerPath"));
    controllerAddress = L3AddressResolver().addressOf(controller, (const char*)par("controllerInterface"));

    selfMsg->setKind(SEND);
    processSend();

    if (stopTime >= SIMTIME_ZERO)
    {
        selfMsg->setKind(STOP);
        scheduleAt(stopTime, selfMsg);
    }
}

void IotSensorModel::processSend()
{
    sendPacket();
    simtime_t d = simTime() + par("updatePeriod");
    if (stopTime < SIMTIME_ZERO || d < stopTime)
    {
        selfMsg->setKind(SEND);
        scheduleAt(d, selfMsg);
    }
    else
    {
        selfMsg->setKind(STOP);
        scheduleAt(stopTime, selfMsg);
    }
}

void IotSensorModel::processStop()
{
    socket.close();
}

void IotSensorModel::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        ASSERT(msg == selfMsg);
        switch (selfMsg->getKind())
        {
        case START:
            processStart();
            break;

        case SEND:
            processSend();
            break;

        case STOP:
            processStop();
            break;

        default:
            throw cRuntimeError("Invalid kind %d in self message", (int)selfMsg->getKind());
        }
    }
    else
        socket.processMessage(msg);
}

void IotSensorModel::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // process incoming packet
    processPacket(packet);
}

void IotSensorModel::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
    delete indication;
}

void IotSensorModel::socketClosed(UdpSocket *socket)
{
    //if (operationalState == State::STOPPING_OPERATION)
    //    startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void IotSensorModel::refreshDisplay() const
{
    ApplicationBase::refreshDisplay();

    char buf[100];
    sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
    getDisplayString().setTagArg("t", 0, buf);
}

void IotSensorModel::processPacket(Packet *pk)
{
    emit(packetReceivedSignal, pk);
    EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(pk) << endl;
    delete pk;
    numReceived++;
}

void IotSensorModel::handleStartOperation(LifecycleOperation *operation)
{
    simtime_t start = std::max(startTime, simTime());
    if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime))
    {
        selfMsg->setKind(START);
        scheduleAt(start, selfMsg);
    }
}

void IotSensorModel::handleStopOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.close();
    delayActiveOperationFinish(par("stopOperationTimeout"));
}

void IotSensorModel::handleCrashOperation(LifecycleOperation *operation)
{
    cancelEvent(selfMsg);
    socket.destroy(); // TODO  in real operating systems, program crash detected by OS and OS closes sockets of crashed programs.
}
