#include "mkdir.h"
#include "ui_mkdir.h"

mkdir::mkdir(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mkdir)
{
    ui->setupUi(this);
}

mkdir::~mkdir()
{
    delete ui;
}

void mkdir::on_pbt_save_clicked()
{
    QString name = ui->lineEdit->text();
    if(!FileManager::mkdir(name.toStdString())) {
        QMessageBox::warning(this, "Warning", "Dir exists!");
        ui->lineEdit->clear();
    } else {
        // success
        this->close();
    }
}

