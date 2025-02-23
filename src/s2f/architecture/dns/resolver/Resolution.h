#ifndef SIMCAN_EX_DNS_RESOLUTION_H_
#define SIMCAN_EX_DNS_RESOLUTION_H_

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/DnsRequest_m.h"

namespace dns
{
    enum ResolutionState
    {
        INIT,
        TLD_RESOLUTION,
        NS_RESOLUTION,
        AUTH_RESOLUTION,
        RESOLVED,
        NOT_FOUND,
        SERVER_ERROR
    };

    struct Resolution
    {
        // Inmutable, could be used as a key
        uint16_t id;
        inet::L3Address remoteAddress;
        int remotePort;
        DnsQuestion current_question;
        ResolutionState state;
        inet::Ptr<DnsRequest> response;

        Resolution(uint16_t id, inet::L3Address remoteAddress, int remotePort, inet::Ptr<const DnsRequest> request)
        {
            this->id = id,
            this->remoteAddress = remoteAddress,
            this->remotePort = remotePort,
            current_question = request->getQuestion(0);
            response = inet::makeShared<DnsRequest>(*request);
            state = INIT;
        }

        Resolution(const Resolution &other) : id(other.id)
        {
            this->remotePort = other.remotePort;
            this->current_question = other.current_question;
            this->state = other.state;
            this->response = other.response->dup();
        }

        Resolution() : id(), remotePort(0), current_question(), state(INIT), response(nullptr) {}

        friend std::ostream &operator<<(std::ostream &os, const Resolution &obj)
        {
            return os << "Resolution [ip: " << obj.remoteAddress
                      << " | port: " << obj.remotePort
                      << " | id: " << obj.id
                      << " | state: " << obj.stateToString(obj.state) << " ]";
        }

        friend bool operator<(const Resolution &lhs, const Resolution &rhs) noexcept { return lhs.id < rhs.id; }

        static const char *stateToString(ResolutionState state)
        {
            static const char *states[] = {"INIT", "TLD_RESOLUTION", "NS_RESOLUTION", "AUTH_RESOLUTION", "RESOLVED", "NOT_FOUND"};
            return states[state];
        }

        bool hasFinished() const { return state == RESOLVED || state == NOT_FOUND; }
        const char* getCurrentQuestion() const { return current_question.getDomain(); }
    };
}
#endif /* SIMCAN_EX_DNS_RESOLUTION_H_ */
