// Channels for defining conections between Ethernet hosts
//
// @author Alberto N&uacute;&ntilde;ez Covarrubias
// @date 2010-11-30

package simcan2.Architecture.Network.DataCenterNetwork;

channelinterface IDataCenterChannel{
}

//
// 10 megabit/sec Ethernet link
//
channel DataCenterEth10M_channel extends ned.DatarateChannel like IDataCenterChannel{
    datarate = 10Mbps;
}

//
// 100 megabit/sec Ethernet link
//
channel DataCenterEth100M_channel extends ned.DatarateChannel like IDataCenterChannel
{
    datarate = 100Mbps;
}

//
// 1 gigabit/sec Ethernet link
//
channel DataCenterEth1G_channel extends ned.DatarateChannel like IDataCenterChannel
{
    datarate = 1Gbps;
}

//
// 10 gigabit/sec Ethernet link
//
channel DataCenterEth10G_channel extends ned.DatarateChannel like IDataCenterChannel
{
    datarate = 10Gbps;
}

//
// 40 gigabit/sec Ethernet link
//
channel DataCenterEth40G_channel extends ned.DatarateChannel like IDataCenterChannel
{
    datarate = 40Gbps;
}

//
// 100 gigabit/sec Ethernet link
//
channel DataCenterEth100G_channel extends ned.DatarateChannel like IDataCenterChannel
{
    datarate = 100Gbps;
}