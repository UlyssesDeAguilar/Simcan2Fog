#include "PowCommon.h"

namespace s2f
{
    namespace p2p
    {
        namespace pow
        {

            Command getCommand(const char *c)
            {
                if (std::strncmp(c, "version", 7) == 0)
                    return Command::VERSION;
                if (std::strncmp(c, "verack", 6) == 0)
                    return Command::VERACK;
                if (std::strncmp(c, "getaddr", 7) == 0)
                    return Command::GETADDR;
                if (std::strncmp(c, "addr", 4) == 0)
                    return Command::ADDR;

                return Command::NOT_SUPPORTED;
            }
        }
    }
}
