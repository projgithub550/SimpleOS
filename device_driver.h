#pragma once
#include <QObject>

#include <string>
#include <queue>
#include "constant.h"
#include "pcb.h"
#include "memory.h"
#include "file_dir.h"



using namespace std;



class DeviceDriver:public QObject
{
    Q_OBJECT
signals:
    void tellManFinIO(PCB* pcb);
    void tellError(PCB* pcb);

public slots:
    // this function is called in run(), to execute a pcb
    void handleEvent(IOType t);

private:
    IOType type;
	queue<PCB*> waitingQue;
	PCB* runningPCB;

    MemoryManager *mmgr;


public:
    DeviceDriver(IOType t,MemoryManager* mgr,page_table);

    void pushWaitingQue(PCB* pcb);

    void handlePageFault();

	// finish executing a PCB, signal the manager
    void finishPCB(int res);

    QString getName(){return name;}
};
