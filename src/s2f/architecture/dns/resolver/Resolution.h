#ifndef SIMCAN_EX_DNS_RESOLUTION_H_
#define SIMCAN_EX_DNS_RESOLUTION_H_

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "s2f/architecture/dns/DnsRequest_m.h"

namespace s2f
{
    namespace dns
    {
        /**
         * @brief DNS resolution states
         */
        enum ResolutionState
        {
            INIT,            //!< Initial state
            TLD_RESOLUTION,  //!< TLD resolution, implies TLD discovery
            NS_RESOLUTION,   //!< NS resolution, implies NS discovery
            AUTH_RESOLUTION, //!< Authoritative resolution
            RESOLVED,        //!< The resolution was successful
            NOT_FOUND,       //!< The resolution failed
            SERVER_ERROR     //!< Server error
        };

        /**
         * @brief Struct that represents a DNS resolution and it's states
         * 
         * @author Ulysses de Aguilar Gudmundsson
         * @version 1.0
         */
        struct Resolution
        {
            uint16_t id;                    //!< Resolution id
            inet::L3Address remoteAddress;  //!< Remote address (the client)
            int remotePort;                 //!< Remote port (the client)
            DnsQuestion current_question;   //!< Current question, as the resolution advances
            ResolutionState state;          //!< Resolution state
            inet::Ptr<DnsRequest> response; //!< DNS response

            /**
             * @brief Constructor
             * 
             * @param id Resolution id
             * @param remoteAddress Address of the client
             * @param remotePort Port of the client
             * @param request The request itself
             */
            Resolution(uint16_t id, inet::L3Address remoteAddress, int remotePort, inet::Ptr<const DnsRequest> request)
            {
                this->id = id,
                this->remoteAddress = remoteAddress,
                this->remotePort = remotePort,
                current_question = request->getQuestion(0);
                response = inet::makeShared<DnsRequest>(*request);
                state = INIT;
            }

            /**
             * @brief Copy constructor
             * 
             * @param other The other object
             */
            Resolution(const Resolution &other) : id(other.id)
            {
                this->remotePort = other.remotePort;
                this->current_question = other.current_question;
                this->state = other.state;
                this->response = other.response->dup();
            }

            /**
             * @brief Default constructor
             */
            Resolution() : id(), remotePort(0), current_question(), state(INIT), response(nullptr) {}

            friend std::ostream &operator<<(std::ostream &os, const Resolution &obj)
            {
                return os << "Resolution [ip: " << obj.remoteAddress
                          << " | port: " << obj.remotePort
                          << " | id: " << obj.id
                          << " | state: " << obj.stateToString(obj.state) << " ]";
            }

            friend bool operator<(const Resolution &lhs, const Resolution &rhs) noexcept { return lhs.id < rhs.id; }

            /**
             * @brief Turns a ResolutionState integer into a string
             * 
             * @param state The ResolutionState
             * @return const char* The string representation 
             */
            static const char *stateToString(ResolutionState state)
            {
                static const char *states[] = {"INIT", "TLD_RESOLUTION", "NS_RESOLUTION", "AUTH_RESOLUTION", "RESOLVED", "NOT_FOUND"};
                return states[state];
            }

            /**
             * @brief Checks if the resolution has finished
             * 
             * @return true The resolution has finished, false otherwise
             */
            bool hasFinished() const { return state == RESOLVED || state == NOT_FOUND; }

            /**
             * @brief Get the current question
             * @return const char* The current question
             */
            const char *getCurrentQuestion() const { return current_question.getDomain(); }
        };
    }
}
#endif /* SIMCAN_EX_DNS_RESOLUTION_H_ */
