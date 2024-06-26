package s2f.UserModel.Edge;

moduleinterface ITile {
    parameters:
        string serviceName;
    gates:
        inout ethernet;
}