// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONTRADETABLEMODEL_H
#define HIVEMIND_QT_DECISIONTRADETABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class marketBranch;
class marketDecision;
class marketMarket;
class marketTrade;
class DecisionTradeTablePriv;
class WalletModel;
class CWallet;


class DecisionTradeTableModel
   : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DecisionTradeTableModel(CWallet *, WalletModel * = 0);
    ~DecisionTradeTableModel();

    enum ColumnIndex {
        Address = 0,
        BuySell = 1,
        DecisionState = 2,
        NShares = 3,
        Price = 4,
        Nonce = 5,
        BlockNumber = 6,
        Hash = 7,
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole,
        AddressRole,
    };

    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    const marketTrade *index(int row) const;
    QModelIndex index(int row, int column, const QModelIndex& parent=QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    void onMarketChange(const marketBranch *, const marketDecision *, const marketMarket *);
    void getData(double **X, double **Y, unsigned int *N) const;

private:
    CWallet *wallet;
    WalletModel *walletModel;
    QStringList columns;
    DecisionTradeTablePriv *priv;

public Q_SLOTS:
    friend class DecisionTradeTablePriv;
};

QString formatAddress(const marketTrade *);
QString formatBuySell(const marketTrade *);
QString formatNShares(const marketTrade *);
QString formatPrice(const marketTrade *);
QString formatDecisionState(const marketTrade *);
QString formatNonce(const marketTrade *);
QString formatBlockNumber(const marketTrade *);
QString formatHash(const marketTrade *);

#endif // HIVEMIND_QT_DECISIONTRADETABLEMODEL_H
