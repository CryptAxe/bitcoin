// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVEMIND_QT_DECISIONDECISIONWINDOW_H
#define HIVEMIND_QT_DECISIONDECISIONWINDOW_H

#include <QDialog>
#include <QModelIndex>

QT_BEGIN_NAMESPACE
class QEvent;
class QLineEdit;
class QTableView;
QT_END_NAMESPACE

class marketBranch;
class DecisionDecisionFilterProxyModel;
class DecisionDecisionTableModel;
class DecisionView;
class WalletModel;


class DecisionDecisionWindow
    : public QDialog
{
    Q_OBJECT

public:
    enum ColumnWidths {
        ADDR_COLUMN_WIDTH = 180,
        PROMPT_COLUMN_WIDTH = 320,
        EVENTOVERBY_COLUMN_WIDTH = 80,
        ISSCALED_COLUMN_WIDTH = 80,
        MINIMUM_COLUMN_WIDTH = 80,
        MAXIMUM_COLUMN_WIDTH = 80,
        ANSWEROPTIONAL_COLUMN_WIDTH = 80,
        HASH_COLUMN_WIDTH = 200,
    };

    explicit DecisionDecisionWindow(QWidget *parent=0);
    void setModel(WalletModel *);
    void onBranchChange(const marketBranch *);
    bool eventFilter(QObject *, QEvent *);

private:
    QLineEdit *filterAddress;
    QLineEdit *filterPrompt;

    DecisionView *view;
    DecisionDecisionTableModel *tableModel;
    QTableView *tableView;
    DecisionDecisionFilterProxyModel *proxyModel;

public Q_SLOTS:
    void currentRowChanged(const QModelIndex &, const QModelIndex &);
    void filterAddressChanged(const QString &);
    void filterPromptChanged(const QString &);
};

#endif // HIVEMIND_QT_DECISIONDECISIONWINDOW_H
