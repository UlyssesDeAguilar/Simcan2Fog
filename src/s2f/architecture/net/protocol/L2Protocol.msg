message L2Protocol
{
    int origin;
    int destination;
    cMessage *payload @owned;
}

cplusplus(L2Protocol){{
static L2Protocol * encapsulate(cMessage *payload, int origin, int destination);
static L2Protocol * encapsulate(cMessage *payload, int destination);
static cMessage * tryDecapsulate(cMessage *msg);
}}

cplusplus(cc) {{

L2Protocol * L2Protocol::encapsulate(cMessage *payload, int origin, int destination)
{
    auto packet = new L2Protocol();
    packet->setOrigin(origin);
    packet->setDestination(destination);
    packet->setPayload(payload);
    return packet;
}

L2Protocol * L2Protocol::encapsulate(cMessage *payload, int destination)
{
    auto packet = new L2Protocol();
    packet->setDestination(destination);
    packet->setPayload(payload);
    return packet;
}

omnetpp::cMessage * L2Protocol::tryDecapsulate(omnetpp::cMessage *msg)
{
    auto packet = dynamic_cast<L2Protocol*>(msg);
    if (packet)
    {
        cMessage* payload = packet->removePayload();
        delete msg;
        return payload;
    }
    else
        return msg;
}
}}