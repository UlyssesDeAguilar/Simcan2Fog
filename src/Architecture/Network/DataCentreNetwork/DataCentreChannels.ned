// Channels for defining conections between Ethernet hosts
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package simcan2.Architecture.Network.DataCentreNetwork;

channelinterface IDataCentreChannel{
}

//
// 10 megabit/sec Ethernet link
//
channel DataCentreEth10M_channel extends ned.DatarateChannel like IDataCentreChannel{
    datarate = 10Mbps;
}

//
// 100 megabit/sec Ethernet link
//
channel DataCentreEth100M_channel extends ned.DatarateChannel like IDataCentreChannel
{
    datarate = 100Mbps;
}

//
// 1 gigabit/sec Ethernet link
//
channel DataCentreEth1G_channel extends ned.DatarateChannel like IDataCentreChannel
{
    datarate = 1Gbps;
}

//
// 10 gigabit/sec Ethernet link
//
channel DataCentreEth10G_channel extends ned.DatarateChannel like IDataCentreChannel
{
    datarate = 10Gbps;
}

//
// 40 gigabit/sec Ethernet link
//
channel DataCentreEth40G_channel extends ned.DatarateChannel like IDataCentreChannel
{
    datarate = 40Gbps;
}

//
// 100 gigabit/sec Ethernet link
//
channel DataCentreEth100G_channel extends ned.DatarateChannel like IDataCentreChannel
{
    datarate = 100Gbps;
}