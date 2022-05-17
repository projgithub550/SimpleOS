#include "procpagedialog.h"
#include "ui_procpagedialog.h"

ProcPageDialog::ProcPageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcPageDialog)
{
    ui->setupUi(this);
}

ProcPageDialog::~ProcPageDialog()
{
    delete ui;
}
