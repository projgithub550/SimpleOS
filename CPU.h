#pragma once
#include "PCB.h"
#include "ProcessManager.h"

class CPU
{
	private:
		int ax,bx,cx,dx;
		int PC;
		short IR;

		PCB* runningPCB;
		ProcessManager* manager;
		
	public:

		// a multi-thread function, always alive 
		void run();

		// this function is called in run(), to execute a pcb
		void executePCB();

		// get the next instruction from PC
		bool fetchInstruction();

		// execute an instruction
		int executeInstruction();

		// finish this pcb
		void finishPCB();
};
