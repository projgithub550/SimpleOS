#include "mainwindow.h"
#include "ui_mainwindow.h"

vector<string> status2string = {"ready","running","blocked","dead"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    memManager = new MemoryManager;
    cpu = new CPU(memManager);
    procManager = new ProcessManager(cpu,memManager);


    // 初始化文件系统
    FileManager::InitFileSys();

    // create threads
    cpuThread = new QThread;
    procManThread = new QThread;

    procManager->moveToThread(procManThread);
    cpu->moveToThread(cpuThread);

    // 创建若干个io device，并存入IO map
    for(IOType t = stdrd; t <= disk; t=(IOType)(t + 1))
    {
        drivers[t] = new DeviceDriver(t,memManager);
        QThread *IOThread = new QThread;
        IOMap[drivers[t]] = IOThread;
        drivers[t]->moveToThread(IOThread);
        IOThread->start();

        connect(procManager, SIGNAL(tellIOExec(IOType)), drivers[t], SLOT(handleEvent(IOType)));
        connect(drivers[t], SIGNAL(tellManFinIO(PCB*)), procManager, SLOT(blocked2ready(PCB*)));

        //IO错误
        connect(drivers[t],SIGNAL(tellError(PCB*)),procManager,SLOT(blocked2dead(PCB*)));
    }
    procManager->setDrivers(this->drivers);


    // begin listen signal
    procManThread->start();
    cpuThread->start();

    // 绑定多线程之间的信号关系
    connect(procManager, SIGNAL(tellCPUExec()), cpu, SLOT(executePCB()));
    connect(cpu, SIGNAL(tellManDead()), procManager, SLOT(run2dead()));
    connect(cpu, SIGNAL(tellManFork(PCB*)), procManager, SLOT(forkProcess(PCB*)));
    connect(cpu, SIGNAL(tellManBlocked()), procManager, SLOT(run2blocked()));

    buildAlloChart();
//    buildMrChart();
    buildFauChart();

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

    vector<PCB*> allPCB = procManager->getAllPCB();
    QList<QTreeWidgetItem*> treeItems;

    for(auto pcb : allPCB) {
        QTreeWidgetItem* tmp = new QTreeWidgetItem(ui->procTree);
        // add pcb'message
        tmp->setText(0, QString::fromStdString(pcb->getWorkDir()));      // EXE文件名
        tmp->setText(1, QString::number(pcb->getPId()));
        tmp->setText(2, QString::number(memManager->getProcPhyMem(pcb->getPId())));      // 占用实际内存
        tmp->setText(3, QString::number(memManager->getProcVirMem(pcb->getPId())));      // 占用虚拟内存
        tmp->setText(4, QString::number(pcb->getPriority()));
        tmp->setText(5, QString::fromStdString(status2string[pcb->getStatus()]));

        // append to item list
        treeItems.append(tmp);
    }

    // add to tree widget
    ui->procTree->addTopLevelItems(treeItems);

//    QString readyStr;
//    QString runningStr;
//    QString blockedStr;
    ui->tb_blocked->clear();
    ui->tb_ready->clear();
    ui->tb_running->clear();
    for(auto pcb : allPCB)
    {
        switch(pcb->getStatus())
        {
            case ready:
                ui->tb_ready->insertPlainText(QString::number(pcb->getPId()) + " ");
                break;
            case running:
                ui->tb_running->insertPlainText(QString::number(pcb->getPId()) + " ");
                break;
            case blocked:
                ui->tb_blocked->insertPlainText(QString::number(pcb->getPId()) + " ");
                break;
            default:
                break;
        }
    }


}

//显示文件系统的tab
void MainWindow::showFileTree() {
//    for (int j = 0; j < DIR_FILE_NUM; j++) {qDebug()<<"j:"<<j<<"inode:"<<(*(FileManager::current_dir + j)).iNode_no;}

    ui->fileTree->clear();

    // 默认处于根目录
    QList<QTreeWidgetItem*> treeItems;
    vector<pair<string, unsigned short>> files = DIR::os_ls();
    for(auto &file : files) {
        qDebug()<<QString::fromStdString(file.first)<<QString::number(file.second);
        QTreeWidgetItem* tmp = new QTreeWidgetItem(ui->fileTree);
        if(file.second==0) {
            // dir
            tmp->setText(0,  QString::fromStdString(file.first));
            tmp->setText(1, "DIR");
        } else if(file.second==1) {
            // txt
            tmp->setText(0,  QString::fromStdString(file.first));
            tmp->setText(1, "TXT");
        } else if(file.second==2) {
            //exe
            tmp->setText(0,  QString::fromStdString(file.first));
            tmp->setText(1, "EXE");
        }
        treeItems.append(tmp);
    }
    ui->fileTree->addTopLevelItems(treeItems);

    // 显示当前path
    ui->le_path->setText(QString::fromStdString(FileManager::pwd()));
}

//显示内存系统的tab
void MainWindow::showMemInfo() {
    // 开启计时器，开始更新内存信息
    memTimerId = startTimer(1000);


}

// 双击进入
void MainWindow::on_fileTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    string file_name = item->text(0).toStdString();
    // 判断是文件还是目录
    if(item->text(1)=="DIR") {
        // 调用cd，进入下一级
        qDebug()<<"cd:"<<QString::fromStdString(file_name);
        FileManager::cd(file_name);
        qDebug()<<"pwd:"<<QString::fromStdString(FileManager::pwd());
        showFileTree();
    } else if(item->text(1)=="TXT") {

        // 弹出窗口显示内容(可以修改)
        Editor editor(file_name);
        editor.exec();

    } else if(item->text(1)=="EXE") {

        // 使用exe文件信息创建进程
        procManager->createProcess(file_name);
    }
}

void MainWindow::on_fileTree_itemPressed(QTreeWidgetItem *item, int column)
{
    selected_item = item;
    qDebug()<<selected_item->text(1);

}

void MainWindow::on_pbt_delete_clicked()
{
    qDebug()<<"delete:"<<selected_item->text(1);

    // 弹出警告框“是否继续”
    int ret = QMessageBox::critical(this, tr("Warning!"), tr("you are tring to delete a file/dir!\nDo you want to continue?"),
                  QMessageBox::Yes | QMessageBox::No,  QMessageBox::No);
    if(ret==QMessageBox::No) return;

    // 判断类型，删除
    if(selected_item->text(1)=="DIR") {
        if(!FileManager::rmdir(selected_item->text(0).toStdString()))
            QMessageBox::warning(this, "Warning", "delete dir failed!");
    } else {
        if(!FileManager::rmfile(selected_item->text(0).toStdString()))
            QMessageBox::warning(this, "Warning", "delete file failed!");
    }
    showFileTree();
}


// 返回上一级
void MainWindow::on_pbt_back_clicked()
{
    FileManager::cd("..");
    showFileTree();
}


void MainWindow::on_pbt_mkdir_clicked()
{
    mkdir mkdir_dialog;
    mkdir_dialog.exec();
    showFileTree();
}

void MainWindow::on_pbt_mkfile_clicked()
{
    mkfile mkfile_dialog;
    mkfile_dialog.exec();
    showFileTree();
}

// 回车跳转
void MainWindow::on_le_path_returnPressed()
{
    // 如果路径相同，不用更改
    if(ui->le_path->text().toStdString()==FileManager::pwd()) return;
    if(!FileManager::cd(ui->le_path->text().toStdString())) {
        // fail
        QMessageBox::warning(this, "Warning", "cd failed!");
        // 显示原来的内容（当前path）
        ui->le_path->setText(QString::fromStdString(FileManager::pwd()));
    } else {
        // 刷新
        showFileTree();
    }
}

// 双击进程展示其内存占用信息
void MainWindow::on_procTree_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    ProcPageDialog dialog(item->text(1).toInt(), memManager);
    dialog.exec();
}

// 初始化memchart
void MainWindow::buildAlloChart()
{
    //初始化数据成员
    alloMaxSize = 60; // 只存储最新的60个数据
    alloMaxX = 80;
    alloMaxY = memManager->getTotalMem();
    qDebug() << "total mem:" << alloMaxY;
    alloSplineSeries = new QSplineSeries();
    QPen pen(0x000001);
    pen.setWidth(5);
    alloSplineSeries->setPen(pen);              //设置画笔颜色和大小

    alloScatterSeries = new QScatterSeries();
    alloScatterSeries->setMarkerSize(5);        //设置散点大小
    alloScatterSeries->setColor(0x0000FF);      //设置散点颜色

    alloChart = new QChart();
    alloChart->addSeries(alloSplineSeries);         //添加数据源
    alloChart->addSeries(alloScatterSeries);        //添加数据源
    alloChart->legend()->hide();                //关闭图例
    alloChart->setTitle("内存分配曲线");
    alloChart->createDefaultAxes();

    //上述方式已经被下面这种方式代替(推荐，不会报警告，但可读性不高)
    alloChart->axes(Qt::Horizontal).back()->setRange(0, alloMaxX);
    alloChart->axes(Qt::Horizontal).back()->setTitleText("时间/秒");
    alloChart->axes(Qt::Vertical).back()->setRange(0, alloMaxY);
    alloChart->axes(Qt::Vertical).back()->setTitleText("内存占用");

    //为chart对象实例化一个Qchartview
    alloChartView = new QChartView(alloChart);
    alloChartView->setRenderHint(QPainter::Antialiasing);         //防止曲线变形，保持曲线圆滑

    ui->gl_mem->addWidget(alloChartView);
}

// 初始化memchart
void MainWindow::buildMrChart()
{
    //初始化数据成员
    mrMaxSize = 60; // 只存储最新的60个数据
    mrMaxX = 80;
    mrMaxY = 1;
    mrSplineSeries = new QSplineSeries();
    QPen pen(0x000001);
    pen.setWidth(5);
    mrSplineSeries->setPen(pen);              //设置画笔颜色和大小

    mrScatterSeries = new QScatterSeries();
    mrScatterSeries->setMarkerSize(5);        //设置散点大小
    mrScatterSeries->setColor(0x0000FF);      //设置散点颜色

    mrChart = new QChart();
    mrChart->addSeries(mrSplineSeries);         //添加数据源
    mrChart->addSeries(mrScatterSeries);        //添加数据源
    mrChart->legend()->hide();                //关闭图例
    mrChart->setTitle("page fault曲线");
    mrChart->createDefaultAxes();

    //上述方式已经被下面这种方式代替(推荐，不会报警告，但可读性不高)
    mrChart->axes(Qt::Horizontal).back()->setRange(0, mrMaxX);
    mrChart->axes(Qt::Horizontal).back()->setTitleText("时间/秒");
    mrChart->axes(Qt::Vertical).back()->setRange(0, mrMaxY);
    mrChart->axes(Qt::Vertical).back()->setTitleText("内存占用率");

    //为chart对象实例化一个Qchartview
    mrChartView = new QChartView(mrChart);
    mrChartView->setRenderHint(QPainter::Antialiasing);         //防止曲线变形，保持曲线圆滑

    ui->gl_mem->addWidget(mrChartView);
}

// 初始化page fault chart
void MainWindow::buildFauChart()
{
    //初始化数据成员
    fauMaxSize = 60; // 只存储最新的60个数据
    fauMaxX = 80;
    fauMaxY = 1;
    fauSplineSeries = new QSplineSeries();
    QPen pen(0x000001);
    pen.setWidth(5);
    fauSplineSeries->setPen(pen);              //设置画笔颜色和大小

    fauScatterSeries = new QScatterSeries();
    fauScatterSeries->setMarkerSize(5);        //设置散点大小
    fauScatterSeries->setColor(0x0000FF);      //设置散点颜色

    fauChart = new QChart();
    fauChart->addSeries(fauSplineSeries);         //添加数据源
    fauChart->addSeries(fauScatterSeries);        //添加数据源
    fauChart->legend()->hide();                //关闭图例
    fauChart->setTitle("缺页率曲线");
    fauChart->createDefaultAxes();

    //上述方式已经被下面这种方式代替(推荐，不会报警告，但可读性不高)
    fauChart->axes(Qt::Horizontal).back()->setRange(0, fauMaxX);
    fauChart->axes(Qt::Horizontal).back()->setTitleText("时间/秒");
    fauChart->axes(Qt::Vertical).back()->setRange(0, fauMaxY);
    fauChart->axes(Qt::Vertical).back()->setTitleText("缺页率");

    //为chart对象实例化一个Qchartview
    fauChartView = new QChartView(fauChart);
    fauChartView->setRenderHint(QPainter::Antialiasing);         //防止曲线变形，保持曲线圆滑

    ui->gl_mem->addWidget(fauChartView);
}

// 定时器事件
void MainWindow::timerEvent(QTimerEvent *event)
{

    if (event->timerId() == memTimerId) {
        this->showProcTree();
    /*********************allocate chart数据更新**************************/
        float alloNew = memManager->getAllocatedMem();
        //qDebug() << "---------------" << alloNew;
        allo_list << alloNew;
        // 数据个数超过了最大数量,删除首个，数据往前移，添加最后一个
        if (allo_list.size() > alloMaxSize) {
            allo_list.pop_front();
        }
        //清空数据
        alloSplineSeries->clear();
        alloScatterSeries->clear();

        //以1为步长
        for (int i = 0; i < allo_list.size(); ++i) {
            alloSplineSeries->append(i, allo_list.at(i));
            alloScatterSeries->append(i, allo_list.at(i));
        }
    /*********************allocate chart数据更新**************************/

    /*********************mem_rate chart数据更新**************************/
        float mrNew = memManager->getMemRate();
//        mr_list << mrNew;
//        // 数据个数超过了最大数量,删除首个，数据往前移，添加最后一个
//        if (mr_list.size() > mrMaxSize) {
//            mr_list.pop_front();
//        }
//        //清空数据
//        mrSplineSeries->clear();
//        mrScatterSeries->clear();

//        //以1为步长
//        for (int i = 0; i < mr_list.size(); ++i) {
//            mrSplineSeries->append(i, mr_list.at(i));
//            mrScatterSeries->append(i, mr_list.at(i));
//        }
    /*********************mem_rate chart数据更新**************************/

    /*********************fault_rate chart数据更新**************************/
        float fauNew = memManager->getPageFaultRate();
        fau_list << fauNew;
        // 数据个数超过了最大数量,删除首个，数据往前移，添加最后一个
        if (fau_list.size() > fauMaxSize) {
            fau_list.pop_front();
        }
        //清空数据
        fauSplineSeries->clear();
        fauScatterSeries->clear();

        //以1为步长
        for (int i = 0; i < fau_list.size(); ++i) {
            fauSplineSeries->append(i, fau_list.at(i));
            fauScatterSeries->append(i, fau_list.at(i));
        }
    /*********************fault_rate chart数据更新**************************/
        ui->le_allo->setText(QString::number(alloNew));
        ui->le_free->setText(QString::number(alloMaxY-alloNew));
        ui->le_mem_rate->setText(QString::number(mrNew));
    }

}
