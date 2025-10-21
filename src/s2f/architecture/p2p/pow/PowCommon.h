#ifndef __POW_COMMON_H_
#define __POW_COMMON_H_

#include <cstdint>
#include <cstring>

namespace s2f
{
    namespace p2p
    {
        namespace pow
        {
            constexpr uint64_t UNNAMED = 0x00;                // Not a full node, may only be ble to provide transactions
            constexpr uint64_t NODE_NETWORK = 0x01;           // Full node
            constexpr uint64_t NODE_GETUTXO = 0x02;           // Full node with getutxo support
            constexpr uint64_t NODE_BLOOM = 0x04;             // Full node with bloom-filtered connections
            constexpr uint64_t NODE_WITNESS = 0x08;           // Full node with support for witness data in blocks
            constexpr uint64_t NODE_XTHIN = 0x10;             // Full node with support for Xtreme Thinblocks.
            constexpr uint64_t NODE_NETWORK_LIMITED = 0x0400; // Same as 0x01 with restrictions from BIP159
                                                              //
            /**
             * Command options for a PoW message. Based on bitcoin networking:
             * https://developer.bitcoin.org/reference/p2p_networking.html#version
             */
            enum Command
            {
                VERSION,
                VERACK,
                GETADDR,
                ADDR,
                NOT_SUPPORTED
            };

            /**
             * Parses a string as its corresponding command.
             *
             * @param c    Command string.
             */
            Command getCommand(const char *c);
        }
    }
}
#endif
