#ifndef __POW_BLOCK_H__
#define __POW_BLOCK_H__

#include "TxComparator.h"
#include "omnetpp/clog.h"
#include "s2f/os/crypto/crypto.h"
#include <algorithm>
#include <cstdint>
#include <omnetpp.h>
#include <openssl/sha.h>
#include <optional>

using namespace s2f::os::crypto;

namespace s2f::chain::pow
{

    constexpr sha256digest GENESIS_HASH = {}; //<! Empty first block hash

    /**
     * Data representation of a block header
     */
    struct BlockHeader
    {
        int32_t version;              //<! Block version
        sha256digest parentBlockHash; //<! Hash of the previous block
        sha256digest merkleRootHash;  //<! Merkle root of the transaction list
        uint32_t time;                //<! Time of mining
        uint32_t nBits;               //<! Target at the time of mining
        uint32_t nonce;               //<! Mining value

        /**
         * Computes the hashing target from the nBits value.
         *
         * @param fakeness  Difficulty easing factor. Makes the required hash require
         *                  N leading zeroes less.
         */
        sha256digest getTarget(int fakeness) const
        {
            sha256digest target{};

            int i = std::clamp<int>(32 - (nBits >> 24) - fakeness, 0, 32 - (nBits >> 24));
            uint32_t coefficient = nBits & 0x00FFFFFF;

            target[i] = std::byte((coefficient >> 16) & 0xFF);
            target[i + 1] = std::byte((coefficient >> 8) & 0xFF);
            target[i + 2] = std::byte(coefficient & 0xFF);

            return target;
        }
    };

    /**
     * @class Block Block.h "Block.h"
     *
     * Data representation of a block within bitcoin.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-11-08
     */
    class Block
    {
      public:
        BlockHeader header;
        std::vector<TxFee> transactions;

        Block(const Block *parent, uint32_t difficulty);

        /**
         * Computes the hash for this block.
         *
         * @return sha256 value of the block header over two iterations.
         */
        sha256digest hash() const { return dsha256(&header, sizeof(header)); }

        /**
         * Verifies whether the block header information is a valid element of the chain.
         *
         * @param parent    The previous element in the chain.
         */
        bool isValid(const Block *parent) const { return header.parentBlockHash == (parent ? parent->hash() : GENESIS_HASH) && header.merkleRootHash == merkleRoot(); }

        /**
         * Computes the merkle root hash of this block. This value is computed
         * by the miner node after appending a coinbase transaction, and then
         * by any other node which wishes to validate a received block.
         *
         * @return The hash value, or an array of 0 on an empty block.
         */
        sha256digest merkleRoot() const;
    };
}
#endif
