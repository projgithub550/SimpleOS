#include "device_driver.h"


void DeviceDriver::DeviceDriver(IOType t,MemoryManager* mgr)
{
    this->type = t;
    this->mmgr = mgr;
}

// execute a pcb(slot func)
void DeviceDriver::handleEvent(IOType _type)
{
    // whether the signal is for me
    if(this->type != _type) return;


    // start executing
    runningPCB = waitingQue.front();
    waitingQue.pop();

    if(runningPCB->getIsPageFault())
    {
        handlePageFault();
        return;
    }

    //step 1: find the file we are about to operate
    int operFile = runningPCB->getActiveFile();
    char buff[BLOCK_SIZE];
    int startAddr = runningPCB->getStartAddr();
    int size = runningPCB->getSize();
    int nbuff = size/BLOCK_SIZE;
    int rmn = size%BLOCK_SIZE;
    int status = 1;

    //step 2: operate this file through fileSystem interface
    if(runningPCB->getOperation() == op_read)//read
    {
        for(int i = 0; i < nbuff; i ++)
        {
            FileManager::readFile(operfile,BLOCK_SIZE,buff);
            status = mmgr->writeMem(pid,startAddr,BLOCK_SIZE,buff);

            if(status == invalid_value)
            {
                finishPCB(err);
                return ;
            }

            startAddr += BLOCK_SIZE;
        }

        FileManager::readFile(operfile,rmn,buff);

        status = mmgr->writeMem(pid,startAddr,rmn,buff);
        if(status == invalid_value)
        {
            finishPCB(err);
            return ;
        }

    }
    else
    {
        for(int i = 0; i < nbuff; i ++)
        {
            FileManager::writeFile(operfile,BLOCK_SIZE,buff);
            status = mmgr->readMem(pid,startAddr,BLOCK_SIZE,buff);

            if(status == invalid_value)
            {
                finishPCB(err);
                return ;
            }

            startAddr += BLOCK_SIZE;
        }

        FileManager::writeFile(operfile,rmn,buff);

        status = mmgr->readMem(pid,startAddr,rmn,buff);
        if(status == invalid_value)
        {
            finishPCB(err);
            return ;
        }


    }

    //finish executing
    finishPCB(ok);
}


void DeviceDriver::handlePageFault()
{
    int addr = runningPCB->getStartAddr();
    int pId = runningPCB->getPId();

    //find the page we need in the disk
    int blockNum = mmgr->p_table.findblocknumber(pId,addr);

    //select a frame (logic page number) for replacement
    int wPage,wBlock;

    //wPage:写到物理页中的页号
    /*wBlock:页替换的时候将替换掉的那页内容写回磁盘，wBlock是要写的磁盘块号，如果为-1，表示
    页表没满，不需要写回*/
    mmgr->LRU(pid,wPage,wBlock);

    //whether the page table is full or not
    if ( wBlock != -1 )
    {
        //write the content of this replaced page back to the block it comes from
        char buff[page_size];
        mmgr->readMemPage(wPage,buff);
        FileManager::writeBlock(wBlock,buff);

        //write the content of the needed block to the frame
        FileManager::readBlock(blockNum,buff);
        mmgr->writeMemPage(wPage,buff);
    }
    else
    {
        //write the frame directly and update the page table accordingly
        char buff[page_size];
        FileManager::readBlock(blockNum,buff);
        mmgr->writeMemPage(wPage,buff);
    }

}

void DeviceDriver::finishPCB(int res)
{
    if(res == ok)
    {
        // send signal to manager
        emit tellManFinIO(runningPCB);
    }
    else
    {
        emit tellError(runningPCB);
    }

    runningPCB = NULL;
}

// called by manager
void DeviceDriver::pushWaitingQue(PCB* pcb)
{
    this->waitingQue.push(pcb);
}
