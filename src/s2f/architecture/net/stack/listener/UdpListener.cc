#include "UdpListener.h"

#include "inet/common/socket/SocketTag_m.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/FragmentationTag_m.h"
#include "inet/networklayer/common/L3AddressResolver.h"

using namespace inet;

Define_Module(UdpListener);

void UdpListener::initialize(int stage)
{
	ApplicationBase::initialize(stage);
	if (stage == INITSTAGE_LOCAL)
	{
		dontFragment = par("dontFragment");
		numSent = 0;
		numReceived = 0;
		WATCH(numSent);
		WATCH(numReceived);
	}
}

void UdpListener::handleMessageWhenUp(cMessage *message)
{
	if (socket.belongsToSocket(message))
		socket.processMessage(message);
	else
	{
		auto packet = check_and_cast<Packet*>(message);
		if (dontFragment)
			packet->addTagIfAbsent<FragmentationReq>()->setDontFragment(true);
		socket.send(packet);
		numSent++;
		emit(packetSentSignal, packet);
	}
}

void UdpListener::finish()
{
	recordScalar("packets sent", numSent);
	recordScalar("packets received", numReceived);
	ApplicationBase::finish();
}

void UdpListener::refreshDisplay() const
{
	ApplicationBase::refreshDisplay();
	char buf[100];
	sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
	getDisplayString().setTagArg("t", 0, buf);
}

void UdpListener::setSocketOptions()
{
	int timeToLive = par("timeToLive");
	if (timeToLive != -1)
		socket.setTimeToLive(timeToLive);

	int dscp = par("dscp");
	if (dscp != -1)
		socket.setDscp(dscp);

	int tos = par("tos");
	if (tos != -1)
		socket.setTos(tos);

	NetworkInterface *multicastInterface = nullptr;
	const char *multicastInterfaceName = par("multicastInterface");
	if (multicastInterfaceName[0])
	{
		IInterfaceTable *ift = getModuleFromPar<IInterfaceTable>(
				par("interfaceTableModule"), this);
		multicastInterface = ift->findInterfaceByName(multicastInterfaceName);
		if (!multicastInterface)
			throw cRuntimeError(
					"Wrong multicastInterface setting: no interface named \"%s\"",
					multicastInterfaceName);
		socket.setMulticastOutputInterface(
				multicastInterface->getInterfaceId());
	}

	auto multicastAddresses = check_and_cast<cValueArray*>(
			par("multicastAddresses").objectValue());
	int multicastInterfaceId =
			multicastInterface != nullptr ?
					multicastInterface->getInterfaceId() : -1;
	for (int i = 0; i < multicastAddresses->size(); i++)
		socket.joinMulticastGroup(
				Ipv4Address(multicastAddresses->get(i).stringValue()),
				multicastInterfaceId);

	bool receiveBroadcast = par("receiveBroadcast");
	if (receiveBroadcast)
		socket.setBroadcast(true);

	bool joinLocalMulticastGroups = par("joinLocalMulticastGroups");
	if (joinLocalMulticastGroups)
	{
		MulticastGroupList mgl = getModuleFromPar<IInterfaceTable>(
				par("interfaceTableModule"), this)->collectMulticastGroups();
		socket.joinLocalMulticastGroups(mgl);
	}
	socket.setCallback(this);
}

void UdpListener::socketDataArrived(UdpSocket *socket, Packet *packet)
{
	emit(packetReceivedSignal, packet);
	EV_INFO << "Received packet: " << UdpSocket::getReceivedPacketInfo(packet)
					<< endl;
	numReceived++;
    packet->removeTag<SocketInd>();
	send(packet, "trafficOut");
}

void UdpListener::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
	EV_WARN << "Ignoring UDP error report " << indication->getName() << endl;
	delete indication;
}

void UdpListener::socketClosed(UdpSocket *socket)
{
	if (operationalState == State::STOPPING_OPERATION)
		startActiveOperationExtraTimeOrFinish(par("stopOperationExtraTime"));
}

void UdpListener::handleStartOperation(LifecycleOperation *operation)
{
	setSocketOptions();
	socket.setOutputGate(gate("socketOut"));
	const char *localAddress = par("localAddress");
	socket.bind(
			*localAddress ?
					L3AddressResolver().resolve(localAddress) : L3Address(),
			par("localPort"));
}

void UdpListener::handleStopOperation(LifecycleOperation *operation)
{
	socket.close();
	delayActiveOperationFinish(par("stopOperationTimeout"));
}

void UdpListener::handleCrashOperation(LifecycleOperation *operation)
{
	socket.destroy();
}
