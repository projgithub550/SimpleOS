#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>
#include <QMessageBox>
#include <iostream>

#include "file_dir.h"

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    explicit Editor(string name, QWidget *parent = nullptr);
    ~Editor();

private slots:
    void on_savePBT_clicked();

private:
    Ui::Editor *ui;
    string file_name;
    int file_num;
    QString content;
};

#endif // EDITOR_H
