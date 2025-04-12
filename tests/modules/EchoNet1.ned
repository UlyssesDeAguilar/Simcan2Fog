import s2f.architecture.net.switch.Switch;
import s2f.architecture.net.card.NetworkCard;
import echo.EchoModule;

network EchoNet1 {
    parameters:
        int vecSize = default(5);
    submodules:
        main: EchoModule{
            ip = 1024;
            recvMode = false;
            addressRange = parent.vecSize;
        }

        sw: Switch;
        
        vec[vecSize]: EchoModule{
            ip = index;
            recvMode = true;
        }

    connections:
        main.comm <--> sw.comm++;

        for i=0..vecSize-1 {
            sw.comm++ <--> vec[i].comm;
        }
}