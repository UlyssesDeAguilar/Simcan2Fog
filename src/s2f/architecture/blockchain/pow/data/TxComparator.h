#ifndef __POW_TXCOMPARATOR_H__
#define __POW_TXCOMPARATOR_H__

#include "Transaction.h"
#include "UtxoSet.h"

namespace s2f::chain::pow
{

    struct TxFee
    {
        Transaction t;
        uint64_t fee;
    };

    class TxComparator
    {
      public:
        bool operator()(const TxFee &tx1, const TxFee &tx2) { return tx1.fee > tx2.fee; }
    };
}
#endif
