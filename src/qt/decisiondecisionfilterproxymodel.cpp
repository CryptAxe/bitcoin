// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "decisiondecisionfilterproxymodel.h"
#include "decisiondecisiontablemodel.h"


bool DecisionDecisionFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString address = index.data(DecisionDecisionTableModel::AddressRole).toString();
    if (!address.contains(filterAddress, Qt::CaseSensitive))
        return false;

    QString prompt = index.data(DecisionDecisionTableModel::PromptRole).toString();
    if (!prompt.contains(filterPrompt, Qt::CaseInsensitive))
        return false;

    return true;
}

void DecisionDecisionFilterProxyModel::setFilterAddress(const QString &str)
{
    filterAddress = (str.size() >= 2)? str: "";
    invalidateFilter();
}

void DecisionDecisionFilterProxyModel::setFilterPrompt(const QString &str)
{
    filterPrompt = (str.size() >= 2)? str: "";
    invalidateFilter();
}

