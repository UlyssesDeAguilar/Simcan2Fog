#ifndef __POW_COMMON_H_
#define __POW_COMMON_H_

#include <cstring>

namespace s2f
{
    namespace p2p
    {
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
#endif
