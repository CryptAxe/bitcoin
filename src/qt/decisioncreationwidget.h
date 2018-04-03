#ifndef DECISIONCREATIONWIDGET_H
#define DECISIONCREATIONWIDGET_H

#include "json/json_spirit_writer_template.h"

#include <QWidget>

class marketBranch;
class marketDecision;

namespace Ui {
class DecisionCreationWidget;
}

class DecisionCreationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DecisionCreationWidget(QWidget *parent = 0);
    ~DecisionCreationWidget();

    json_spirit::Array createDecisionArray();

Q_SIGNALS:
    /** Signal raised when json_spirit::Array for new Decision is created */
    void newDecisionArray(const json_spirit::Array array);

    /** Signal raised for user input errors on decision creation widget */
    void inputError(const QString &error);

    /** Signal raised when user updates a decision array */
    void updatedDecisionArray(const json_spirit::Array array);

public Q_SLOTS:
    // Load a json_spirit array for editing
    void editArray(json_spirit::Array array);

    // Setup the UI for the combo creation widget
    void comboCreationUI();

private Q_SLOTS:
    void on_pushButtonCreateDecision_clicked();

    void on_radioButtonBinary_clicked(bool checked);

    void on_radioButtonScaled_clicked(bool checked);

    void on_inputError(const QString &errorMessage);

    void on_pushButtonUpdateDecision_clicked();

private:
    Ui::DecisionCreationWidget *ui;
    const marketBranch *branch;
    int updateIndex;
};

#endif // DECISIONCREATIONWIDGET_H
