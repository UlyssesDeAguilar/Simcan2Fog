#include "VMRequest.h"

std::ostream& operator<<(std::ostream &os, const VM_Response::State &obj)
{
    switch (obj)
    {
    case VM_Response::NOT_ASSIGNED:
        os << "Not assigned";
        break;
    case VM_Response::ACCEPTED:
        os << "Accepted";
        break;
    case VM_Response::REJECTED:
        os << "Rejected";
        break;
    default:
        os.setstate(std::ios_base::failbit);
    }

    return os;
}
std::ostream &operator<<(std::ostream &os, const VM_Response &obj)
{
    return os << "    +RSP [ State:" << obj.state
              << " | Ip:  " << obj.ip 
              << " | RentStart: " << obj.startTime 
              << " | Price:" << obj.price << "]";
}

std::ostream &operator<<(std::ostream &os, const VM_Request &obj)
{
    return os << "    +RQ[ Type:" << obj.vmType
              << " | Id: " << obj.vmId 
              << " | MaxStart:  " << obj.times.maxStartTime
              << " | RentTime: " << obj.times.rentTime << "]" << '\n'
              << obj.response;
}