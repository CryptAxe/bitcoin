// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONMARKETFILTERPROXYMODEL_H
#define HIVEMIND_QT_DECISIONMARKETFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class DecisionMarketFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DecisionMarketFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterAddress(const QString &);
    void setFilterTitle(const QString &);
    void setFilterDescription(const QString &);
    void setFilterTag(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterAddress;
    QString filterTitle;
    QString filterDescription;
    QString filterTag;
};


#endif // HIVEMIND_QT_DECISIONMARKETFILTERPROXYMODEL_H
