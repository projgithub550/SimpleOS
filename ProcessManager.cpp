#include "ProcessManager.h"

// keep listening signals from other threads(cpu, iodevices)
void ProcessManager::run() {
	// do corresponding work according to signal type

}

void ProcessManager::createProcess(string workDir, char[] program)
{
	//step 1: create a new PCB and initiate it
	PCB* pcb = new PCB();


	//step 2: create a virtual address space for it and build a page table

    // put it into PCBMap
    PCBMap[pcb] = 1;

    //step 3: put it into ready queue based on its priority
    readyQue.push(pcb);
    pcb->setStatus(st_ready);

	//step 2: transform to running status when cpu is free
	ready2run();
}

void ProcessManager::ready2run()
{
	// if there's a pcb running, return
	if (runningPCB) return;

    //step 1: pop out from ready queue
	PCB* pcb = readyQue.top();
	readyQue.pop();

	//step 2: pointer->pcb
	runningPCB = pcb;
    pcb->setStatus(st_running);

	//step 3: recover context of the process
	recoverContext();

	//step 4: send signal to CPU, start executing
    emit tellCPUExec();
}

void ProcessManager::run2blocked()
{
	// get current runningPCB
	PCB* pcb = runningPCB;
    pcb->setStatus(st_blocked);

	// add pcb to the proper queue according to its triggered event
    IOtype ioType = pcb->getIOtype();

    if(ioType==type_stdio) {
        stdio->pushWaitingQue(pcb);
    } else {
        diskio->pushWaitingQue(pcb);
    }
	// dispatch the next selected process to run
	ready2run();
}

void ProcessManager::blocked2ready(PCB* pcb)
{
    // change this PCB's status
    pcb->setStatus(st_ready);

    // push it into ready queue
    readyQue.push(pcb);

}

//only when a pcb complete all its work（cpu.finishPCB发送信号调用此函数）
void ProcessManager::run2dead() {
    // 释放内存


	// delete this PCB
    PCBMap.erase(runningPCB);
	delete runningPCB;
	runningPCB = nullptr;
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

void ProcessManager::saveContext()
{
	// set runningPCB's register's value to that of CPU

}

void ProcessManager::recoverContext(PCB* pcb)
{
	// set CPU's register's value to that of runningPCB
    cpu->setRunningPCB(pcb);
}

QVector<PCB*> ProcessManager::getAllPCB() {
    QVector<PCB*> res;
    for(auto item : PCBMap) res.push_back(item.first);
    return res;
}
