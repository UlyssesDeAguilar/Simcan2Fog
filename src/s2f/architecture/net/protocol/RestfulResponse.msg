import s2f.architecture.net.protocol.RestMsg;

enum ResponseCode
{
    OK = 200;
    NOT_FOUND = 403;
    INTERNAL_ERROR = 503;
};

//
//  Class inspired on HTTP v1.1 protocol 
//
class RestfulResponse extends RestMsg
{
    ResponseCode responseCode;              // Verb that acts upon the resource on the path
};