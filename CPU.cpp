#include "CPU.h"

// a multi-thread function, always alive to run pcb.
void CPU::run()
{
	while (1) {
		// thread will block here if there's no pcb to run
		executePCB();
	}
}

// execute a pcb
void CPU::executePCB()
{
	// block here, waiting for manager's signal to start run a pcb
	wait(sem);

	//fetch the next instruction and execute it
	bool hasNext = false;

	//step 1: load the next instrcutions into IR
	hasNext = fetchInstruction();

	while (hasNext)
	{
		// step 2: execute the instruction stored in IR
		// check whether this pcb is blocked or not
		if (executeInstruction() == -1) {
			return;
		}
		
		hasNext = fetchInstruction();
	}

	//finish executing and signal manager when pcb complete(not blocked)
	finishPCB();
}

// get the next instruction from PC
bool CPU::fetchInstruction() {

}

// execute an instruction, return pcb's state(blocked:-1)
int CPU::executeInstruction() {
	// is an io request, pcb need to block
	if (io_request) {

		// send signal to manager to call run2blocked
		signal();

		// return -1 to stop this pcb running
		return -1;
	}

	// normally execute
	return 0;
}

// normally complete
void CPU::finishPCB() {
	// send signal to manager to call run2dead
	signal();

	// change runningPCB

}