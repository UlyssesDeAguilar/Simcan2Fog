#include "PowCommon.h"

namespace s2f
{
    namespace p2p
    {
        Command getCommand(const char *c)
        {
            if (std::strncmp(c, "version", 7) == 0)
                return Command::VERSION;
            if (std::strncmp(c, "verack", 6) == 0)
                return Command::VERACK;

            return Command::VERACK;
        }
    }
}
