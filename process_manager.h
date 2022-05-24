#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <QObject>

#include <queue>
#include <map>
#include <string>
#include <vector>

#include "pcb.h"
#include "cpu.h"
#include "device_driver.h"
#include "memory.h"
#include "file_dir.h"
#include "constant.h"

using namespace std;

class ProcessManager : public QObject
{
    Q_OBJECT
    signals:
        void tellCPUExec();
        void tellIOExec(IOType _type);

    public slots:
        void run2dead();
        void run2blocked();
        void blocked2ready(PCB* pcb);
        void blocked2dead(PCB* pcb);

    private:
        static bool cmp(PCB* a, PCB* b) { return a->getPriority() < b->getPriority(); }

        CPU* cpu;
        map<IOType,DeviceDriver*> drivers;
        MemoryManager* mmgr;

        priority_queue<PCB*, vector<PCB*>, decltype(&cmp)> readyQue;
        PCB* runningPCB;					//运行中的PC
        vector<PCB*> allPCB;              //记录所有活着的PCB

    public:
        ProcessManager(CPU* cpu,MemoryManager* mgr);

        void setDrivers(map<IOType,DeviceDriver*> drs);
        void createProcess(string workDir);

        void ready2run();

        // When there is a new PCB into readyQue, try to use the top to preempt the runningPCB
        bool tryPreemp();

        vector<PCB*> getAllPCB()
        {
            return this->allPCB;
        }
};

#endif
