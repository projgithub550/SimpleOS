#ifndef EDITOR_H
#define EDITOR_H

#include <QDialog>

namespace Ui {
class Editor;
}

class Editor : public QDialog
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

private slots:
    void on_savePBT_clicked();

private:
    Ui::Editor *ui;
};

#endif // EDITOR_H
