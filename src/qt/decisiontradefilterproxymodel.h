// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONTRADEFILTERPROXYMODEL_H
#define HIVEMIND_QT_DECISIONTRADEFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>


class DecisionTradeFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DecisionTradeFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterAddress(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterAddress;
};


#endif // HIVEMIND_QT_DECISIONTRADEFILTERPROXYMODEL_H
