// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_BALLOTOUTCOMEWINDOW_H
#define HIVEMIND_QT_BALLOTOUTCOMEWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class BallotOutcomeFilterProxyModel;
class BallotOutcomeTableModel;
class BallotView;
class WalletModel;


class BallotOutcomeWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        HEIGHT_COLUMN_WIDTH = 100,
        HASH_COLUMN_WIDTH = 200,
    };

    explicit BallotOutcomeWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onBranchChange(const marketBranch *);
    bool eventFilter(QObject *, QEvent *);

private:
    BallotView *ballotView;
    BallotOutcomeTableModel *tableModel;
    QTableView *tableView;
    BallotOutcomeFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
};

#endif // HIVEMIND_QT_BALLOTOUTCOMEWINDOW_H
