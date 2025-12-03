#ifndef __POW_TXCOMPARATOR_H__
#define __POW_TXCOMPARATOR_H__

#include "Transaction.h"
#include "UtxoSet.h"

namespace s2f::chain::pow
{

    /**
     * Transaction with fee metadata.
     *
     * Used as a sorting criterion for the mempool
     */
    struct TxFee
    {
        Transaction tx; //!< Transaction information
        uint64_t fee;   //!<
    };

    /**
     * @class TxComparator TxComparator.h "TxComparator.h"
     *
     * Comparator method to define transaction priority.
     *
     * @author Tomás Daniel Expósito Torre
     * @date 2025-11-20
     */
    class TxComparator
    {
      public:
        /**
         * Compares two transactions through their computing fee.
         *
         * @param tx1 Transaction A.
         * @param tx2 Transaction B.
         *
         * @return True if the fee for tx1 is lower than tx2, False otherwise.
         */
        bool operator()(const TxFee &tx1, const TxFee &tx2) { return tx1.fee < tx2.fee; }
    };
}
#endif
