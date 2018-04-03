// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONDECISIONFILTERPROXYMODEL_H
#define HIVEMIND_QT_DECISIONDECISIONFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class DecisionDecisionFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit DecisionDecisionFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

    void setFilterAddress(const QString &);
    void setFilterPrompt(const QString &);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;

private:
    QString filterAddress;
    QString filterPrompt;
};


#endif // HIVEMIND_QT_DECISIONDECISIONFILTERPROXYMODEL_H
