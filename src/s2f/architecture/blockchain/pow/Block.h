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
        std::vector<Transaction> transactions; // TODO: define transaction order

        /**
         * Computes the hash for this block.
         *
         * @return sha256 value of the block header over two iterations.
         */
        sha256digest hash();

        /**
         * Adds a transaction to this block.
         *
         * @param t transaction object.
         */
        void add(const Transaction t);

        /**
         * Adds a list of transactions to this block.
         *
         * @param trns  List of transactions to insert.
         */
        void add(std::vector<Transaction> &trns);

        /**
         * Verifies whether the block header information is a valid element of the chain.
         *
         * @param parent    The previous element in the chain.
         */
        bool isValid(Block &parent)
        {
            return header.parentBlockHash == parent.hash() && header.merkleRootHash == merkleRoot();
        }

      private:
        /**
         * Computes the merkle root hash of this block.
         *
         * @return The hash value, or an array of 0 on an empty block.
         */
        sha256digest merkleRoot();
    };
}
#endif
