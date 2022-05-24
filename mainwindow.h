#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QScatterSeries>

#include "process_manager.h"
#include "cpu.h"
#include "file_dir.h"
#include "memory.h"

#include "editor.h"
#include "mkdir.h"
#include "mkfile.h"
#include "procpagedialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
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

    /*************画图对应的方法***************/
    void buildAlloChart();                                      //创建图表函数
    void buildMrChart();
    void buildFauChart();

    void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;    //定时器响应函数
    /****************************************/

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_pbt_back_clicked();

    void on_pbt_mkdir_clicked();

    void on_le_path_returnPressed();

    void on_pbt_mkfile_clicked();

    void on_procTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_fileTree_itemPressed(QTreeWidgetItem *item, int column);

    void on_pbt_delete_clicked();

private:
    Ui::MainWindow *ui;

    CPU* cpu;
    ProcessManager* procManager;
    MemoryManager* memManager;
    map<IOType,DeviceDriver*> drivers;
    map<DeviceDriver*, QThread*> IOMap; //记录所有IO设备及其对应线程

    QThread *cpuThread;
    QThread *procManThread;

    QTreeWidgetItem* selected_item;


    /**************allo chart对应的数据**************/
    int memTimerId;                     //定时器ID
    int alloMaxSize;                        //展示的数据数量
    int alloMaxX;                           //X轴长度
    int alloMaxY;                           //Y轴长度
    QList<float> allo_list;         //存储坐标数据，使用list可以很方便的增删
    QChart *alloChart;                      //chart对象，绘图的载体
    QChartView *alloChartView;              //chartView,chart展示的载体
    QSplineSeries *alloSplineSeries;        //绘图数据，连续曲线
    QScatterSeries *alloScatterSeries;      //绘图数据，散点
    /****************************************/

    /**************mem_rate chart对应的数据**************/
    int mrMaxSize;                        //展示的数据数量
    int mrMaxX;                           //X轴长度
    int mrMaxY;                           //Y轴长度
    QList<float> mr_list;         //存储坐标数据，使用list可以很方便的增删
    QChart *mrChart;                      //chart对象，绘图的载体
    QChartView *mrChartView;              //chartView,chart展示的载体
    QSplineSeries *mrSplineSeries;        //绘图数据，连续曲线
    QScatterSeries *mrScatterSeries;      //绘图数据，散点
    /****************************************/

    /**************fault_rate chart对应的数据**************/
    int fauMaxSize;                        //展示的数据数量
    int fauMaxX;                           //X轴长度
    int fauMaxY;                           //Y轴长度
    QList<float> fau_list;         //存储坐标数据，使用list可以很方便的增删
    QChart *fauChart;                      //chart对象，绘图的载体
    QChartView *fauChartView;              //chartView,chart展示的载体
    QSplineSeries *fauSplineSeries;        //绘图数据，连续曲线
    QScatterSeries *fauScatterSeries;      //绘图数据，散点
    /****************************************/
};
#endif // MAINWINDOW_H
