#include "UtxoSet.h"
#include <numeric>

using namespace s2f::chain::pow;

int UtxoSet::getCoin(const sha256digest &txid, int vout)
{
    auto it = database.find(txid);

    if (vout < 0 || it == database.end() || vout > it->second.size())
        return -1;

    return it->second[vout].amount;
}

void UtxoSet::spendCoin(const sha256digest &txid, int vout)
{
    auto it = database.find(txid);

    if (vout < 0 || it == database.end() || vout > it->second.size())
        return;

    auto &outputs = it->second;
    outputs[vout].amount = -1;

    // Remove transaction once outputs are spent
    if (std::reduce(outputs.begin(), outputs.end(), 0) == -outputs.size())
        database.erase(txid);
}
