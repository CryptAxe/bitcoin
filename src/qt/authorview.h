#ifndef AUTHORVIEW_H
#define AUTHORVIEW_H

#include "json/json_spirit_writer_template.h"

#include <QList>
#include <QTableView>
#include <QWidget>

class AuthorPendingTableModel;
class ComboCreationWidget;
class DecisionCreationWidget;
class DecisionMarketCreationWidget;

namespace Ui {
class AuthorView;
}

class AuthorView : public QWidget
{
    Q_OBJECT

public:
    explicit AuthorView(QWidget *parent = 0);
    ~AuthorView();

private Q_SLOTS:
    void on_pushButtonCreateCombo_clicked();
    void on_pushButtonCreateDecision_clicked();
    void on_pushButtonCreateMarket_clicked();
    void on_pushButtonFinalize_clicked();
    void on_finalizeError(const QString &errorMessage);
    void calculateFees();

Q_SIGNALS:
    void newPendingCombo(const json_spirit::Array array);
    void newPendingDecision(const json_spirit::Array array);
    void newPendingDecisionMarket(const json_spirit::Array array);

private:
    Ui::AuthorView *ui;

    ComboCreationWidget *comboCreationWidget;
    DecisionCreationWidget *decisionCreationWidget;
    DecisionMarketCreationWidget *decisionMarketCreationWidget;

    AuthorPendingTableModel *pendingTableModel;
    QTableView *pendingTableView;
};

#endif // AUTHORVIEW_H
