#ifndef MKDIR_H
#define MKDIR_H

#include <QDialog>
#include <QMessageBox>
#include "file_dir.h"

namespace Ui {
class mkdir;
}

class mkdir : public QDialog
{
    Q_OBJECT

public:
    explicit mkdir(QWidget *parent = nullptr);
    ~mkdir();

private slots:
    void on_pbt_save_clicked();

private:
    Ui::mkdir *ui;
};

#endif // MKDIR_H
