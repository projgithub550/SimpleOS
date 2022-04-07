#include "ProcessManager.h"

// keep listening signals from other threads(cpu, iodevices)
void ProcessManager::listener() {
	// do corresponding work according to signal type
	
	
}

void ProcessManager::createProcess(string workDir, char[] program)
{
	//step 1: create a new PCB and initiate it
	PCB* pcb = new PCB();


	//step 2: create a virtual address space for it and build a page table


	//step 3: put it into ready queue based on its priority
	readyQue.push(pcb);

	//step 2: transform to running status when cpu is free
	ready2run();
}

void ProcessManager::ready2run()
{
	// if there's a pcb running, return
	if (runningPCB) return;

	//step 1: pop out of ready queue
	PCB* pcb = readyQue.top();
	readyQue.pop();

	//step 2: pointer->pcb
	runningPCB = pcb;

	//step 3: recover context of the process
	recoverContext();

	//step 4: send signal to CPU, start executing
	signal();
}

void ProcessManager::run2blocked()
{
	// get current runningPCB
	PCB* pcb = runningPCB;

	// add pcb to the proper queue according to its triggered event
	Event event = pcb->getEvent();

	//find the related IO device
	IODevice* device = deviceMap[event];

	// insert into blockedMap
	blockedMap[pcb] = device;

	// add this pcb to this device's waiting queue
	device->pushWaitingQue(pcb);

	// dispatch the next selected process to run
	ready2run();
}

// served to be called by IODevice
void ProcessManager::blocked2run(PCB* pcb)
{
	// remove this pcb from blockedMap(need mutex)
	mutex
		blockedMap.erase(pcb);

	// push it into ready queue(need mutex)
	mutex
		readyQue.push(pcb);

}

void ProcessManager::run2dead() {
	// delete this PCB
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

		// cpu context
		saveContext();

		// set CPU's runningCPU and recover its context
		recoverContext();

		return true;
	}
	// fail to preempt
	return false;
}

void ProcessManager::saveContext()
{
	// set runningPCB's register's value to that of CPU
	
}

void ProcessManager::recoverContext()
{
	// set CPU's register's value to that of runningPCB

}


