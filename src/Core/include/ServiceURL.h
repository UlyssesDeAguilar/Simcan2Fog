#include "inet/networklayer/contract/ipv4/Ipv4Address.h"
#include "inet/networklayer/common/L3Address.h"

/**
 * @brief Addresses used by switches/router in Data Centres
 */
typedef inet::Ipv4Address LocalAddress;

/**
 * @brief Addresses that are actually routable
 */
typedef inet::L3Address GlobalAddress;

/**
 * @brief This struct helps with routing and identifying both resources and services
 */
struct ServiceURL
{
protected:
    LocalAddress local;
    GlobalAddress global;
    uint8_t state;

public:
    enum State
    {
        LOCAL = 1,  //!< Contains a LocalIp (equivalent to a LAN address)
        GLOBAL = 2, //!< Contains a GlobalIp
    };

    friend std::ostream &operator<<(std::ostream &os, const ServiceURL &obj)
    {
        // If we have a complete URL
        if (obj.isFullUrl())
            return os << "#" << obj.local << "@" << obj.global;

        // If we have a partial URL == Local/Global Ip
        if (obj.state & GLOBAL)
            os << obj.global;
        else
            os << "#" << obj.local;

        return os;
    }

    ServiceURL() { state = 0; }
    ServiceURL(const uint32_t &addr) : local(addr), state(LOCAL) {}
    ServiceURL(const uint32_t &addr, const GlobalAddress &global) : local(addr), global(global), state(LOCAL | GLOBAL) {}

    /**
     * @brief Construct a new Service URL from text
     * @throws cRuntimeError If the specified URL is invalid
     * @param url The url in text format
     */
    ServiceURL(const std::string &url)
    {
        // Set state to all 0
        state = 0;

        // Search for the @
        size_t npos = url.find('@');

        // If not found
        if (npos == std::string::npos)
        {
            if (url[0] == '#')
            {
                // Attempt parsing
                local.set(url.c_str() + 1);
                state = LOCAL;
            }
            else
            {
                if (!global.tryParse(url.c_str()))
                    throw inet::cRuntimeError("Invalid Ipv4 address string (global) `%s'", url.c_str());
                state = GLOBAL;
            }
        }
        else
        {
            std::string local_str = url.substr(1, npos - 1);

            // Local address parsing
            local.set(local_str.c_str());

            // Global address parsing
            global.tryParse(npos + url.c_str() + 1);

            state |= (LOCAL | GLOBAL);
        }
    }

    bool isFullUrl() const { return state == (GLOBAL | LOCAL); }

    State getState() const { return static_cast<State>(state); }

    const LocalAddress &getLocalAddress() const { return local; }

    const GlobalAddress &getGlobalAddress() const { return global; }

    void setLocalAddress(const uint32_t address) { local.set(address); }
};