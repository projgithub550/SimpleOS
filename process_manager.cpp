#include "process_manager.h"
#include <QtGlobal>

ProcessManager::ProcessManager(CPU* cpu,MemoryManager* mgr)
{
    this->cpu = cpu;
    this->mmgr = mgr;
    this->runningPCB = NULL;
}

void ProcessManager::setDrivers(map<IOType,DeviceDriver*> ds)
{
    this->drivers = ds;
}



void ProcessManager::createProcess(string workDir)
{
    //step 1: create a new PCB and initiate it
    PCB* pcb = new PCB();
    pcb->setPId(this->getAllPCB().size() + 1);
    pcb->setEvent(normal);

    //generate the priority randomly whose value is between 1 and 5
    pcb->setPriority(rand()%5 + 1);

    pcb->setWorkDir(workDir);
    pcb->setActiveFile(-1);

    //step 2: create a virtual address space for it and build a page table
    mmgr->createPageTable(pcb->getPId(),workDir);
    pcb->setTextStart(0);

    //get the program's size
  //   qDebug() << "传入的文件名为:" << QString::fromStdString( pcb->getWorkDir());
    os_file* of =  File::Open_File(workDir);

    int fileSize = of->f_iNode->i_size;
    pcb->setTextEnd(fileSize);
    File::Close_File(of);

    pcb->setBase(page_size*maxa_page_number);
    pcb->setTop(page_size*maxa_page_number);
    pcb->setPC(pcb->getTextStart());

    for(int i = 0; i < REG_NUM; i ++)
    {
        pcb->setReg(i,0);
    }

    //step 3: put it into ready queue based on its priority
    pcb->setStatus(ready);
    this->allPCB.push_back(pcb);
    readyQue.push(pcb);



    //step 2: transform to running status when cpu is free
    qDebug() << "创建了一个进程加入到就绪队列，进程优先级为：" << pcb->getPriority();
    this->ready2run();
}

void ProcessManager::forkProcess(PCB* pcb)
{
    PCB* child = new PCB();
    child->setPId(this->getAllPCB().size() + 11);//fork的进程号多加10
    child->setEvent(normal);

    //generate the priority randomly whose value is between 1 and 5
    child->setPriority(rand()%5 + 1);

    //file info
    child->setWorkDir(pcb->getWorkDir());
    child->setActiveFile(pcb->getActiveFile());

    for(int i = 0; i < (int)pcb->getFileIds().size(); i ++)
    {
        child->pushFileIds((pcb->getFileIds())[i]);
    }

    //address space
   // qDebug() << "创建页表-------------------";
   // qDebug() << "传入的文件名为:" << QString::fromStdString( pcb->getWorkDir());
     mmgr->createPageTable(child->getPId(),pcb->getWorkDir());
     child->setTextStart(0);

     //get the program's size
    // qDebug() << "工作路径为：" <<QString::fromStdString( pcb->getWorkDir());
     child->setTextEnd(pcb->getTextEnd());


     child->setBase(page_size*maxa_page_number);
     child->setTop(page_size*maxa_page_number);

    //register
    child->setPC(pcb->getPC());

    for(int i = 0; i < REG_NUM; i ++)
    {
        child->setReg(i,pcb->getReg(i));
    }
    child->setStatus(ready);

    this->allPCB.push_back(child);
    readyQue.push(child);
    qDebug() << "fork了一个进程加入到就绪队列，进程优先级为：" << child->getPriority();
    ready2run();

    return;
}

void ProcessManager::ready2run()
{
    //if there's no pcb waiting in the ready queue,return;
    if(readyQue.size() == 0) return;

    // if there's a pcb running, try preemption
    if (runningPCB != NULL)
    {
        this->tryPreemp();
        return;
    }


    //step 1: pop out from ready queue

  //  qDebug() << "sssssssssssssssssssssssss";
    PCB* pcb = readyQue.top();
    readyQue.pop();

    qDebug() << "进程" << pcb->getPId() << "进入运行态";
    //step 2: pointer->pcb
    runningPCB = pcb;
    pcb->setStatus(running);

    //step 3: recover context of the process
    this->cpu->setRunningPCB(runningPCB);


    //step 4: send signal to CPU, start executing
    emit tellCPUExec();
}

void ProcessManager::run2blocked()
{
  //  qDebug() << "进程-------------------";

    // get current runningPCB
    PCB* pcb = runningPCB;
    pcb->setStatus(blocked);

    // add pcb to the proper queue according to its triggered event
    switch (pcb->getEvent())
    {
        case std_io:
            qDebug() << "进程" << pcb->getPId()<< "阻塞，加入标准输入输出阻塞队列";
            this->drivers[stdrd]->pushWaitingQue(pcb);
            emit tellIOExec(stdrd);
            break;
        case disk_io:
            qDebug() << "进程" << pcb->getPId()<< "阻塞，加入磁盘阻塞队列";
            this->drivers[disk]->pushWaitingQue(pcb);
            emit tellIOExec(disk);
            break;
        case preempted:
            qDebug() << "进程" << pcb->getPId()<< "被抢占，回到就绪队列";
            pcb->setStatus(ready);
            readyQue.push(pcb);
            break;
        case normal:
            qDebug() << "error";
            this->run2dead();
            break;
    }

    // dispatch the next selected process to run
    runningPCB = NULL;
    ready2run();
}

void ProcessManager::blocked2ready(PCB* pcb)
{
    qDebug() << "进程"<<pcb->getPId()<<"响应正常结束,进入就绪队列";
    // change this PCB's status
    pcb->setStatus(ready);

    // push it into ready queue
    readyQue.push(pcb);

 //   qDebug() << "加入到就绪队列";
    ready2run();
}

//only when a pcb complete all its work（cpu.finishPCB发送信号调用此函数）
void ProcessManager::run2dead()
{
    //delete the page table and release all the pages pointed in each entry
    mmgr->freePage(runningPCB->getPId());

    // delete this PCB
    vector<PCB*>::iterator it = find(allPCB.begin(),allPCB.end(),runningPCB);
    if(it != allPCB.end())
    {
        this->allPCB.erase(it);
    }
    delete runningPCB;
    runningPCB = NULL;

    //dispatch next pcb
    //qDebug() << "dead";
    qDebug() << "进程死亡，调度下一进程";
    ready2run();
}

void ProcessManager::blocked2dead(PCB* pcb)
{
    qDebug() << "进程"<<pcb->getPId()<<"响应异常结束,直接销毁";
    //delete the page table and release all the pages pointed in each entry
    mmgr->freePage(pcb->getPId());

    // delete this PCB
    vector<PCB*>::iterator it = find(allPCB.begin(),allPCB.end(),pcb);
    if(it != allPCB.end())
    {
        this->allPCB.erase(it);
    }
    delete pcb;

}

// When there is a new PCB into readyQue, try to use the top to preempt the runningPCB
bool ProcessManager::tryPreemp() {
    PCB* pcb = readyQue.top();
    if (pcb->getPriority() > runningPCB->getPriority()) {

        //tell cpu to stop running
        qDebug() << "进程" << pcb->getPId()<<"尝试抢占CPU";
        this->cpu->setNState(PREEMPTED);


        return true;
    }
    // fail to preempt
    return false;
}
