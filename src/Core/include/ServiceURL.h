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
    short state;

    void _guard() const
    {
        if (!(state & INITIALIZED))
            throw std::logic_error("Unset Service URL");
    }

public:
    enum State
    {
        LOCAL,          //!< Contains a LocalIp (equivalent to a LAN address)
        GLOBAL = 2,     //!< Contains a GlobalIp
        INITIALIZED = 4 //!< Wheter it was initialized or not
    };

    friend std::ostream &operator<<(std::ostream &os, const ServiceURL &obj)
    {
        // If we have a complete URL
        if (obj.isFullUrl())
            return os << "L" << obj.local << "@" << obj.global;

        // If we have a partial URL == Local/Global Ip
        if (obj.state & GLOBAL)
            os << obj.global;
        else
            os << "L" << obj.local;

        return os;
    }

    ServiceURL() { state = 0; }

    /**
     * @brief Construct a new Service URL from text
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
            if (url[0] == 'L')
            {
                // Attempt parsing
                local.set(url.c_str() + 1);
                state = LOCAL;
            }
            else
            {
                global.tryParse(url.c_str());
                state = GLOBAL;
            }
        }
        else
        {
            std::string local_str = url.substr(1, npos);

            // Local address parsing
            local.set(local_str.c_str());

            // Global address parsing
            global.tryParse(npos + url.c_str() + 1);

            state |= (LOCAL | GLOBAL);
        }

        // Mark as initialized
        state |= INITIALIZED;
    }

    bool isFullUrl() const { return ((~state) & (GLOBAL|LOCAL)) == 0;}

    State getState() const { return static_cast<State>(state); }

    const LocalAddress &getLocalAddress() const
    {
        _guard();
        return local;
    }

    const GlobalAddress &getGlobalAddress() const
    {
        _guard();
        return global;
    }
};