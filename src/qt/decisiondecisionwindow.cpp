// Copyright (c) 2015 The Hivemind Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QScrollBar>
#include <QTableView>
#include <QVBoxLayout>

#include "decisiondecisionfilterproxymodel.h"
#include "decisiondecisiontablemodel.h"
#include "decisiondecisionwindow.h"
#include "decisionview.h"
#include "walletmodel.h"


DecisionDecisionWindow::DecisionDecisionWindow(QWidget *parent)
    : view((DecisionView *)parent),
    tableModel(0),
    tableView(0),
    proxyModel(0)
{
    setWindowTitle(tr("Decisions"));
    setMinimumSize(800,200);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0,0,0,0);
    vlayout->setSpacing(0);

    QGridLayout *glayout = new QGridLayout();
    glayout->setHorizontalSpacing(0);
    glayout->setVerticalSpacing(0);
    vlayout->addLayout(glayout);

    QLabel *filterByAddressLabel = new QLabel(tr("Filter By Address: "));
    filterByAddressLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByAddressLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByAddressLabel, 0, 0);

    filterAddress = new QLineEdit();
    glayout->addWidget(filterAddress, 0, 1);
    connect(filterAddress, SIGNAL(textChanged(QString)), this, SLOT(filterAddressChanged(QString)));

    QLabel *filterByPromptLabel = new QLabel(tr("Filter By Prompt: "));
    filterByPromptLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByPromptLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByPromptLabel, 1, 0);

    filterPrompt = new QLineEdit();
    glayout->addWidget(filterPrompt, 1, 1);
    connect(filterPrompt, SIGNAL(textChanged(QString)), this, SLOT(filterPromptChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void DecisionDecisionWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getDecisionDecisionTableModel();
    if (!tableModel)
        return;

    proxyModel = new DecisionDecisionFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(DecisionDecisionTableModel::Prompt, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(DecisionDecisionTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::Prompt, PROMPT_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::EventOverBy, EVENTOVERBY_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::IsScaled, ISSCALED_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::Minimum, MINIMUM_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::Maximum, MAXIMUM_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::AnswerOptional, ANSWEROPTIONAL_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionDecisionTableModel::Hash, HASH_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void DecisionDecisionWindow::onBranchChange(const marketBranch *branch)
{
    if (!tableModel || !proxyModel)
        return;

    tableModel->onBranchChange(branch);
    if (proxyModel->rowCount()) {
        QModelIndex topLeft = proxyModel->index(0, 0, QModelIndex());
        int columnCount = proxyModel->columnCount();
        if (columnCount > 0) {
            QModelIndex topRight = proxyModel->index(0, columnCount-1, QModelIndex());
            QItemSelection selection(topLeft, topRight);
            tableView->selectionModel()->select(selection, QItemSelectionModel::Select);
        }
        tableView->setFocus();
        currentRowChanged(topLeft, topLeft);
    }
    else {
       view->onDecisionChange(0);
    }
}

bool DecisionDecisionWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == tableView)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            if ((ke->key() == Qt::Key_C)
                && (ke->modifiers().testFlag(Qt::ControlModifier)))
            {
                /* Ctrl-C: copy the selected cells in TableModel */
                QString selected_text;
                QItemSelectionModel *selection = tableView->selectionModel();
                QModelIndexList indexes = selection->selectedIndexes();
                int prev_row = -1;
                for(int i=0; i < indexes.size(); i++) {
                    QModelIndex index = indexes.at(i);
                    if (i) {
                        char c = (index.row() != prev_row)? '\n': '\t';
                        selected_text.append(c);
                    }
                    QVariant data = tableView->model()->data(index);
                    selected_text.append( data.toString() );
                    prev_row = index.row();
                }
                QApplication::clipboard()->setText(selected_text);
                return true;
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

void DecisionDecisionWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !view || !proxyModel || !curr.isValid())
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketDecision *decision = tableModel->index(row);
    view->onDecisionChange(decision);
}

void DecisionDecisionWindow::filterAddressChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterAddress(str);
}

void DecisionDecisionWindow::filterPromptChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterPrompt(str);
}

