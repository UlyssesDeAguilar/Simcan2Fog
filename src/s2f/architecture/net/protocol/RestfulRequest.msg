import s2f.architecture.net.protocol.RestMsg;

enum Verb
{
    GET = 0;
    POST = 1;
    PUT = 2;
    DELETE = 3;
    HEAD = 4;
};

//
//  Class inspired on HTTP v1.1 protocol 
//
class RestfulRequest extends RestMsg
{
    uint16_t port = 443;    // Optional and by default 443, works with hostname to identify the service to be reached
    Verb verb;              // Verb that acts upon the resource on the path
};