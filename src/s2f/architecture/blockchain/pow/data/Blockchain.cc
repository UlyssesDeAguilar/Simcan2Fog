#include "Blockchain.h"
#include <algorithm>
#include <cstdint>

namespace s2f::chain::pow
{
    uint32_t getDifficulty(const std::vector<Block> &blockchain, int height)
    {
        if (height == -1)
            return GENESIS_NBITS;

        int lastDiff = height - (height % BLOCK_DIFFUPDATE);
        uint32_t nBits = blockchain[lastDiff].header.nBits;
        uint32_t targetspan, timespan;
        uint32_t exponent, coefficient;

        // Only compute on new interval
        if ((height + 1) % BLOCK_DIFFUPDATE)
            return nBits;

        targetspan = BLOCK_MINETIME * BLOCK_DIFFUPDATE;
        exponent = nBits >> 24;
        coefficient = nBits & 0x007FFFFF;
        timespan = blockchain[height].header.time - blockchain[lastDiff].header.time;
        timespan = std::clamp(timespan, targetspan / 4, targetspan * 4);

        coefficient *= targetspan / timespan;

        // handle overflow and underflow
        while (coefficient & ~0x007FFFFF)
        {
            coefficient >>= 8;
            exponent++;
        }
        while (coefficient && !(coefficient & 0x00800000))
        {
            coefficient <<= 8;
            exponent--;
        }
        return (exponent << 24) | coefficient;
    }
}
