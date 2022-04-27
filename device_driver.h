#pragma once
#include <QObject>

#include <string>
#include <queue>
#include "pcb.h"

using namespace std;

enum IOType
{
    std,
    disk
};

class DeviceDriver:public QObject
{
    Q_OBJECT
signals:
    void tellManFinIO(PCB* pcb);

public slots:
    // this function is called in run(), to execute a pcb
    void handleEvent(QString name);

private:
    IOType type;
	queue<PCB*> waitingQue;
	PCB* runningPCB;

public:
    void pushWaitingQue(PCB* pcb);

	// finish executing a PCB, signal the manager
	void finishPCB();

    QString getName(){return name;}
};
