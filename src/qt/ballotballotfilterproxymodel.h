// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_BALLOTBALLOTFILTERPROXYMODEL_H
#define HIVEMIND_QT_BALLOTBALLOTFILTERPROXYMODEL_H

#include <QModelIndex>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QString>

class BallotBallotFilterProxyModel
    : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit BallotBallotFilterProxyModel(QObject *parent=0)
       : QSortFilterProxyModel(parent) { }

protected:

private:
};

#endif // HIVEMIND_QT_BALLOTBALLOTFILTERPROXYMODEL_H
