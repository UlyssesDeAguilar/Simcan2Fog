#include "Block.h"
#include "s2f/os/crypto/crypto.h"
#include <openssl/sha.h>

using namespace s2f::p2p::pow;
using namespace s2f::os::crypto;

Block::Block(const Block *parent, uint32_t difficulty)
{
    header = {
        .version = 1,
        .parentBlockHash = parent ? parent->hash() : GENESIS_HASH,
        .merkleRootHash = {},
        .time = static_cast<uint32_t>(time(nullptr)),
        .nBits = difficulty,
        .nonce = 0,
    };
}
sha256digest Block::merkleRoot() const
{
    std::vector<sha256digest> hashes;
    std::array<std::byte, 2 * SHA256_DIGEST_LENGTH> buf{};

    if (transactions.empty())
        return {};

    // Compute initial TXIDs
    hashes.reserve(transactions.size());
    for (auto &txfee : transactions)
        hashes.push_back(txfee.tx.hash());

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

            hashes[i] = dsha256(buf.data(), buf.size());
        }

        hashes.resize(hashes.size() / 2);
    }

    return hashes.front();
}
