#include "device_driver.h"

// execute a pcb(slot func)
void DeviceDriver::handleEvent(IOType _type)
{
    // whether the signal is for me
    if(this->type != _type) return;


	// start executing
    runningPCB = waitingQue.front();

    if(runningPCB->isPageFault)
    {
        handlePageFault();
        finishPCB();
        return;
    }

    //step 1: find the file we are about to operate
    int operFile = runningPCB->getActiveFile();
    char buff[];

    //step 2: operate this file through fileSystem interface
    if(runningPCB->getOperation() == op_read)//read
    {
        readFile(openfile,size,buff);

        status = writeMem(pid,startAddr,size,buff);

    } else {
        status = readMem(pid,startAddr,size,buff);

        writeFile(openFile,size,buff);
    }

    //finish executing
	finishPCB();
}


void DeviceDriver::handlePageFault()
{
    int addr = runningPCB->getStartAddr();
    int pId = runningPCB->getPId();

    //find the page we need in the disk
    int blockNum = findPage(pId,addr);

    //select a frame (logic page number) for replacement
    int wPage,wBlock;
    LRU(pid,wPage,wBlock);

    //whether the page table is full or not
    if ( wBlock != -1 )
    {
        //write the content of this replaced page back to the block it comes from
        char buff[page_size];
        readMemPage(wPage,buff);
        writeBlock(wBlock,buff);

        //write the content of the needed block to the frame
        readBlock(blockNum,buff);
        writeMemPage(wPage,buff);
    }
    else
    {
        //write the frame directly and update the page table accordingly
        char buff[page_size];
        readBlock(blockNum,buff);
        writeMemPage(wPage,buff);
    }

}

void DeviceDriver::finishPCB() {
	// send signal to manager
    emit tellManFinIO(runningPCB);
	
    runningPCB = NULL;
}

// called by manager
void DeviceDriver::pushWaitingQue(PCB* pcb)
{
    this->waitingQue.push(pcb);
}
