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
    this->allPCB.push_back(pcb);
    readyQue.push(pcb);

    pcb->setStatus(ready);

    //step 2: transform to running status when cpu is free
    qDebug() << "创建了一个进程加入到就绪队列，进程优先级为：" << pcb->getPriority();
    this->ready2run();
}

void ProcessManager::ready2run()
{
   // qDebug() << "ready to run";
    // if there's a pcb running, return or try preemption
    if (runningPCB != NULL) return;

    //step 1: pop out from ready queue
    if(readyQue.size() == 0) return;

  //  qDebug() << "sssssssssssssssssssssssss";
    PCB* pcb = readyQue.top();
    readyQue.pop();

  //  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaa";
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
    //case preempted:
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
        PCB* old = runningPCB;
        runningPCB = nullptr;

        // change to new runningPCB
        ready2run();

        // old runningPCB back to readyQue
        readyQue.push(old);
        old->setStatus(ready);

        return true;
    }
    // fail to preempt
    return false;
}
