// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_BALLOTSEALEDVOTEFILTERPROXYMODEL_H
#define HIVEMIND_QT_BALLOTSEALEDVOTEFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>



class BallotSealedVoteFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BallotSealedVoteFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

protected:

private:
};


#endif // HIVEMIND_QT_BALLOTSEALEDVOTEFILTERPROXYMODEL_H
