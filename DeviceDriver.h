#pragma once
#include <QObject>

#include <string>
#include <queue>
#include "PCB.h"

using namespace std;

class DeviceDriver:public QObject
{
    Q_OBJECT
signals:
    void tellIOExec();
    void tellManFinIO(PCB* pcb);

public slots:
    // this function is called in run(), to execute a pcb
    void executePCB(QString name);

private:
    QString name;

	queue<PCB*> waitingQue;
	PCB* runningPCB;
    IOtype ioType;

public:
    void pushWaitingQue(PCB* pcb);

	// fetch next PCB from waitingQue and change it to running state
	void wait2run();

	// finish executing a PCB, signal the manager
	void finishPCB();

    QString getName(){return name;}
};
