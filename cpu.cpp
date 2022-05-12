#include "cpu.h"

// execute a pcb
void CPU::executePCB()
{
    this->recoverContext();

	//fetch the next instruction and execute it
    bool hasNext = false;
    Event event = normal;

	//step 1: load the next instrcutions into IR
    event = fetchInstruction();

    while (event == normal)
	{
		// step 2: execute the instruction stored in IR
		// check whether this pcb is blocked or not
        if ((event = executeInstruction()) != normal ) break;
		
        event = fetchInstruction();
	}

    //finish executing and signal manager when pcb is completed or blocked
    runningPCB->setEvent(event);
    this->saveContext();
    finishPCB(event);
}


void CPU::recoverContext()
{
    for(int i = 0; i < REG_NUM; i ++)
    {

    }
}

void CPU::saveContext()
{

}


// get the next instruction from PC
Event CPU::fetchInstruction() {

    IR = readMem(pId, PC);

    //page fault
    if(IR == -1)
    {
        runningPCB->setIsPageFault(true);
        return disk_io;
    }

    PC += 2;
    return normal;
}

// execute an instruction, return pcb's state(blocked:-1)
Event CPU::executeInstruction() {

	// is an io request, pcb need to block
	if (io_request) {

		// send signal to manager to call run2blocked
		signal();

		// return -1 to stop this pcb running
		return -1;
	}

	// normally execute
	return 0;

    unsigned short OpCode = (unsigned short)(IR >> 12);
    unsigned short Src = (unsigned short)((IR & 0x0c00) >> 10);
    unsigned short Des = (unsigned short)((IR & 0x0300) >> 8);
    unsigned short Addr = (unsigned short)((IR & 0x00ff));


    文件操作指令： op + fileid + addr + size
                 3     3
    switch (OpCode)
    {
    case 0: // 空操作
        break;
    case 1: // Des+=Src
        Reg[Des] += Reg[Src];
        break;
    case 2: // Des-=Src
        Reg[Des] -= Reg[Src];
        break;
    case 3: // Des&=Src
        Reg[Des] &= Reg[Src];
        break;
    case 4: // Des=~Src
        Reg[Des] = ~Reg[Src];
        break;
    case 5: // Des=Src+1
        Reg[Des] = Reg[Src] + 1;
        break;
    case 6: // Des=Src-1
        Reg[Des] = Reg[Src] - 1;
        break;
    case 7: // Addr=>Des
        Reg[Des] = Addr;
        break;
    case 8: // [Addr]=>Des
        Reg[Des] = readText(pId, Addr);
        break;
    case 9: // Src=>[Addr]
        writeText(pId, Src, Addr);
        break;
    case 10: // [Addr]=>PC
        PC = readText(pId, Addr);
        break;
    case 11: // if(Des!=Src) [Addr]=>PC
        if (Reg[Des] != Reg[Src])
            PC = readText(pId, Addr);
        break;
    case 12: // 建立文件接口
        switch (Src)
        {
        case 0: // 建立与标准输入流对应的输入接口，可以考虑在初始化PCB时自动在fileTable初始化
            // fileTable[0]=cin...
            break;
        case 1: // 从标准输入获取输入文件名，并建立链接，注意Signal
            // Signal()
            // cin >> FileName
            // FileName >> FilePtr
            // fileTable[Addr]=FilePtr // Addr由exe文件自动生成，为文件在CPU内部的标志
            break;
        }
        return -1;
    case 13: // 调用接口，完成输入输出，注意Signal
            IR

                    op
                    fileId
                    addr
                    size


        switch (Src)
        {
        case 0: // 输入
            // fileTable[Addr]=>1b=>Des
            break;
        case 1: // 输出
            // Des=>1b=>fileTable[Addr]
            break;
        }
        return -1;
    case 15: // 结束
        return 0;
    default:
        return 0;
    }
    return 1;

}

// normally complete
void CPU::finishPCB(Event event)
{

    switch(event)
    {
        case normal:
            emit tellManDead();
            break;
        default:
            emit tellManBlocked();
            break;
    }

	// change runningPCB
    runningPCB = nullptr;
}

