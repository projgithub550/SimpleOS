#include "cpu.h"

void CPU::executePCB()
{
	recoverContext();

	int NextState = NEXT;

	while (NextState == NEXT)
	{
		NextState = fetchInstruction();
		if (NextState == NEXT)
		{
			NextState = executeInstruction();
		}
	}

	if (NextState == END) // 正常结束
	{
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
	if(readMem(running->getPId(), PC, 2, (short *)&IR)==0)
	{
		runningPCB->setStartAddr(PC);
		return BLOCK_DISK;
	}
	PC += 2;
	return NEXT;
}

int CPU::executeInstruction()
{
	short OpCode = (short)((IR & 0xe000) >> 13);
	short RegA = (short)((IR & 0x1c00) >> 10);
	short RegB = (short)((IR & 0x0380) >> 7);
	short RegC = (short)((IR & 0x0007));
	short ImmS = (short)((IR & 0x007f));
	short ImmU = (short)((IR & 0x03ff));

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
		if (writeMem(running->getPId(), getReg(RegB) + ImmS, 2, (void *)&Reg[RegA]) == BLOCK)
		{
			runningPCB->setStartAddr(getReg(RegB) + ImmS);
			PC -= 2;
			return BLOCK_DISK;
		}
		break;
	case 5:
	{
		if (readMem(running->getPId(), getReg(RegB) + ImmS, 2, (void *)&Reg[RegA]) == BLOCK)
		{
			runningPCB->setStartAddr(getReg(RegB) + ImmS);
			PC -= 2;
			return BLOCK_DISK;
		}
		break;
	}
	case 6:
		if (getReg(RegA) == getReg(RegB))
			PC += (1 + ImmS);
		break;
	case 7:
		if ((getReg(RegA) == 0) && (getReg(RegB) == 0))
		{
			runningPCB->setOpe(getReg(7));
			runningPCB->setActiveFile(getReg(0));
			runningPCB->setSize(getReg(6));
			runningPCB->setStartAddr(getReg(5));
			runningPCB->pushFileIds(getReg(0));
			return BLOCK_IO;
		}
		else
		{
			Reg[RegA] = PC + 1;
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
		emit tellManDead();
		break;
	default:
		emit tellManBlocked();
		break;
	}
	// change runningPCB
	runningPCB = nullptr;
}