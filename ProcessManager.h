<<<<<<< HEAD
#include "PCB.h"
#include "IODevice.h"
=======
﻿#pragma once

#include <QObject>
#include <QVector>

>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
#include <queue>
#include <map>
#include <string>
#include <vector>

<<<<<<< HEAD
using namespace std;

class ProcessManager
{
	private:
		static bool cmp(PCB* a, PCB* b) { return a->getPriority() < b->getPriority(); }

		priority_queue<PCB*, vector<PCB*>, decltype(&cmp)> readyQue;
		PCB* runningPCB;					//�����е�PCB
		map<PCB*, IODevice*> blockedMap;	//������PCB��Ӧ������ԭ���豸��
		map<Event, IODevice*> deviceMap;	//�豸�����������¼���Ӧ�����IO�豸

=======
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
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
		
	public:
		// keep listening signals from other threads(cpu, iodevices)
		void run();

		void createProcess(string workDir, char[] program);
		
<<<<<<< HEAD
		void ready2run();
		
		void run2blocked();
		
		// served to be called by IODevice
		void blocked2run(PCB* pcb);

		void run2dead();
=======
        void ready2run();
>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456

		// When there is a new PCB into readyQue, try to use the top to preempt the runningPCB
		bool tryPreemp();
		
<<<<<<< HEAD
		void saveContext();
		
		void recoverContext();
		
				
=======
        //上下文切换，包括pcb和cpu状态的保存
		void saveContext();
        void recoverContext(PCB* pcb);
		
        QVector<PCB*> getAllPCB();

>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
};
