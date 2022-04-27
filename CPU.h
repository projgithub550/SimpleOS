#pragma once
#include "pcb.h"
#include "process_manager.h"
class CPU : public QObject
{
    Q_OBJECT
    signals:
        void tellManDead();
        void tellManBlocked();

    public slots:
        // this function is called in run(), to execute a pcb
        void executePCB();

	private:
        int Reg[REG_NUM];
        unsigned short PC;
        unsigned short IR;
        bool Flag; // 标志位

        PCB* runningPCB;
		
	public:
        void setRunningPCB(PCB* pcb){runningPCB = pcb;}
        PCB* getRunningPCB(){return runningPCB;}

                void recoverContext();

                void saveContext();

		// get the next instruction from PC
		bool fetchInstruction();

		// execute an instruction
		int executeInstruction();

		// finish this pcb
                void finishPCB(Event event);
};
