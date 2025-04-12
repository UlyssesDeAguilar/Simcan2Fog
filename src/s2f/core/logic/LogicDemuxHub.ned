package s2f.core.logic;

module LogicDemuxHub
{
    parameters:
        @class(LogicDemuxHub);
        string demuxClass;

        @display("i=device/hub,,0;bgb=,,,,0");
    gates:
        input mainIn;
        output mainOut;
        input demuxIn[];
        output demuxOut[];
}
