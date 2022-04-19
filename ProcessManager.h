<<<<<<< HEAD
#include "PCB.h"
#include "IODevice.h"
=======
ï»¿#pragma once

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
		PCB* runningPCB;					//ÔËĞĞÖĞµÄPCB
		map<PCB*, IODevice*> blockedMap;	//´æ×èÈûPCB¶ÔÓ¦µÄ×èÈûÔ­Òò£¨Éè±¸£©
		map<Event, IODevice*> deviceMap;	//Éè±¸±í£¬¸ù¾İ×èÈûÊÂ¼ş¶ÔÓ¦¾ßÌåµÄIOÉè±¸

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
        PCB* runningPCB;					//è¿è¡Œä¸­çš„PC
        map<PCB*, int> PCBMap;              //è®°å½•æ‰€æœ‰æ´»ç€çš„PCB
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
        //ä¸Šä¸‹æ–‡åˆ‡æ¢ï¼ŒåŒ…æ‹¬pcbå’ŒcpuçŠ¶æ€çš„ä¿å­˜
		void saveContext();
        void recoverContext(PCB* pcb);
		
        QVector<PCB*> getAllPCB();

>>>>>>> b596dffb48aa89f33ce708c040d68e97361c6456
};
