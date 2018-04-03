// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_BALLOTVOTEWINDOW_H
#define HIVEMIND_QT_BALLOTVOTEWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class BallotVoteFilterProxyModel;
class BallotVoteTableModel;
class BallotView;

class WalletModel;

class BallotVoteWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        HEIGHT_COLUMN_WIDTH = 100,
        ADDRESS_COLUMN_WIDTH = 200,
    };

    explicit BallotVoteWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onBranchBallotChange(const marketBranch *, unsigned int);
    bool eventFilter(QObject *, QEvent *);

private:
    BallotView *ballotView;
    BallotVoteTableModel *tableModel;
    QTableView *tableView;
    BallotVoteFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
};

#endif // HIVEMIND_QT_BALLOTVOTEWINDOW_H
