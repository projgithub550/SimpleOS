#include "process_manager.h"
#include <QtGlobal>

void ProcessManager::createProcess(string workDir)
{
    //step 1: create a new PCB and initiate it
    PCB* pcb = new PCB();
    pcb->setPId(this->getAllPCB().size() + 1);
    pcb->setEvent(-1);

    //generate the priority randomly whose value is between 1 and 5
    pcb->setPriority(qrand()%5 + 1);

    pcb->setWorkDir(workDir);
    pcb->setActiveFile(-1);
    pcb->setOperation(-1);

    //step 2: create a virtual address space for it and build a page table
    pcb->setTextStart(0);
    pcb->setTextEnd();
    pcb->setDataStart();
    pcb->setDataEnd();
    pcb->setBase();
    pcb->setTop();
    pcb->setPC(pcb->getTextStart());
    int r[REG_NUM] = {0};
    pcb->setReg(r);



    //step 3: put it into ready queue based on its priority
    this->allPCB.push_back(pcb);
    readyQue.push(pcb);
    pcb->setStatus(st_ready);

    //step 2: transform to running status when cpu is free
    this->ready2run();
}

void ProcessManager::ready2run()
{
    // if there's a pcb running, return or try preemption
    if (runningPCB) return;

    //step 1: pop out from ready queue
    PCB* pcb = readyQue.top();
    readyQue.pop();

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
        {
            stdio->pushWaitingQue(pcb);
            emit tellIOExec(std);
        }
            break;
        case disk_io:
        {
            diskio->pushWaitingQue(pcb);
            emit tellIOExec(disk);
        }
            break;
    //case preempted:

    }

    // dispatch the next selected process to run
    runningPCB = NULL;
    ready2run();
}

void ProcessManager::blocked2ready(PCB* pcb)
{
    // change this PCB's status
    pcb->setStatus(ready);

    // push it into ready queue
    readyQue.push(pcb);

}

//only when a pcb complete all its work（cpu.finishPCB发送信号调用此函数）
void ProcessManager::run2dead()
{
    //delete the page table and release all the pages pointed in each entry


    // delete this PCB
    this->allPCB.erase(runningPCB);
    delete runningPCB;
    runningPCB = NULL;
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
        old->setStatus(st_ready);

        return true;
    }
    // fail to preempt
    return false;
}



