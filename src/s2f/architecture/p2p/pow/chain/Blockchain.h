#ifndef __POW_BLOCKCHAIN_H__
#define __POW_BLOCKCHAIN_H__

#include "Block.h"
#include "s2f/os/crypto/crypto.h"

namespace s2f::p2p::pow
{
    constexpr uint32_t GENESIS_NBITS = 0x1D00FFFF;
    constexpr uint64_t SATOSHIS_PER_BTC = 1e9;
    constexpr uint64_t BASE_SUBSIDY = 50 * SATOSHIS_PER_BTC;

    constexpr uint32_t BLOCK_MINETIME = 600;      //<! 10 minutes per block
    constexpr uint32_t BLOCK_DIFFUPDATE = 2016;   //<! ~2 weeks
    constexpr uint32_t HALVING_INTERVAL = 210000; //<! ~4 years

    constexpr uint32_t MAX_BLOCK_SERIALIZED_SIZE = 1000; //<! Maximum size to transmit over the network
    constexpr uint32_t COINBASE_SIZE = 80;               //<! Size of the coinbase transaction

    /**
     * Compute the compact difficulty for a blockchain at a certain height.
     * The height is 0-indexed.
     *
     * @param blockchain Target chain.
     * @param height Height of the chain where the difficulty is computed
     */
    uint32_t getDifficulty(const std::vector<Block> &blockchain, int height);

    /**
     * Return the subsidy for mining a block at a certain chain height.
     * The height is 0-indexed.
     *
     * @param height Height of the chain where the difficulty is computed
     */
    inline uint64_t getSubsidy(int height) { return BASE_SUBSIDY >> (height / HALVING_INTERVAL); }
}
#endif
