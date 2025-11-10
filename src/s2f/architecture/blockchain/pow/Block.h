#ifndef __POW_BLOCK_H__
#define __POW_BLOCK_H__

#include "Transaction.h"
#include "s2f/os/crypto/crypto.h"
#include <omnetpp.h>
#include <openssl/sha.h>

namespace s2f::chain::pow
{
    struct BlockHeader
    {
        int32_t version;
        sha256digest parentBlockHash;
        sha256digest merkleRootHash;
        uint32_t time;
        uint32_t nBits;
        uint32_t nonce;
    };

    class Block
    {
      public:
        BlockHeader header;
        std::vector<Transaction> transactions;

        /**
         * Computes the hash for this block.
         *
         * @return sha256 value of the block header over two iterations.
         */
        sha256digest hash() const { return os::crypto::dsha256(&header, sizeof(header)); }

        /**
         * Adds a transaction to this block.
         *
         * @param t transaction object.
         */
        void add(const Transaction t) { transactions.push_back(t); }

        /**
         * Adds a list of transactions to this block.
         *
         * @param trns  List of transactions to insert.
         */
        void add(std::vector<Transaction> &trns) { transactions.insert(transactions.end(), trns.begin(), trns.end()); }

        /**
         * Verifies whether the block header information is a valid element of the chain.
         *
         * @param parent    The previous element in the chain.
         */
        bool isValid(Block &parent) const { return header.parentBlockHash == parent.hash() && header.merkleRootHash == merkleRoot(); }

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
