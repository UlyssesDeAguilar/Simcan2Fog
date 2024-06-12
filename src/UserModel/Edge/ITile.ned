package simcan2.UserModel.Edge;

moduleinterface ITile {
    parameters:
        string serviceName;
    gates:
        inout ethernet;
}