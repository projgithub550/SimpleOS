#ifndef MKFILE_H
#define MKFILE_H

#include <QDialog>
#include <QMessageBox>
#include "file_dir.h"

namespace Ui {
class mkfile;
}

class mkfile : public QDialog
{
    Q_OBJECT

public:
    explicit mkfile(QWidget *parent = nullptr);
    ~mkfile();

private slots:
    void on_pbt_save_clicked();

private:
    Ui::mkfile *ui;
};

#endif // MKFILE_H
