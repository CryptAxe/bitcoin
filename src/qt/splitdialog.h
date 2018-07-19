#ifndef SPLITDIALOG_H
#define SPLITDIALOG_H

#include <QWidget>

namespace Ui {
class SplitDialog;
}

class SplitDialog : public QWidget
{
    Q_OBJECT

public:
    explicit SplitDialog(QWidget *parent = 0);
    ~SplitDialog();

private:
    Ui::SplitDialog *ui;
};

#endif // SPLITDIALOG_H
