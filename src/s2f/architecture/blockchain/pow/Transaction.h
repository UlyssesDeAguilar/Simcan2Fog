#ifndef __POW_TRANSACTION_H__
#define __POW_TRANSACTION_H__

#include <cstddef>
#include <omnetpp.h>
namespace s2f::chain::pow
{
    struct utxo
    {
        int amount;
        int pubkey;
    };
    struct txi
    {
        int txid;
        int vout;
        int sequenceNumber;
        int signatureScript;
    };
    class Transaction
    {
      public:
        int version;
        std::vector<struct utxo> outputs;
        std::vector<struct txi> inputs;
        int locktime;

        Transaction fromJSON() { return *this; }
        int size() { return sizeof(*this); }
    };
}

#endif
