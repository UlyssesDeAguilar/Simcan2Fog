#include "Block.h"
#include "s2f/os/crypto/crypto.h"
#include <openssl/sha.h>

using namespace s2f::chain::pow;

void Block::add(const Transaction t)
{
    transactions.push_back(t);
}

void Block::add(std::vector<Transaction> &trns)
{
    for (auto t : trns)
        transactions.push_back(t);
}

sha256digest Block::hash()
{
    return s2f::os::crypto::dsha256(&header, sizeof(header));
}

sha256digest Block::merkleRoot()
{
    std::vector<sha256digest> hashes;

    if (transactions.empty())
        return {};

    // Balance the merkle tree
    hashes.reserve(transactions.size());
    for (auto &t : transactions)
        hashes.push_back(t.hash());

    // Start hashing TXIDs
    while (hashes.size() > 1)
    {
        if (hashes.size() % 2)
            hashes.push_back(hashes.back());

        // Concatenate + hash all pairs
        for (size_t i = 0; i < hashes.size() / 2; i++)
        {
            auto &left = hashes[2 * i];
            auto &right = hashes[2 * i + 1];
            std::array<std::byte, 2 * SHA256_DIGEST_LENGTH> _;

            // NOTE: read openssl init+update+final, might save copying data.
            std::copy(left.begin(), left.end(), _.begin());
            std::copy(right.begin(), right.end(), _.begin() + SHA256_DIGEST_LENGTH);

            hashes[i] = s2f::os::crypto::sha256(_.data(), _.size());
        }

        hashes.resize(hashes.size() / 2);
    }

    return hashes.front();
}
