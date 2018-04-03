// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONTRADEWINDOW_H
#define HIVEMIND_QT_DECISIONTRADEWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class marketDecision;
class marketMarket;
class DecisionTradeFilterProxyModel;
class DecisionTradeTableModel;
class DecisionView;
class WalletModel;


class DecisionTradeWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        ADDR_COLUMN_WIDTH = 180,
        BUYSELL_COLUMN_WIDTH = 60,
        DECISIONSTATE_COLUMN_WIDTH = 60,
        NSHARES_COLUMN_WIDTH = 100,
        PRICE_COLUMN_WIDTH = 100,
        NONCE_COLUMN_WIDTH = 100,
        HASH_COLUMN_WIDTH = 200,
    };

    explicit DecisionTradeWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onMarketChange(const marketBranch *, const marketDecision *, const marketMarket *);
    const DecisionTradeTableModel *getTradeModel(void) const { return tableModel; }
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *filterAddress;

    DecisionView *view;
    DecisionTradeTableModel *tableModel;
    QTableView *tableView;
    DecisionTradeFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterAddressChanged(const QString &);
};

#endif // HIVEMIND_QT_DECISIONTRADEWINDOW_H
