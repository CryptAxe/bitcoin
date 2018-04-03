// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "decisiontradefilterproxymodel.h"
#include "decisiontradetablemodel.h"


bool DecisionTradeFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString address = index.data(DecisionTradeTableModel::AddressRole).toString();
    if (!address.contains(filterAddress, Qt::CaseSensitive))
        return false;

    return true;
}

void DecisionTradeFilterProxyModel::setFilterAddress(const QString &str)
{
    filterAddress = (str.size() >= 2)? str: "";
    invalidateFilter();
}


