#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTreeWidgetItem>
#include <QTreeWidget>
# include <QListWidget>
# include <QListWidgetItem>
# include "ProcessManager.h"
# include "CPU.h"
# include "editor.h"

#include "file_dir.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showProcTree();
    void showFileTree();
    void showMemInfo();

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_fileTree_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;

    CPU* cpu;
    ProcessManager* procManager;
    map<DeviceDriver*, QThread*> IOMap; //记录所有IO设备及其对应线程

    QThread *cpuThread;
    QThread *procManThread;

};
#endif // MAINWINDOW_H
