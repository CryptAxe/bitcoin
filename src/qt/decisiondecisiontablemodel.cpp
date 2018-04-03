// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QList>

#include "guiutil.h"
#include "decisiondecisiontablemodel.h"
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
        Qt::AlignLeft|Qt::AlignVCenter, /* Prompt */
        Qt::AlignRight|Qt::AlignVCenter, /* EventOverBy */
        Qt::AlignRight|Qt::AlignVCenter, /* IsScaled */
        Qt::AlignRight|Qt::AlignVCenter, /* Minimum */
        Qt::AlignRight|Qt::AlignVCenter, /* Maximum */
        Qt::AlignRight|Qt::AlignVCenter, /* AnswerOptional */
        Qt::AlignLeft|Qt::AlignVCenter, /* Hash */
    };

// Private implementation
class DecisionDecisionTablePriv
{
public:
    DecisionDecisionTablePriv(CWallet *wallet, DecisionDecisionTableModel *parent)
      : wallet(wallet),
        parent(parent)
    {
    }

    CWallet *wallet;
    DecisionDecisionTableModel *parent;

    /* Local cache of Decisions */
    QList<const marketDecision *> cached;

    int size()
    {
        return cached.size();
    }

    const marketDecision *index(int idx)
    {
        if(idx >= 0 && idx < cached.size())
            return cached[idx];
        return 0;
    }

    QString describe(const marketDecision *decision, int unit)
    {
        return QString();
    }
};

DecisionDecisionTableModel::DecisionDecisionTableModel(CWallet *wallet, WalletModel *parent)
    : QAbstractTableModel(parent),
    wallet(wallet),
    walletModel(parent),
    priv(new DecisionDecisionTablePriv(wallet, this))
{
    columns
        << tr("Address")
        << tr("Prompt")
        << tr("Event Over By")
        << tr("IsScaled")
        << tr("Minimum")
        << tr("Maximum")
        << tr("AnswerOptional")
        << tr("Hash")
        ;
}

DecisionDecisionTableModel::~DecisionDecisionTableModel()
{
    delete priv;
}

int DecisionDecisionTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int DecisionDecisionTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant DecisionDecisionTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    const marketDecision *decision = (const marketDecision *) index.internalPointer();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case Address:
            return formatAddress(decision);
        case Prompt:
            return formatPrompt(decision);
        case EventOverBy:
            return QVariant((int)decision->eventOverBy);
        case IsScaled:
            return formatIsScaled(decision);
        case Minimum:
            return QVariant((double)decision->min*1e-8);
        case Maximum:
            return QVariant((double)decision->max*1e-8);
        case AnswerOptional:
            return formatAnswerOptional(decision);
        case Hash:
            return formatHash(decision);
        default:
            ;
        }
        break;
    case AddressRole:
        return formatAddress(decision);
    case PromptRole:
        return formatPrompt(decision);
    case Qt::TextAlignmentRole:
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant DecisionDecisionTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            case Prompt:
                return tr("Prompt");
            case EventOverBy:
                return tr("EventOverBy");
            case IsScaled:
                return tr("IsScaled");
            case Minimum:
                return tr("Minimum");
            case Maximum:
                return tr("Maximum");
            case AnswerOptional:
                return tr("Answer is Optional");
            case Hash:
                return tr("Hash");
            }
        }
    }
    return QVariant();
}

const marketDecision *DecisionDecisionTableModel::index(int row) const
{
    return priv->index(row);
}

QModelIndex DecisionDecisionTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    const marketDecision *decision = priv->index(row);
    if (decision)
        return createIndex(row, column, (void *)decision);
    return QModelIndex();
}

Qt::ItemFlags DecisionDecisionTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
DecisionDecisionTableModel::onBranchChange(const marketBranch *branch)
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

    if (!branch)
        return;

    /* insert into cache */
    vector<marketDecision *> vec = pmarkettree->GetDecisions(branch->GetHash());
    if (vec.size()) {
        beginInsertRows(QModelIndex(), 0, vec.size()-1);
        for(uint32_t i=0; i < vec.size(); i++)
            priv->cached.append(vec[i]);
        endInsertRows();
    }
}

QString formatAddress(const marketDecision *decision)
{
    CTxDestination addr(decision->keyID);
    return QString::fromStdString(EncodeDestination(addr));
}

QString formatPrompt(const marketDecision *decision)
{
    return QString::fromStdString(decision->prompt);
}

QString formatEventOverBy(const marketDecision *decision)
{
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%u", decision->eventOverBy);
    return QString(tmp);
}

QString formatIsScaled(const marketDecision *decision)
{
    return QString(decision->isScaled? "Yes": "No");
}

QString formatMaximum(const marketDecision *decision)
{
    if (decision->isScaled) {
       char tmp[32];
       snprintf(tmp, sizeof(tmp), "%.8f", decision->max*1e-8);
       return QString(tmp);
    }
    return QString("");
}

QString formatMinimum(const marketDecision *decision)
{
    if (decision->isScaled) {
       char tmp[32];
       snprintf(tmp, sizeof(tmp), "%.8f", decision->min*1e-8);
       return QString(tmp);
    }
    return QString("");
}

QString formatAnswerOptional(const marketDecision *decision)
{
    return QString(decision->answerOptionality? "Optional": "Not Optional");
}

QString formatBranchID(const marketDecision *decision)
{
    return QString::fromStdString(decision->branchid.ToString());
}

QString formatHash(const marketDecision *decision)
{
    return QString::fromStdString(decision->GetHash().ToString());
}

