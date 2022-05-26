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
     //   void tellCPUPreempt();
        void tellIOExec(IOType _type);


    public slots:
     //   void terminate(int pid);
        void run2dead();
        void run2blocked();
        void blocked2ready(PCB* pcb);
        void blocked2dead(PCB* pcb);
        void forkProcess(PCB* pcb);

    private:
        struct cmp
        {
            bool operator()(PCB* pcb1,PCB* pcb2)
            {
                return pcb1->getPriority() < pcb2->getPriority();
            }
        };

        CPU* cpu;
        map<IOType,DeviceDriver*> drivers;
        MemoryManager* mmgr;

        priority_queue<PCB*, vector<PCB*>, cmp> readyQue;
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
