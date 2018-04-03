#ifndef DECISIONSELECTIONVIEW_H
#define DECISIONSELECTIONVIEW_H

#include "decisionselectionmodel.h"

#include <QTableView>
#include <QWidget>
#include <QStringList>

namespace Ui {
class DecisionSelectionView;
}

class DecisionSelectionView : public QWidget
{
    Q_OBJECT

public:
    explicit DecisionSelectionView(QWidget *parent = 0);
    ~DecisionSelectionView();

public Q_SLOTS:
    void loadDecisions(QList<marketDecision *> decisions);
    void on_table_doubleClicked(QModelIndex index);

Q_SIGNALS:
    void decisionSelected(QString decisionHex);
    void multipleDecisionsSelected(QStringList hexList);
    void done();

private Q_SLOTS:
    void on_pushButtonDone_clicked();

private:
    Ui::DecisionSelectionView *ui;

    DecisionSelectionModel *decisionSelectionModel;
    QTableView *decisionSelectionTable;
};

#endif // DECISIONSELECTIONVIEW_H
