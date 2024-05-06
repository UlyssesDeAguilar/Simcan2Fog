#include "DNS_Service.h"
Define_Module(dns::DNS_Service);
using namespace dns;

std::set<std::string> DNS_Service::prefixSet = {"dc", "fg", "ed", "cloudProvider", "userGenerator"};

void DNS_Service::initialize(int stage)
{
    // This layer could be initialized earlier, maybe in INITSTAGE_TRANSPORT_LAYER
    if (stage == (INITSTAGE_LAST + 1) && isMain)
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
    ApplicationBase::initialize(stage);
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
    int tokenCount = 0;
    bool validPath = false;

    // Start to tokenize the string up to depth 4
    while (std::getline(stream, token, '.') && tokenCount <= 4)
    {
        /*
            We're looking for x in the path like: NetworkName.x.NetworkAdapter(...)
            with the correct prefix or name
        */
        switch (tokenCount)
        {
        case 1:
            hostName = token;
            break;
        case 2:
            if (token == componentName && filterHostByName(hostName))
                validPath = true;
            break;
        case 4:
            validPath = false;
            break;
        default:
            break;
        }
        tokenCount++;
    }

    // If the path has only 3 elements and it's valid (it has an NetworkAdapter +  good prefix)
    // then store the .domain (the dot is crucial) with the detected IP address
    if (validPath)
    {
        ResourceRecord r;
        r.ip = L3Address(elem->getAttribute("address"));
        r.url = "." + hostName;
        r.type = RR_Type::NS;
        records[r.url] = r;
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
    socket.bind(DNS_PORT);
}

void DNS_Service::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    // See if it is a DNS request
    auto request = dynamic_cast<const DNS_Request *>(packet->peekData().get());

    if (request == nullptr)
        error("Recieved unkown packet");

    // Retrieve IP:Port from sender
    L3Address remoteAddress = packet->getTag<L3AddressInd>()->getSrcAddress();
    // int remotePort = packet->getTag<L4PortInd>()->getSrcPort();
    int remotePort = DNS_PORT;

    // Process and package response
    auto response = selectAndExecHandler(request);
    auto responsePacket = new Packet("DNS_Request", Ptr<DNS_Request>(response));

    // Send to the requester
    socket->sendTo(responsePacket, remoteAddress, remotePort);

    // Release the memory
    delete packet;
}

void DNS_Service::socketErrorArrived(UdpSocket *socket, Indication *indication) {}

DNS_Request * DNS_Service::selectAndExecHandler(const DNS_Request *request)
{
    Handler_t handler;
    auto response = new DNS_Request(*request);

    switch (request->getOperationCode())
    {
    case OP_Code::QUERY:
        handler = &DNS_Service::handleQuery;
        break;
    case OP_Code::UPDATE_R:
        handler = &DNS_Service::handleInsert;
        break;
    case OP_Code::UPDATE_D:
        handler = &DNS_Service::handleDelete;
        break;
    default:
        handler = &DNS_Service::handleNotImplemented;
        break;
    }

    (this->*handler)(request, response);
    return response;
}

void DNS_Service::handleQuery(const DNS_Request *request, DNS_Request *response)
{
    auto questionCount = request->getQuestionArraySize();

    // If there are no questions --> Error
    if (questionCount == 0)
    {
        EV_INFO << "Query with 0 questions" << endl;
        response->setReturnCode(ReturnCode::FORMERR);
        return;
    }

    for (int i = 0; i < questionCount; i++)
    {
        try
        {
            // Attempt to retrieve the record
            auto domain = request->getQuestion(i);
            auto record = records.at(domain);

            // Insert it into the response
            response->insertRecord(record);
        }
        catch (const std::out_of_range &e)
        {
            // Ignore the exception
        };
    }

    // Indicate everything went OK
    response->setReturnCode(ReturnCode::NOERROR);

    return;
}

void DNS_Service::handleInsert(const DNS_Request *request, DNS_Request *response)
{
    auto recordCount = request->getRecordArraySize();

    // If there are no records --> Error
    if (recordCount == 0)
    {
        response->setReturnCode(ReturnCode::FORMERR);
        return;
    }

    // FIXME : Check overrides?
    for (int i = 0; i < recordCount; i++)
    {
        auto newRecord = request->getRecord(i);
        records[newRecord.url] = newRecord;
    }

    // Indicate everything went OK
    response->setReturnCode(ReturnCode::NOERROR);

    return;
}

void DNS_Service::handleDelete(const DNS_Request *request, DNS_Request *response)
{
    auto recordCount = request->getRecordArraySize();

    // If there are no records --> Error
    if (recordCount == 0)
    {
        response->setReturnCode(ReturnCode::FORMERR);
        return;
    }

    for (int i = 0; i < recordCount; i++)
    {
        auto newRecord = request->getRecord(i);
        auto pos = records.find(newRecord.url);

        // Warn about not deleted entries
        if (pos == records.end())
            EV_WARN << "Specified record for deletion (" << newRecord.url << ", " << newRecord.type << ") does not exist" << endl;
        else
            records.erase(pos);
    }

    // Indicate everything went OK
    response->setReturnCode(ReturnCode::NOERROR);

    return;
}

void DNS_Service::handleNotImplemented(const DNS_Request *request, DNS_Request *response)
{
    // Select the not implemented response
    response->setReturnCode(ReturnCode::NOTIMP);
    return;
}

void DNS_Service::printRecords()
{
    EV << "Record table: " << endl;

    // Print all the records (const & avoids copying the elements)
    for (auto const &elem : records)
        EV << elem.first << " : " << elem.second.ip << ", " << elem.second.typeToStr() << endl;
}