#include "Blockchain.h"
#include <algorithm>
#include <cstdint>

using namespace s2f::chain::pow;
uint32_t getDifficulty(const std::vector<Block> &blockchain, int height)
{
    int lastDiff = height - (height % BLOCK_DIFFUPDATE);
    uint32_t nBits = blockchain[lastDiff].header.nBits;
    uint32_t targetspan = BLOCK_MINETIME * BLOCK_DIFFUPDATE;
    uint32_t timespan, exponent, coefficient;

    // Only compute on new interval
    if ((height + 1) % BLOCK_DIFFUPDATE)
        return nBits;

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

uint32_t getSubsidy(const std::vector<Block> &blockchain, int height)
{
    return 0;
}
