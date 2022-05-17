#include "mkfile.h"
#include "ui_mkfile.h"

mkfile::mkfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mkfile)
{
    ui->setupUi(this);
}

mkfile::~mkfile()
{
    delete ui;
}

void mkfile::on_pbt_save_clicked()
{
    QString name = ui->le_fname->text();
    QString ftype = ui->le_ftype->text();
    QString content = ui->textEdit->toPlainText();
    int ftype_num;
    if(ftype=="TXT") {
        ftype_num = 1;
    } else if (ftype=="EXE") {
        ftype_num = 2;
    } else {
        QMessageBox::warning(this, "Warning", "no such file type!");
        return;
    }

    if(!FileManager::mkfile(name.toStdString(), content.toStdString(), ftype_num)){
        QMessageBox::warning(this, "Warning", "mkfile failed!");
    } else {
        // success
        this->close();
    }
}


