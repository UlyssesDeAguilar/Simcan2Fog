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
    std::array<std::byte, 2 * SHA256_DIGEST_LENGTH> buf{};

    if (transactions.empty())
        return {};

    // Compute initial TXIDs
    hashes.reserve(transactions.size());
    for (auto &t : transactions)
        hashes.push_back(t.hash());

    // Merkle root rounds
    while (hashes.size() > 1)
    {
        if (hashes.size() % 2)
            hashes.push_back(hashes.back());

        // Concatenate and re-hash TXIDs
        for (size_t i = 0; i < hashes.size() / 2; i++)
        {
            auto &left = hashes[2 * i];
            auto &right = hashes[2 * i + 1];

            std::copy(left.begin(), left.end(), buf.begin());
            std::copy(right.begin(), right.end(), buf.begin() + SHA256_DIGEST_LENGTH);

            hashes[i] = s2f::os::crypto::dsha256(buf.data(), buf.size());
        }

        hashes.resize(hashes.size() / 2);
    }

    return hashes.front();
}
