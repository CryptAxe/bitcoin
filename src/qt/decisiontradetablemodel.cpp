// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "guiutil.h"
#include "decisiontradetablemodel.h"
#include "key_io.h"
#include "primitives/market.h"
#include "sync.h"
#include "txdb.h"
#include "uint256.h"
#include "util.h"
#include "validation.h"
#include "wallet/wallet.h"
#include "walletmodel.h"

// TODO replace with unique pointer extern and remove
// validation.h dependency?
//extern CMarketTreeDB *pmarkettree;

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* Address */
        Qt::AlignRight|Qt::AlignVCenter, /* BuySell */
        Qt::AlignRight|Qt::AlignVCenter, /* DecisionState */
        Qt::AlignRight|Qt::AlignVCenter, /* NShares */
        Qt::AlignRight|Qt::AlignVCenter, /* Price */
        Qt::AlignRight|Qt::AlignVCenter, /* Nonce */
        Qt::AlignRight|Qt::AlignVCenter, /* BlockNum */
        Qt::AlignLeft|Qt::AlignVCenter, /* Hash */
    };

// Private implementation
class DecisionTradeTablePriv
{
public:
    DecisionTradeTablePriv(CWallet *wallet, DecisionTradeTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    DecisionTradeTableModel *parent;

    /* Local cache of Trades */
    QList<const marketTrade *> cached;

    int size()
    {
        return cached.size();
    }

    const marketTrade *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketTrade *trade, int unit)
    {
        return QString();
    }
};

DecisionTradeTableModel::DecisionTradeTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new DecisionTradeTablePriv(wallet, this))
{
    columns
        << tr("Address")
        << tr("Buy/Sell")
        << tr("DecisionState")
        << tr("Shares")
        << tr("Price")
        << tr("Nonce")
        << tr("Block")
        << tr("Hash")
        ;
}

DecisionTradeTableModel::~DecisionTradeTableModel()
{
    delete priv;
}

int DecisionTradeTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int DecisionTradeTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant DecisionTradeTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketTrade *trade = (const marketTrade *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Address:
            return formatAddress(trade);
        case BuySell:
            return formatBuySell(trade);
        case DecisionState:
            return formatDecisionState(trade);
        case NShares:
            return QVariant((double)trade->nShares*1e-8);
        case Price:
            return QVariant((double)trade->price*1e-8);
        case Nonce:
            return QVariant((int)trade->nonce);
        case BlockNumber:
            return QVariant((int)trade->nHeight);
        case Hash:
            return formatHash(trade);
        default:
            ;
        }
        break;
    case AddressRole:
        return formatAddress(trade);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant DecisionTradeTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
            return columns[section];
        else
        if (role == Qt::TextAlignmentRole)
            return column_alignments[section];
        else
        if (role == Qt::ToolTipRole)
        {
            switch(section)
            {
            case Address:
                return tr("Address");
            case BuySell:
                return tr("Buy or Sell");
            case DecisionState:
                return tr("DecisionState");
            case NShares:
                return tr("Number of Shares");
            case Price:
                return tr("Price per Share");
            case Nonce:
                return tr("Nonce");
            case BlockNumber:
                return tr("BlockNumber");
            case Hash:
                return tr("Hash");
            }
        }
    }
    return QVariant();
}

const marketTrade *DecisionTradeTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex DecisionTradeTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketTrade *trade = priv->index(row);
    if (trade)
        return createIndex(row, column, (void *)trade);
    return QModelIndex();
}

Qt::ItemFlags DecisionTradeTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void DecisionTradeTableModel::onMarketChange(
    const marketBranch *branch,
    const marketDecision *decision,
    const marketMarket *market)
{
    if (!priv)
        return;

    /* erase cache */
    if (priv->cached.size()) {
        beginRemoveRows(QModelIndex(), 0, priv->cached.size()-1);
        for(ssize_t i=0; i < priv->cached.size(); i++)
            delete priv->cached[i];
        priv->cached.clear();
        endRemoveRows();
    }

    if (!branch || !decision || !market)
        return;

    /* insert into cache */
    vector<marketTrade *> vec = pmarkettree->GetTrades(market->GetHash());
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++) {
            InsertMarketObjectHeight(vec[i]);
            priv->cached.append(vec[i]);
        }
        endInsertRows();
    }
}

void DecisionTradeTableModel::getData(double **Xptr, double **Yptr, unsigned int *Nptr) const
{
    if (!Xptr || !Yptr || !Nptr || !priv)
        return;

    const QList<const marketTrade *> &cached = priv->cached;
    unsigned int N = *Nptr = cached.size();
    if (N) {
        double *X = *Xptr = new double [N];
        double *Y = *Yptr = new double [N];
        unsigned int count = 0;
        for(unsigned int i=0; i < N; i++) {
            const marketTrade *trade = cached[i];
            if (trade->nHeight == (uint32_t)(-1))
                continue;
            uint8_t is_buy = (trade->decisionState == 1)? 1: 0;
            uint8_t is_sell = (trade->decisionState == 0)? 1: 0;
            if (is_buy || is_sell) {
               double price = trade->price * 1e-8;
               X[count] = (double) trade->nHeight;
               Y[count] = (is_buy)? price: (1.0 - price);
               count++;
            }
        }
        *Nptr = count;
    }
}

QString formatAddress(const marketTrade *trade)
{
    CTxDestination addr(trade->keyID);
    return QString::fromStdString(EncodeDestination(addr));
}

QString formatBuySell(const marketTrade *trade)
{
   return QString(trade->isBuy? "Buy": "Sell");
}

QString formatBuyOrSell(const marketTrade *trade)
{
    return QString(trade->isBuy? "Buy": "Sell");
}

QString formatNShares(const marketTrade *trade)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", trade->nShares*1e-8);
    return QString(tmp);
}

QString formatPrice(const marketTrade *trade)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", trade->price*1e-8);
    return QString(tmp);
}

QString formatDecisionState(const marketTrade *trade)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", trade->decisionState);
    return QString(tmp);
}

QString formatNonce(const marketTrade *trade)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", trade->nonce);
    return QString(tmp);
}

QString formatBlockNumber(const marketTrade *trade)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%06u", trade->nHeight);
    return QString(tmp);
}

QString formatHash(const marketTrade *trade)
{
    return QString::fromStdString(trade->GetHash().ToString());
}

