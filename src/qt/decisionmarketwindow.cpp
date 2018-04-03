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

#include "decisionmarketfilterproxymodel.h"
#include "decisionmarkettablemodel.h"
#include "decisionmarketwindow.h"
#include "decisionview.h"
#include "walletmodel.h"


DecisionMarketWindow::DecisionMarketWindow(QWidget *parent)
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

    QLabel *filterByTitleLabel = new QLabel(tr("Filter By Title: "));
    filterByTitleLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByTitleLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByTitleLabel, 1, 0);

    filterTitle = new QLineEdit();
    glayout->addWidget(filterTitle, 1, 1);
    connect(filterTitle, SIGNAL(textChanged(QString)), this, SLOT(filterTitleChanged(QString)));

    QLabel *filterByDescriptionLabel = new QLabel(tr("Filter By Description: "));
    filterByDescriptionLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByDescriptionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByDescriptionLabel, 2, 0);

    filterDescription = new QLineEdit();
    glayout->addWidget(filterDescription, 2, 1);
    connect(filterDescription, SIGNAL(textChanged(QString)), this, SLOT(filterDescriptionChanged(QString)));

    QLabel *filterByTagLabel = new QLabel(tr("Filter By Tag: "));
    filterByTagLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    filterByTagLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    glayout->addWidget(filterByTagLabel, 3, 0);

    filterTag = new QLineEdit();
    glayout->addWidget(filterTag, 3, 1);
    connect(filterTag, SIGNAL(textChanged(QString)), this, SLOT(filterTagChanged(QString)));

    tableView = new QTableView();
    tableView->installEventFilter(this);
    tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    tableView->setTabKeyNavigation(false);
    tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    tableView->setAlternatingRowColors(true);
    vlayout->addWidget(tableView);
}

void DecisionMarketWindow::setModel(WalletModel *model)
{
    if (!model)
        return;

    tableModel = model->getDecisionMarketTableModel();
    if (!tableModel)
        return;

    proxyModel = new DecisionMarketFilterProxyModel(this);
    proxyModel->setSourceModel(tableModel);

    // tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView->setModel(proxyModel);
    tableView->setAlternatingRowColors(true);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableView->setSortingEnabled(true);
    tableView->sortByColumn(DecisionMarketTableModel::Title, Qt::AscendingOrder);
    tableView->verticalHeader()->hide();

    tableView->setColumnWidth(DecisionMarketTableModel::Address, ADDR_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::B, B_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::TradingFee, TRADINGFEE_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::Title, TITLE_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::Description, DESCRIPTION_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::Tags, TAGS_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::Maturation, MATURATION_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::DecisionIDs, DECISIONIDS_COLUMN_WIDTH);
    tableView->setColumnWidth(DecisionMarketTableModel::Hash, HASH_COLUMN_WIDTH);

    connect(tableView->selectionModel(),
       SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
       this, SLOT(currentRowChanged(QModelIndex, QModelIndex)));
}

void DecisionMarketWindow::onDecisionChange(const marketBranch *branch, const marketDecision *decision)
{
    if (!tableModel || !proxyModel)
        return;

    tableModel->onDecisionChange(branch, decision);
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
    } else {
       view->onMarketChange(0);
    }
}

bool DecisionMarketWindow::eventFilter(QObject *obj, QEvent *event)
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

void DecisionMarketWindow::currentRowChanged(const QModelIndex &curr, const QModelIndex &prev)
{
    if (!tableModel || !view || !proxyModel || !curr.isValid())
        return;

    int row = proxyModel->mapToSource(curr).row();
    const marketMarket *market = tableModel->index(row);
    view->onMarketChange(market);
}

void DecisionMarketWindow::filterAddressChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterAddress(str);
}

void DecisionMarketWindow::filterTitleChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterTitle(str);
}

void DecisionMarketWindow::filterDescriptionChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterDescription(str);
}

void DecisionMarketWindow::filterTagChanged(const QString &str)
{
    if (proxyModel)
        proxyModel->setFilterTag(str);
}

