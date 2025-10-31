#ifndef __P2P_POW_COMMON_H__
#define __P2P_POW_COMMON_H__

#include <cstdint>
#include <cstring>

namespace s2f
{
    namespace p2p
    {
        namespace pow
        {
            /* Network start string*/
            constexpr uint32_t MAIN_NET = 0xF9BEB4D9; //<! Main network
            constexpr uint32_t TEST_NET = 0x0B110907; //<! Test network
            constexpr uint32_t REG_TEST = 0xFABFB5DA; //<! Reg test

            /* Node service types */
            constexpr uint64_t UNNAMED = 0x00;                //<! Not a full node, may only be ble to provide transactions
            constexpr uint64_t NODE_NETWORK = 0x01;           //<! Full node
            constexpr uint64_t NODE_GETUTXO = 0x02;           //<! Full node with getutxo support
            constexpr uint64_t NODE_BLOOM = 0x04;             //<! Full node with bloom-filtered connections
            constexpr uint64_t NODE_WITNESS = 0x08;           //<! Full node with support for witness data in blocks
            constexpr uint64_t NODE_XTHIN = 0x10;             //<! Full node with support for Xtreme Thinblocks.
            constexpr uint64_t NODE_NETWORK_LIMITED = 0x0400; //<! Same as 0x01 with restrictions from BIP159

            /* Ping-Pong connectivity */
            constexpr uint64_t PONG_TIMEOUT_SECONDS = 1200; //<! for pingpong message connectivity
            constexpr uint64_t PING_POLLING_MIN = 60;       //<! minimum time before sending ping
            constexpr uint64_t PING_POLLING_MAX = 600;      //<! maximum time before sending ping

            enum PowEvent
            {
                SEND_PING = 8,
                AWAITING_POLL,
            };

        }
    }
}
#endif
