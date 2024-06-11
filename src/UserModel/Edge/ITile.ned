package simcan2.UserModel.Edge;

moduleinterface ITile {
    parameters:
        string serviceName;
        int sensors;
        int actuators;
    gates:
        inout ethernet;
}