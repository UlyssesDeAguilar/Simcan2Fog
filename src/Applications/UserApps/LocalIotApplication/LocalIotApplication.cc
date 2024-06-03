#include "LocalIotApplication.h"
Define_Module(LocalIotApplication);
using namespace hypervisor;

void LocalIotApplication::initialize()
{
    // Init the super-class
    UserAppBase::initialize();

    processingMIs = par("processingMIs");
    listeningPort = par("listeningPort");

    // Record times
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void LocalIotApplication::finish()
{
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << " + Total simulation time (simulated):" << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds" << '\n';
    // EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << '\n';

    // Finish the super-class
    UserAppBase::finish();
}

bool LocalIotApplication::run()
{
    bool rerun = false;
    switch (pc)
    {
    case 0:
        // Opening udp socket
        open_server(listeningPort, ConnectionMode::UDP);
        break;
    case 1:
    {
        if (returnContext.result == ERROR)
            abort();
        else
        {
            udpSocket = returnContext.rf;
            rerun = true;
        }
        break;
    }
    case 2:
        rerun = recv(udpSocket);
        break;
    case 3:
        EV_INFO << "Recieved package" << "\n";
        if (actuators.size() > 0)
        {
            EV << "Should do something here" << "\n";
            // recv(udpSocket);
        }
        rerun = true;
        break;
    default:
        // Emulates an arduino, it's in an infinite loop that listens to the incoming requests
        pc = 1;
        rerun = true;
        break;
    }

    return rerun;
}