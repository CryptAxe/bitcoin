// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <QList>

#include "guiutil.h"
#include "decisionbranchtablemodel.h"
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
        Qt::AlignLeft|Qt::AlignVCenter, /* name */
        Qt::AlignLeft|Qt::AlignVCenter, /* description */
        Qt::AlignLeft|Qt::AlignVCenter, /* baseListingFee */
        Qt::AlignRight|Qt::AlignVCenter, /* freeDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* targetDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* maxDecisions */
        Qt::AlignRight|Qt::AlignVCenter, /* minTradingFee */
        Qt::AlignRight|Qt::AlignVCenter, /* tau */
        Qt::AlignRight|Qt::AlignVCenter, /* ballotTime */
        Qt::AlignRight|Qt::AlignVCenter, /* unsealTime */
        Qt::AlignRight|Qt::AlignVCenter, /* consensusThreshold */
        Qt::AlignRight|Qt::AlignVCenter, /* alpha */
        Qt::AlignRight|Qt::AlignVCenter, /* tol */
        Qt::AlignLeft|Qt::AlignVCenter, /* Hash */
    };


// Private implementation
class DecisionBranchTablePriv
{
public:
    DecisionBranchTablePriv(CWallet *wallet, DecisionBranchTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    DecisionBranchTableModel *parent;

    /* Local cache of Branches */
    QList<const marketBranch *> cached;

    int size()
    {
        return cached.size();
    }

    const marketBranch *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketBranch *branch, int unit)
    {
        return QString();
    }
};

DecisionBranchTableModel::DecisionBranchTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new DecisionBranchTablePriv(wallet, this))
{
    columns
        << tr("Name")
        << tr("Description")
        << tr("Base Listing Fee")
        << tr("Free Decisions")
        << tr("Target Decisions")
        << tr("Max Decisions")
        << tr("Min Trading Fee")
        << tr("Tau")
        << tr("Ballot Time")
        << tr("Unseal Time")
        << tr("ConsensusThreshold")
        << tr("Alpha")
        << tr("Tol")
        << tr("Hash")
        ;
}

DecisionBranchTableModel::~DecisionBranchTableModel()
{
    delete priv;
}

int DecisionBranchTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int DecisionBranchTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant DecisionBranchTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketBranch *branch = (const marketBranch *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Name:
            return formatName(branch);
        case Description:
            return formatDescription(branch);
        case BaseListingFee:
            return QVariant((double)branch->baseListingFee*1e-8);
        case FreeDecisions:
            return QVariant((int)branch->freeDecisions);
        case TargetDecisions:
            return QVariant((int)branch->targetDecisions);
        case MaxDecisions:
            return QVariant((int)branch->maxDecisions);
        case MinTradingFee:
            return QVariant((double)branch->minTradingFee*1e-8);
        case Tau:
            return QVariant((int)branch->tau);
        case BallotTime:
            return QVariant((int)branch->ballotTime);
        case UnsealTime:
            return QVariant((int)branch->unsealTime);
        case ConsensusThreshold:
            return QVariant((double)branch->consensusThreshold*1e-8);
        case Alpha:
            return QVariant((double)branch->alpha*1e-8);
        case Tol:
            return QVariant((double)branch->tol*1e-8);
        case Hash:
            return formatHash(branch);
        default:
            ;
        }
        break;
    case DescriptionRole:
        return formatDescription(branch);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant DecisionBranchTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Name:
                return tr("Name.");
            case Description:
                return tr("Description.");
            case BaseListingFee:
                return tr("BaseListingFee.");
            case FreeDecisions:
                return tr("FreeDecisions.");
            case TargetDecisions:
                return tr("TargetDecisions.");
            case MaxDecisions:
                return tr("MaxDecisions.");
            case MinTradingFee:
                return tr("MinTradingFee.");
            case Tau:
                return tr("Tau.");
            case BallotTime:
                return tr("BallotTime.");
            case UnsealTime:
                return tr("UnsealTime.");
            case ConsensusThreshold:
                return tr("ConsensusThreshold.");
            case Alpha:
                return tr("Alpha.");
            case Tol:
                return tr("Tol.");
            case Hash:
                return tr("Hash.");
            }
        }
    }
    return QVariant();
}

const marketBranch *DecisionBranchTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex DecisionBranchTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketBranch *branch = priv->index(row);
    if (branch)
        return createIndex(row, column, (void *)branch);
    return QModelIndex();
}

Qt::ItemFlags DecisionBranchTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
DecisionBranchTableModel::setTable(void)
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

    /* insert into cache */
    vector<marketBranch *> vec = pmarkettree->GetBranches();
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(size_t i=0; i < vec.size(); i++)
            priv->cached.append(vec[i]);
        endInsertRows();
    }
}

QString formatName(const marketBranch *branch)
{
    return QString::fromStdString(branch->name);
}

QString formatDescription(const marketBranch *branch)
{
    return QString::fromStdString(branch->description);
}

QString formatBaseListingFee(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", branch->baseListingFee*1e-8);
    return QString(tmp);
}

QString formatFreeDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->freeDecisions);
    return QString(tmp);
}

QString formatTargetDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->targetDecisions);
    return QString(tmp);
}

QString formatMaxDecisions(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->maxDecisions);
    return QString(tmp);
}

QString formatMinTradingFee(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.8f", branch->minTradingFee*1e-8);
    return QString(tmp);
}

QString formatTau(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->tau);
    return QString(tmp);
}

QString formatBallotTime(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->ballotTime);
    return QString(tmp);
}

QString formatUnsealTime(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", branch->unsealTime);
    return QString(tmp);
}

QString formatConsensusThreshold(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.3f", branch->consensusThreshold*1e-8);
    return QString(tmp);
}

QString formatAlpha(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.3f", branch->alpha*1e-8);
    return QString(tmp);
}

QString formatTol(const marketBranch *branch)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%.3f", branch->tol*1e-8);
    return QString(tmp);
}

QString formatHash(const marketBranch *branch)
{
    return QString::fromStdString(branch->GetHash().ToString());
}

