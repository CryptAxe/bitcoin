// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "decisionbranchfilterproxymodel.h"
#include "decisionbranchtablemodel.h"


bool DecisionBranchFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString description = index.data(DecisionBranchTableModel::DescriptionRole).toString();
    if (!description.contains(filterDescription, Qt::CaseInsensitive))
        return false;

    return true;
}

void DecisionBranchFilterProxyModel::setFilterDescription(const QString &str)
{
    filterDescription = (str.size() >= 2)? str: "";
    invalidateFilter();
}

