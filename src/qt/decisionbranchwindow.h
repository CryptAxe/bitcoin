// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONBRANCHWINDOW_H
#define HIVEMIND_QT_DECISIONBRANCHWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class DecisionBranchFilterProxyModel;
class DecisionBranchTableModel;
class DecisionView;
class WalletModel;


class DecisionBranchWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        NAME_COLUMN_WIDTH = 100,
        DESCRIPTION_COLUMN_WIDTH = 150,
        BASELISTINGFEE_COLUMN_WIDTH = 80,
        FREEDECISIONS_COLUMN_WIDTH = 60,
        TARGETDECISIONS_COLUMN_WIDTH = 60,
        MAXDECISIONS_COLUMN_WIDTH = 60,
        MINTRADINGFEE_COLUMN_WIDTH = 80,
        TAU_COLUMN_WIDTH = 40,
        BALLOTTIME_COLUMN_WIDTH = 80,
        UNSEALTIME_COLUMN_WIDTH = 80,
        CONSENSUSTHRESHOLD_COLUMN_WIDTH = 100,
        ALPHA_COLUMN_WIDTH = 100,
        TOL_COLUMN_WIDTH = 100,
        HASH_COLUMN_WIDTH = 200,
    };

    explicit DecisionBranchWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void setTableViewFocus(void);
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *filterDescription;

    DecisionView *view;
    DecisionBranchTableModel *tableModel;
    QTableView *tableView;
    DecisionBranchFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterDescriptionChanged(const QString &);
};

#endif // HIVEMIND_QT_DECISIONBRANCHWINDOW_H
