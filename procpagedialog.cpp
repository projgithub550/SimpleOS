#include "procpagedialog.h"
#include "ui_procpagedialog.h"

ProcPageDialog::ProcPageDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcPageDialog)
{
    ui->setupUi(this);
}

ProcPageDialog::ProcPageDialog(int PID, MemoryManager *memManager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcPageDialog)
{
    ui->setupUi(this);
    int vm = memManager->getProcVirMem(PID);
    int pm = memManager->getProcPhyMem(PID);
    QString text = "";
    text.append("Virtual_page #"+QString::number(vm) +
                    "Phisical_page #"+QString::number(pm)+"\n");
    ui->textBrowser->setText(text);
}

ProcPageDialog::~ProcPageDialog()
{
    delete ui;
}
