#include "editor.h"
#include "ui_editor.h"

Editor::Editor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
}

Editor::Editor(string name, QWidget *parent) :
    QDialog(parent), ui(new Ui::Editor)
{
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

    int len = content.size();
    void *tmp = &content;

    file_num = FileManager::openFile(file_name.toStdString());
    if(!FileManager::writeFile(file_num, len, tmp)) {
        QMessageBox::warning(this, "Warning", "Save failed!");
    }
    FileManager::closeFile(file_num);
}

