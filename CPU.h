#ifndef CPU_H
#define CPU_H

#include <QObject>

#include "pcb.h"
#include "memory.h"
#include "constant.h"
#include <cstdlib>

class CPU : public QObject
{
    Q_OBJECT
signals:
    void tellManDead();
    void tellManFork(PCB* pcb);
    void tellManBlocked();

public slots:
    void executePCB();		 // 执行PCB

private:
    // 在执行读写文件操作前准备如下：[7]存储读写意图，[0]存储文件句柄，[6]存储读取大小，[5]存储开始地址
    int Reg[REG_NUM]; // [0]=0
    int PC;
    int IR;
    int nstate;

    PCB *runningPCB;
    MemoryManager* mmgr;


    int getReg(int i)
    {
        if (i == 0)
            return 0;
        return Reg[i];
    }



public:
    CPU(MemoryManager* m);
    void setRunningPCB(PCB* pcb);
    void setNState(int state)
    {
        this->nstate = state;
    }

    void recoverContext(); // 恢复现场
    void saveContext(); // 保存现场

    void finishPCB();		 // 结束PCB

    int fetchInstruction(); // 获取指令
    int executeInstruction(); // 执行指令

    int moniterPreemption();//监视抢占
};
#endif
