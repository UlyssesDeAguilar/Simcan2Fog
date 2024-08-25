import s2f.architecture.net.switch.Switch;
import echo.Echo;

network EchoNet1 {
    parameters:
        int vecSize = default(5);
    submodules:
        main: Echo{
            ip = 1024;
            recvMode = false;
            addressRange = vecSize;
        }

        sw: Switch;
        
        vec[vecSize]: Echo{
            ip = index;
            recvMode = true;
        }

    connections:
        main.comm <--> sw.manager;

        for i=0..vecSize-1 {
            sw.comm++ <--> vec[i].comm;
        }
}