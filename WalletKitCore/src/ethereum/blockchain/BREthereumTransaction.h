//
//  BBREthereumTransaction.h
//  Core Ethereum
//
//  Created by Ed Gamble on 2/21/2018.
//  Copyright © 2018-2019 Breadwinner AG.  All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.

#ifndef BR_Ethereum_Transaction_H
#define BR_Ethereum_Transaction_H

#include "ethereum/base/BREthereumBase.h"
#include "ethereum/contract/BREthereumToken.h"
#include "BREthereumNetwork.h"
#include "BREthereumTransactionStatus.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRANSACTION_NONCE_IS_NOT_ASSIGNED   UINT64_MAX

/// If we get a gasEstimate we'll want the gasLimit to have a margin over the estimate
#define GAS_LIMIT_MARGIN_PERCENT        (20)

static inline BREthereumGas
gasApplyLimitMargin (BREthereumGas gas) {
    return ethGasCreate(gas.amountOfGas);//ethGasCreate(((100 + GAS_LIMIT_MARGIN_PERCENT) * gas.amountOfGas) / 100);
}

/**
 * An Ethereum Transaction is a transaction on the Ethereum P2P network
 *
 * Per the Ethereum Specification: A transaction (formally, T) is a single cryptographically-signed
 * instruction constructed by an actor externally to the scope of Ethereum. While it is assumed
 * that the ultimate external actor will be human in nature, software tools will be used in its
 * construction and dissemination1. There are two types of transactions: those which result in
 * message calls and those which result in the creation of new accounts with associated code
 * (known informally as ‘contract creation’). Both types specify a number of common fields:
 * { nonce, gasPrice, gasLimit, to, value, {v,r,s}}.
 *
 * Additional filds are: {hash, chainId, data and status}.
 */
typedef struct BREthereumTransactionRecord *BREthereumTransaction;

extern BREthereumTransaction
transactionCreate(BREthereumAddress sourceAddress,
                  BREthereumAddress targetAddress,
                  BREthereumEther amount,
                  BREthereumGasPrice gasPrice,
                  BREthereumGas gasLimit,
                  const char *data,
                  uint64_t nonce);

extern BREthereumTransaction
transactionCopy (BREthereumTransaction transaction);

extern void
transactionRelease (BREthereumTransaction transaction);

extern void
transactionReleaseForSet (void *ignore, void *item);

extern BREthereumAddress
transactionGetSourceAddress(BREthereumTransaction transaction);

extern BREthereumAddress
transactionGetTargetAddress(BREthereumTransaction transaction);

extern BREthereumBoolean
transactionHasAddress (BREthereumTransaction transaction,
                       BREthereumAddress address);
    
extern BREthereumEther
transactionGetAmount(BREthereumTransaction transaction);

/**
 * Return the gasPrice
 */
extern BREthereumGasPrice
transactionGetGasPrice (BREthereumTransaction transaction);

/**
 * Return the gasLimit
 */
extern BREthereumGas
transactionGetGasLimit (BREthereumTransaction transaction);

/**
 * Return the gasUsed if the transaction is included.  In that case *isValid will be
 * ETHEREUM_BOOLEAN_TRUE.  If not included, then *isValid will be ETHEREUM_BOOLEAN_FALSE and the
 * gasUsed will be zero.
 */
extern BREthereumGas
transactionGetGasUsed (BREthereumTransaction transaction,
                       BREthereumBoolean *isValid);

/**
 * Return the feeBasis for transaction based on the gasLimit.  The gasLimit is an upper bound
 * on the gasUsed; thus the returned feeBasis is but an estimate.
 */
extern BREthereumFeeBasis
transactionGetFeeBasisEstimated (BREthereumTransaction transaction);

/**
 * Return the feeBasis for transaction if the transaction is included.  When included *isValid
 * will be ETHEREUM_BOOLEAN_TRUE.  If not included, then *isValid will be ETHEREUM_BOOLEAN_FALSE
 * and the returned feeBasis should not be referenced (it will be filled with zeros).
 */
extern BREthereumFeeBasis
transactionGetFeeBasisConfirmed (BREthereumTransaction transaction,
                                 BREthereumBoolean *isValid);

/**
 * Return the feeBasis for transaction.  If the transaction is confirmed (aka blocked) then
 * the value returned is the actual feeBasis paid {gasUsed, gasPrice}; if the transaction is not
 * confirmed then an estimated feeBasis is returned {gasEstimate, gasPrice}.
 */
extern BREthereumFeeBasis
transactionGetFeeBasis (BREthereumTransaction transaction);

/**
 * Return the fee (in Ether) for transaction based on `transactionGetFeeBasis()`.  If the fee
 * computation overflows, then *overflow is set to ETHEREUM_BOOLEAN_TRUE.
 */
extern BREthereumEther
transactionGetFee (BREthereumTransaction transaction,
                   BREthereumBoolean *overflow);

extern uint64_t
transactionGetNonce (BREthereumTransaction transaction);

private_extern void
transactionSetNonce (BREthereumTransaction transaction,
                     uint64_t nonce);

extern const BREthereumHash
transactionGetHash (BREthereumTransaction transaction);

// Caution
extern void
transactionSetHash (BREthereumTransaction transaction,
                    BREthereumHash hash);

extern const char * // no not modify the return value
transactionGetData (BREthereumTransaction transaction);

// Support BRSet
extern size_t
transactionHashValue (const void *h);

// Support BRSet
extern int
transactionHashEqual (const void *h1, const void *h2);

//
// Transaction Signing
//
extern void
transactionSign(BREthereumTransaction transaction,
                BREthereumSignature signature);

extern BREthereumBoolean
transactionIsSigned (BREthereumTransaction transaction);

extern BREthereumSignature
transactionGetSignature (BREthereumTransaction transaction);

/**
 * Extract the signer's address.  If not signed, an empty address is returned.
 */
extern BREthereumAddress
transactionExtractAddress(BREthereumTransaction transaction,
                          BREthereumNetwork network,
                          BRRlpCoder coder);
//
// Transaction RLP Encoding
//
extern BREthereumTransaction
transactionRlpDecode (BRRlpItem item,
                      BREthereumNetwork network,
                      BREthereumRlpType type,
                      BRRlpCoder coder);

/**
 * RLP encode transaction for the provided network with the specified type.  Different networks
 * have different RLP encodings - notably the network's chainId is part of the encoding.
 */

extern BRRlpItem
transactionRlpEncode(BREthereumTransaction transaction,
                     BREthereumNetwork network,
                     BREthereumRlpType type,
                     BRRlpCoder coder);

extern BRRlpData
transactionGetRlpData (BREthereumTransaction transaction,
                       BREthereumNetwork network,
                       BREthereumRlpType type);

extern char *
transactionGetRlpHexEncoded (BREthereumTransaction transaction,
                             BREthereumNetwork network,
                             BREthereumRlpType type,
                             const char *prefix);
//
// Transaction Comparison
//
extern BREthereumComparison
transactionCompare (BREthereumTransaction t1,
                    BREthereumTransaction t2);

extern BREthereumTransactionStatus
transactionGetStatus (BREthereumTransaction transaction);

extern void
transactionSetStatus (BREthereumTransaction transaction,
                      BREthereumTransactionStatus status);
    
extern BREthereumBoolean
transactionIsConfirmed (BREthereumTransaction transaction);

// TODO: Rethink
extern BREthereumBoolean
transactionIsSubmitted (BREthereumTransaction transaction);

extern BREthereumBoolean
transactionIsErrored (BREthereumTransaction transaction);

extern void
transactionShow (BREthereumTransaction transaction, const char *topic);

extern void
transactionsRelease (BRArrayOf(BREthereumTransaction) transactions);

//
// Transaction Result
//
typedef struct BREthereumTransactionResultRecord *BREthereumTransactionResult;

#ifdef __cplusplus
}
#endif

#endif /* BR_Ethereum_Transaction_H */
