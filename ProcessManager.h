#pragma once

#include <QObject>
#include <QVector>

#include <queue>
#include <map>
#include <string>
#include <vector>

#include "PCB.h"
#include "CPU.h"
#include "DeviceDriver.h"

using namespace std;

class ProcessManager : public QObject
{
    Q_OBJECT
    signals:
        void tellCPUExec();

    public slots:
        void run2dead();
        void run2blocked();
        void blocked2ready(PCB* pcb);

	private:
		static bool cmp(PCB* a, PCB* b) { return a->getPriority() < b->getPriority(); }

        CPU* cpu;
        DeviceDriver *stdio;
        DeviceDriver *diskio;

		priority_queue<PCB*, vector<PCB*>, decltype(&cmp)> readyQue;
        PCB* runningPCB;					//运行中的PC
        map<PCB*, int> PCBMap;              //记录所有活着的PCB
		
	public:
		// keep listening signals from other threads(cpu, iodevices)
		void run();

		void createProcess(string workDir, char[] program);
		
        void ready2run();

		// When there is a new PCB into readyQue, try to use the top to preempt the runningPCB
		bool tryPreemp();
		
        //上下文切换，包括pcb和cpu状态的保存
		void saveContext();
        void recoverContext(PCB* pcb);
		
        QVector<PCB*> getAllPCB();

};
