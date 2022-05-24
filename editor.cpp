#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
}

Editor::Editor(string name, QWidget *parent) :
    QDialog(parent), ui(new Ui::Editor), file_name(name)
{
    ui->setupUi(this);
    content = QString::fromStdString(FileManager::cat(name));
    ui->textEdit->setText(content);
}

Editor::~Editor()
{
    delete ui;
}

// 保存文件内容回磁盘
void Editor::on_savePBT_clicked()
{
    QString new_content = ui->textEdit->toPlainText();
    if(new_content==content) return;

    string str_content = new_content.toStdString();
    int len = str_content.size();
    qDebug()<<"len:"<<len<<"content:"<<new_content;
    char *tmp = (char *)str_content.c_str();

    //这段有问题，直接调用更底层接口
//    file_num = FileManager::openFile(file_name);
//    qDebug()<<"fileNum:"<<file_num;
//    if(!FileManager::writeFile(file_num, len+1, tmp)) {
//        QMessageBox::warning(this, "Warning", "Save failed!");
//    }
//    FileManager::closeFile(file_num);

    os_file* fp = File::Open_File(file_name);
    if (!fp)
        qDebug()<<"fp error";
    if (!disk::os_writefile(tmp, len+1, fp)) {
        QMessageBox::warning(this, "Warning", "Save failed!");
    }
    File::Close_File(fp);
    QMessageBox::warning(this, "Success", "Save success!");
    this->close();
}

