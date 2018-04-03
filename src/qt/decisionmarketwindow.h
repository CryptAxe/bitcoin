// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONMARKETWINDOW_H
#define HIVEMIND_QT_DECISIONMARKETWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class marketDecision;
class DecisionMarketTableModel;
class DecisionMarketFilterProxyModel;
class DecisionView;
class WalletModel;


class DecisionMarketWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        ADDR_COLUMN_WIDTH = 180,
        B_COLUMN_WIDTH = 100,
        TRADINGFEE_COLUMN_WIDTH = 100,
        TITLE_COLUMN_WIDTH = 260,
        DESCRIPTION_COLUMN_WIDTH = 200,
        TAGS_COLUMN_WIDTH = 100,
        MATURATION_COLUMN_WIDTH = 60,
        DECISIONIDS_COLUMN_WIDTH = 200,
        HASH_COLUMN_WIDTH = 200,
    };

    explicit DecisionMarketWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onDecisionChange(const marketBranch *, const marketDecision *);
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *filterAddress;
    QLineEdit *filterTitle;
    QLineEdit *filterDescription;
    QLineEdit *filterTag;

    DecisionView *view;
    DecisionMarketTableModel *tableModel;
    QTableView *tableView;
    DecisionMarketFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterAddressChanged(const QString &);
    void filterTitleChanged(const QString &);
    void filterDescriptionChanged(const QString &);
    void filterTagChanged(const QString &);
};

#endif // HIVEMIND_QT_DECISIONMARKETWINDOW_H
