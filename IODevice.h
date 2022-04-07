#pragma once
#include <string>
#include <queue>
#include "PCB.h"

using namespace std;

class IODevice {
private:
	queue<PCB*> waitingQue;
	PCB* runningPCB;

public:
	void pushWaitingQue(PCB* pcb) {	waitingQue.push(pcb); }

	// fetch next PCB from waitingQue and change it to running state
	bool wait2run();

	// finish executing a PCB, signal the manager
	void finishPCB();

	// a multi-thread function, always alive
	void run();

	// this function is called in run(), to execute a pcb
	void executePCB();
};
