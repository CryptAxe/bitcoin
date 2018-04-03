// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "guiutil.h"
#include "decisionmarkettablemodel.h"
#include "key_io.h"
#include "primitives/market.h"
#include "sync.h"
#include "txdb.h"
#include "uint256.h"
#include "util.h"
#include "wallet/wallet.h"
#include "walletmodel.h"

extern CMarketTreeDB *pmarkettree;


// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* Address */
        Qt::AlignRight|Qt::AlignVCenter, /* B */
        Qt::AlignRight|Qt::AlignVCenter, /* TradingFee */
        Qt::AlignRight|Qt::AlignVCenter, /* MaxCommision */
        Qt::AlignLeft|Qt::AlignVCenter, /* Title */
        Qt::AlignLeft|Qt::AlignVCenter, /* Description */
        Qt::AlignLeft|Qt::AlignVCenter, /* Tags */
        Qt::AlignRight|Qt::AlignVCenter, /* Maturation */
        Qt::AlignLeft|Qt::AlignVCenter, /* Hash */
    };

// Private implementation
class DecisionMarketTablePriv
{
public:
    DecisionMarketTablePriv(CWallet *wallet, DecisionMarketTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    DecisionMarketTableModel *parent;

    /* Local cache of Markets */
    QList<const marketMarket *> cached;

    int size()
    {
        return cached.size();
    }

    const marketMarket *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketMarket *market, int unit)
    {
        return QString();
    }
};

DecisionMarketTableModel::DecisionMarketTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new DecisionMarketTablePriv(wallet, this))
{
    columns
        << tr("Address")
        << tr("Liquidity Parameter")
        << tr("Trading Fee")
        << tr("Max Commission")
        << tr("Title")
        << tr("Description")
        << tr("Tags")
        << tr("Maturation")
        << tr("Decision IDs")
        << tr("Hash")
        ;
}

DecisionMarketTableModel::~DecisionMarketTableModel()
{
    // unsubscribeFromCoreSignals();
    delete priv;
}

int DecisionMarketTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int DecisionMarketTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant DecisionMarketTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketMarket *market = (const marketMarket *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Address:
            return formatAddress(market);
        case B:
            return QVariant((double)market->B*1e-8);
        case TradingFee:
            return QVariant((double)market->tradingFee*1e-8);
        case MaxCommission:
            return QVariant((double)market->maxCommission*1e-8);
        case Title:
            return formatTitle(market);
        case Description:
            return formatDescription(market);
        case Tags:
            return formatTags(market);
        case Maturation:
            return QVariant((int)market->maturation);
        case DecisionIDs:
            return formatDecisionIDs(market);
        case Hash:
            return formatHash(market);
        default:
            ;
        }
        break;
    case AddressRole:
        return formatAddress(market);
    case TitleRole:
        return formatTitle(market);
    case DescriptionRole:
        return formatDescription(market);
    case DecisionIDsRole:
        return formatDecisionIDs(market);
    case TagsRole:
        return formatTags(market);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant DecisionMarketTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case B:
                return tr("Liquidity Parameter");
            case TradingFee:
                return tr("Trading Fee");
            case MaxCommission:
                return tr("Maximum Commission");
            case Title:
                return tr("Title");
            case Description:
                return tr("Description");
            case Tags:
                return tr("Tags");
            case Maturation:
                return tr("Maturation");
            case DecisionIDs:
                return tr("Decision IDs");
            case Hash:
                return tr("Hash");
            }
        }
    }
    return QVariant();
}

const marketMarket *DecisionMarketTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex DecisionMarketTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketMarket *market = priv->index(row);
    if (market)
        return createIndex(row, column, (void *)market);
    return QModelIndex();
}

Qt::ItemFlags DecisionMarketTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
DecisionMarketTableModel::onDecisionChange(const marketBranch *branch, const marketDecision *decision)
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

    if (!branch || !decision)
        return;

    /* insert into cache */
    vector<marketMarket *> vec = pmarkettree->GetMarkets(decision->GetHash());
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++)
            priv->cached.append(vec[i]);
        endInsertRows();
    }
}

QString formatAddress(const marketMarket *market)
{
    CTxDestination addr(market->keyID);
    return QString::fromStdString(EncodeDestination(addr));
}

QString formatB(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", market->B*1e-8);
    return QString(tmp);
}

QString formatTradingFee(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", market->tradingFee*1e-8);
    return QString(tmp);
}

QString formatMaxCommission(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", market->maxCommission*1e-8);
    return QString(tmp);
}

QString formatTitle(const marketMarket *market)
{
    return QString::fromStdString(market->title);
}

QString formatDescription(const marketMarket *market)
{
    return QString::fromStdString(market->description);
}

QString formatTags(const marketMarket *market)
{
    return QString::fromStdString(market->tags);
}

QString formatMaturation(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", market->maturation);
    return QString(tmp);
}

QString formatDecisionIDs(const marketMarket *market)
{
    const vector<uint256> &decisionIDs = market->decisionIDs;
    const vector<uint8_t> &decisionFunctionIDs = market->decisionFunctionIDs;
    if (decisionIDs.size() != decisionFunctionIDs.size())
       return QString("");

    QString str;
    for(uint32_t i=0; i < decisionIDs.size(); i++) {
       str += decisionIDs[i].ToString().c_str();
       str += ':';
       str += decisionFunctionIDToString( decisionFunctionIDs[i] ).c_str();
       if (i + 1 != decisionIDs.size())
          str += '\n';
    }
    return str;
}

QString formatDecisionFunctionIDs(const marketMarket *market)
{
    return QString("");
}

QString formatTxPoWh(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", market->txPoWh);
    return QString(tmp);
}

QString formatTxPoWd(const marketMarket *market)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", market->txPoWd);
    return QString(tmp);
}

QString formatHash(const marketMarket *market)
{
    return QString::fromStdString(market->GetHash().ToString());
}

