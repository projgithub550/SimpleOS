#include "cpu.h"
#include <QThread>

CPU::CPU(MemoryManager* m)
{
    this->mmgr = m;
}
void CPU::setRunningPCB(PCB* pcb)
{
    this->runningPCB = pcb;
}

void CPU::executePCB()
{
    recoverContext();

    int NextState = NEXT;

    qDebug() << "cpu is running";
    int i = 1;
    while (NextState == NEXT)
    {
        qDebug() << "进程"<<runningPCB->getPId()<<"正在取第"<<i<<"条指令";
        NextState = fetchInstruction();
        if (NextState == NEXT)
        {
            NextState = executeInstruction();
        }
        i ++;
//        qDebug() << "执行完next_state为" << NextState;
    }

    if (NextState == END) // 正常结束
    {
       // qDebug() << "cpu正常结束";
        runningPCB->setEvent(normal);
    }
    else if (NextState == BLOCK_DISK) // 缺页中断发生
    {
        runningPCB->setEvent(disk_io);
        runningPCB->setIsPageFault(true);
    }
    else if (NextState == BLOCK_IO) // 文件读写中断
    {
        runningPCB->setEvent(disk_io);
    }

    this->saveContext();
    finishPCB();
}

void CPU::recoverContext()
{
    for (int i = 0; i < REG_NUM; i++)
    {
        Reg[i]=runningPCB->getReg(i);
    }
    PC = runningPCB->getPC();
}

void CPU::saveContext()
{
    for (int i = 0; i < REG_NUM; i++)
    {
        runningPCB->setReg(i, Reg[i]);
    }
    runningPCB->setPC(PC);
}

int CPU::fetchInstruction() // 每次返回一个16位指令
{

    QThread::sleep(3);
//    qDebug() << PC;
    char temp[6];
    if(mmgr->readMem(runningPCB->getPId(), PC, 6, temp)==invalid_value)
    {
        qDebug() << "invalid";
        runningPCB->setStartAddr(PC);
        return BLOCK_DISK;
    }

    char temp2[6];
    int i;
    for(i = 0; temp[i] != '\n' && temp[i] != '\0'; i ++)
    {
        temp2[i] =  temp[i];
    }
    temp2[i] = '\0';
    this->IR = atoi(temp2);
    qDebug() << "ir = " << IR;
    PC += i + 1;

    return NEXT;
}

int CPU::executeInstruction()
{
    int OpCode = (int)((IR & 0xe000) >> 13);
    int RegA = (int)((IR & 0x1c00) >> 10);
    int RegB = (int)((IR & 0x0380) >> 7);
    int RegC = (int)((IR & 0x0007));
    int ImmS = ((int)((IR & 0x007f)) > 63 ? (int)(IR & 0x007f) - 128 : (int)(IR & 0x007f));
    int ImmU = (int)((IR & 0x03ff));



    switch (OpCode)
    {
    case 0:
        if (RegA == 0) // 终止指令
            return END;
        else
            Reg[RegA] = getReg(RegB) + getReg(RegC);
        break;
    case 1:
        Reg[RegA] = getReg(RegB) + ImmS;
        break;
    case 2:
        Reg[RegA] = ~(getReg(RegB) & getReg(RegC));
        break;
    case 3:
        Reg[RegA] = ImmU << 6;
        break;
    case 4:
        if (mmgr->writeMem(runningPCB->getPId(), getReg(RegB) + ImmS, 2, (void *)&Reg[RegA]) == BLOCK)
        {
            runningPCB->setStartAddr(getReg(RegB) + ImmS);
            PC -= 2;
            return BLOCK_DISK;
        }
        break;
    case 5:
    {   
        if (mmgr->readMem(runningPCB->getPId(), getReg(RegB) + ImmS, 2, (void *)&Reg[RegA]) == BLOCK)
        {
            runningPCB->setStartAddr(getReg(RegB) + ImmS);
            PC -= 2;
            return BLOCK_DISK;
        }
        break;
    }
    case 6:
        if (getReg(RegA) == getReg(RegB))
            PC += ImmS;
        break;
    case 7:
        if ((getReg(RegA) == 0) && (getReg(RegB) == 0))
        {
            runningPCB->setOperation((Operation)getReg(7));
            runningPCB->setActiveFile(getReg(0));
            runningPCB->setSize(getReg(6));
            runningPCB->setStartAddr(getReg(5));
            runningPCB->pushFileIds(getReg(0));
            return BLOCK_IO;
        }
        else
        {
            Reg[RegA] = PC;
            PC = getReg(RegB);
        }
        break;
    }
    return NEXT;
}

void CPU::finishPCB()
{
    switch (runningPCB->getEvent())
    {
    case normal:
 //       qDebug() << "dead";
        emit tellManDead();
        break;
    default:
    //    qDebug() << "blocked";
        emit tellManBlocked();
        break;
    }
    // change runningPCB
    runningPCB = nullptr;
}
