#pragma once
#include "pcb.h"

const int BLOCK_IO = -2;   // 读写文件中断
const int BLOCK_DISK = -1; // 缺页中断
const int NEXT = 1;		   // 指令正常执行
const int END = 0;		   // 指令结束执行
const int BLOCK = -1;	   // 读写指令失败后返回值，意味着缺页中断的发生

class CPU
{
private:
    // 在执行读写文件操作前准备如下：[7]存储读写意图，[0]存储文件句柄，[6]存储读取大小，[5]存储开始地址
    int Reg[8]; // [0]=0
    int PC;
    int IR;

    PCB *runningPCB;
    ProcessManager *manager;

    int getReg(int i)
    {
        if (i == 0)
            return 0;
        return Reg[i];
    }

public:
    void CPU::recoverContext(); // 恢复现场
    void CPU::saveContext(); // 保存现场

    void executePCB();		 // 执行PCB
    void finishPCB();		 // 结束PCB

    int fetchInstruction(); // 获取指令
    int executeInstruction(); // 执行指令
};
