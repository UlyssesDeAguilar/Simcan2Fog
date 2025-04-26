package s2f.tiles;

moduleinterface ITile {
    parameters:
        string serviceName;
    gates:
        inout ethernet;
}