package s2f.users.edge;

moduleinterface ITile {
    parameters:
        string serviceName;
    gates:
        inout ethernet;
}