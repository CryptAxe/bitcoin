// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "decisionmarketfilterproxymodel.h"
#include "decisionmarkettablemodel.h"


bool DecisionMarketFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QString address = index.data(DecisionMarketTableModel::AddressRole).toString();
    if (!address.contains(filterAddress, Qt::CaseSensitive))
        return false;

    QString title = index.data(DecisionMarketTableModel::TitleRole).toString();
    if (!title.contains(filterTitle, Qt::CaseInsensitive))
        return false;

    QString description = index.data(DecisionMarketTableModel::DescriptionRole).toString();
    if (!description.contains(filterDescription, Qt::CaseInsensitive))
        return false;

    if (filterTag.size()) {
        QString tagsString = index.data(DecisionMarketTableModel::TagsRole).toString();
        QStringList tags = tagsString.split(',');
        bool hasTag = false;
        for(ssize_t i=0; i < tags.size(); i++) {
            if (QString::compare(filterTag,tags[i],Qt::CaseSensitive) == 0) {
                hasTag = true; 
                break;
            }
        }
        if (!hasTag)
            return false;
    }

    return true;
}

void DecisionMarketFilterProxyModel::setFilterAddress(const QString &str)
{
    filterAddress = (str.size() >= 2)? str: "";
    invalidateFilter();
}

void DecisionMarketFilterProxyModel::setFilterTitle(const QString &str)
{
    filterTitle = (str.size() >= 2)? str: "";
    invalidateFilter();
}

void DecisionMarketFilterProxyModel::setFilterDescription(const QString &str)
{
    filterDescription = (str.size() >= 2)? str: "";
    invalidateFilter();
}

void DecisionMarketFilterProxyModel::setFilterTag(const QString &str)
{
    filterTag = (str.size() >= 2)? str: "";
    invalidateFilter();
}

