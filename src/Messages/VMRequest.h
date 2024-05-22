#ifndef _VM_REQUEST
#define _VM_REQUEST

#include "Core/include/SIMCAN_types.h"

struct VM_Response
{
    enum State{
        NOT_ASSIGNED,
        ACCEPTED,
        REJECTED
    };
    double startTime = double();
    int price = int();
    State state = NOT_ASSIGNED;
    std::string ip;

    friend std::ostream& operator<<(std::ostream &os, const VM_Response &obj);
    friend std::ostream& operator<<(std::ostream &os, const State &obj);
};

/**
 * @brief Request that includes the caracteristics needed to satisfy the VM needs
 */
struct VM_Request
{
public:
    /**
     * @brief Times associated with a VM instance
     */
    struct InstanceRequestTimes
    {
        double maxStartTime;        // T1
        double rentTime;            // T2
        double maxSubTime;          // T3
        double maxSubscriptionTime; // T4
    };

    InstanceRequestTimes times;
    std::string vmType;
    std::string vmId;
    VM_Response response;

    bool operator==(const VM_Request &other) { return this->vmId == other.vmId; }
    friend std::ostream& operator<<(std::ostream &os, const VM_Request &obj);
};

#endif /*_VM_REQUEST */