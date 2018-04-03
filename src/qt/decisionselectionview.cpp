#include "decisionselectionview.h"
#include "forms/ui_decisionselectionview.h"

#include <QItemSelectionModel>
#include <QModelIndexList>

DecisionSelectionView::DecisionSelectionView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DecisionSelectionView)
{
    ui->setupUi(this);

    // Setup model and decision selection table
    decisionSelectionTable = new QTableView(this);
    decisionSelectionTable->horizontalHeader()->setStretchLastSection(true);

#if QT_VERSION < 0x050000
    decisionSelectionTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
#else
    decisionSelectionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
#endif

    decisionSelectionModel = new DecisionSelectionModel(this);
    decisionSelectionTable->setModel(decisionSelectionModel);
    decisionSelectionTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui->frame->layout()->addWidget(decisionSelectionTable);

    // Setup signals
    connect(decisionSelectionTable, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(on_table_doubleClicked(QModelIndex)));
}

DecisionSelectionView::~DecisionSelectionView()
{
    delete ui;
}

void DecisionSelectionView::loadDecisions(QList<marketDecision *> decisions)
{
    decisionSelectionModel->loadDecisions(decisions);
}

void DecisionSelectionView::on_table_doubleClicked(QModelIndex index)
{
    QString hex = decisionSelectionTable->model()->data(decisionSelectionTable->model()->index(index.row(), 1)).toString();
    Q_EMIT decisionSelected(hex);
    Q_EMIT done();
}

void DecisionSelectionView::on_pushButtonDone_clicked()
{
    QItemSelectionModel *selection = decisionSelectionTable->selectionModel();
    QStringList hexList;

    if (selection->hasSelection()) {
        QModelIndexList list = selection->selectedRows();

        for (int i = 0; i < list.size(); i++) {
            QString hex = decisionSelectionTable->model()->data(decisionSelectionTable->model()->index(i, 1)).toString();
            hexList.push_back(hex);
        }
    }
    Q_EMIT multipleDecisionsSelected(hexList);
    Q_EMIT done();
}
