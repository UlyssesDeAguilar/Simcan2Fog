#include "DNS_Service.h"

Define_Module(DNS_Service);

std::set<std::string> DNS_Service::prefixSet = {"dc", "fg", "ed", "cloudProvider", "userGenerator"};

void DNS_Service::initialize(int stage)
{
    // This layer could be initialized earlier, maybe in INITSTAGE_TRANSPORT_LAYER
    if (stage == INITSTAGE_LAST && isMain)
    {
        // Retrieve and open the dump file from Ipv4Configurator
        auto *root = getEnvir()->getXMLDocument(config_file);

        if (root == nullptr)
            error("Error reading from IP table dump");

        // Retrieve the interfaces (they are stored in a vector)
        auto interface_list = root->getElementsByTagName("interface");

        // Process them
        for (auto elem : interface_list)
        {
            processXMLInterface(elem);
        }

        // Dump (if requested) the detected topology
        if (debug)
            printRecords();
    }
    else if (stage == INITSTAGE_LOCAL)
    {
        isMain = getParentModule()->par("isMain");
        debug = getParentModule()->par("debug");
        config_file = getParentModule()->par("ipDump");
    }
}

void DNS_Service::processXMLInterface(cXMLElement *elem)
{
    auto module_path = elem->getAttribute("hosts");

    if (module_path == nullptr)
        error("Module path is missing");

    // Retrieve the kind from the name of the module
    std::stringstream stream(module_path);
    std::string token;
    std::string hostName;
    std::string componentName("networkAdapter");

    // Start to tokenize the string up to depth 3
    for (int l = 0; std::getline(stream, token, '.') && l < 3; l++)
    {
        /*
            We're looking for x in the path like: NetworkName.x.NetworkAdapter(...)
            with the correct prefix or name
        */
        if (l == 1)
            hostName = token;
        else if (l == 2 && token == componentName && filterHostByName(hostName))
            records[hostName] = L3Address(elem->getAttribute("address"));
    }
}

bool DNS_Service::filterHostByName(std::string hostName)
{
    // Check that we have more than 2 characters
    if (hostName.length() < 2)
        return false;

    // Attempt to filter by prefix
    std::string prefix = hostName.substr(0, 2);
    if (DNS_Service::prefixSet.find(prefix) != DNS_Service::prefixSet.end())
        return true;

    // Attempt to filter by full name
    if (DNS_Service::prefixSet.find(hostName) != DNS_Service::prefixSet.end())
        return true;

    // It did not match anything
    EV << false << endl;
    return false;
}

void DNS_Service::finish()
{
    // Empty at the moment
}

void DNS_Service::handleStartOperation(LifecycleOperation *operation)
{
    // Prepare the socket for incoming data
    socket.setOutputGate(gate("socketOut"));
    socket.setCallback(this);
    socket.bind(53);
}

void DNS_Service::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    auto request = dynamic_cast<INET_AppMessage *>(packet);

    if (request == nullptr)
        error("Recieved unkown packet");

    request->getAppMessage(); // !! PROBLEMS
}
void DNS_Service::socketErrorArrived(UdpSocket *socket, Indication *indication) {}


void DNS_Service::printRecords()
{
    EV << "Detected topology: " << endl;

    // Print all the records (const & avoids copying the elements)
    for (auto const &elem : records)
        EV << elem.first << " : " << elem.second << endl;
}