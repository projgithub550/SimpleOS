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
    QString name = ui->lineEdit->text();
    QString content = ui->textEdit->toPlainText();
    if(!FileManager::mkfile(name.toStdString(), content.toStdString())){
        QMessageBox::warning(this, "Warning", "mkfile failed!");
    } else {
        // success
        this->close();
    }
}

