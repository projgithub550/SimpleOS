#include "IODevice.h"

// a multi-thread function, always alive to run pcb.
void IODevice::run()
{
	while (1) {
		// thread will block here if there's no pcb to run
		executePCB();
	}
}

// execute a pcb
void IODevice::executePCB() {
	// wait for the manager's signal
	wait(sem);

	// start executing
	

	// finish executing
	finishPCB();
}

bool IODevice::wait2run() {
	// unable to fetch a PCB
	if (waitingQue.empty()) return false;

	PCB* pcb = waitingQue.front();
	waitingQue.pop();

	runningPCB = pcb;
}

void IODevice::finishPCB() {
	// send signal to manager
	signal();

	// change runningPCB
	if (!wait2run()) {
		runningPCB = nullptr;
	}
}
