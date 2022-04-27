#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    procManager = new ProcessManager;
    cpu = new CPU;

    // create threads
    cpuThread = new QThread;
    procManThread = new QThread;

    procManager->moveToThread(procManThread);
    cpu->moveToThread(cpuThread);

    // 根据配置文件创建若干个io device，并存入IO map
    for() {
        DeviceDriver* driver = new DeviceDriver;
        QThread *IOThread = new QThread;
        IOMap[driver] = IOThread;
        driver->moveToThread(IOThread);
        IOThread->start();

        connect(procManager, SIGNAL(tellIOExec(IOType)), device, SLOT(handleEvent(IOType)));
        connect(device, SIGNAL(tellManFinIO(PCB*)), procManager, SLOT(blocked2ready(PCB*)));
    }
    // begin listen signal
    procManThread->start();
    cpuThread->start();

    // 绑定多线程之间的信号关系
    connect(procManager, SIGNAL(tellCPUExec()), cpu, SLOT(executePCB()));
    connect(cpu, SIGNAL(tellManDead()), procManager, SLOT(run2dead()));
    connect(cpu, SIGNAL(tellManBlocked()), procManager, SLOT(run2blocked()));


    // 初始化proc列表信息
    showProcTree();
    showFileTree();
}

MainWindow::~MainWindow()
{
    delete cpu;
    delete procManager;

    delete ui;
}


void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    // 点击tab刷新
    // 进程tab
    if(index==0) showProcTree();
    //文件tab
    else if(index==1) showFileTree();
    //内存file
    else if(index==2) showMemInfo();
}

// 在procTree下展示所有进程信息
void MainWindow::showProcTree() {
    ui->procTree->clear();

    QVector<PCB*> allPCB = procManager->getAllPCB();
    QList<QTreeWidgetItem*> treeItems;

    for(auto pcb : allPCB) {
        QTreeWidgetItem* tmp = new QTreeWidgetItem(ui->procTree);
        // add pcb'message
        tmp->setData();

        // set treeItem's color
        tmp->setBackgroundColor(1,QColor("#FFF4C4"));
        tmp->setBackgroundColor(4,QColor("#FFF4C4"));

//        // add all its childPCB to this item's child树形添加子进程
//        for(遍历该pcb的子进程) {
//            QTreeWidgetItem* child = new QTreeWidgetItem(tmp);

//            // 设置子进程信息
//            chile->setData();

//            tmp->addChild(child);
//        }

        // append to item list
        treeItems.append(tmp);
    }
    // add to tree widget
    ui->procTree->addTopLevelItems(treeItems);
}

//显示文件系统的tab
void MainWindow::showFileTree() {
    ui->fileTree->clear();

    // 默认处于根目录
    QList<QTreeWidgetItem*> treeItems;
    vector<string> files = dir->os_ls();
    for(auto file : files) {
        QTreeWidgetItem* tmp = new QTreeWidgetItem(ui->fileTree);
        // add file'data
        tmp->setData();

        treeItems.append(tmp);
    }
    ui->fileTree->addTopLevelItems(treeItems);
}

//显示内存系统的tab
void MainWindow::showMemInfo() {

}

// 双击删除（弹出警示框是否删除）
void MainWindow::on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{

}

// 单击进入
void MainWindow::on_fileTree_itemClicked(QTreeWidgetItem *item, int column)
{
    // 判断是文件还是目录
    if(目录) {
        // 调用cd，进入下一级
    } else if(txt) {
        // 调用open获取内容string
        string content = openFile();

        // 弹出窗口显示内容(可以修改)
        Editor editor;

        editor.show();

    } else if(exe) {

        // 使用exe文件信息创建进程
        procManager->createProcess(...);
    }
}

