#ifndef PROCPAGEDIALOG_H
#define PROCPAGEDIALOG_H

#include <QDialog>

namespace Ui {
class ProcPageDialog;
}

class ProcPageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcPageDialog(QWidget *parent = nullptr);
    ~ProcPageDialog();

private:
    Ui::ProcPageDialog *ui;
};

#endif // PROCPAGEDIALOG_H
