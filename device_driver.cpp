#include "device_driver.h"
#include <QThread>

DeviceDriver::DeviceDriver(IOType t,MemoryManager* mgr)
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

    qDebug() << "正在处理进程：" << runningPCB->getPId()<<"的事件";
    QThread::sleep(3);
    if(runningPCB->getIsPageFault())
    {
        handlePageFault();
        //finish executing
        finishPCB(OK);
        return;
    }

    //step 1: find the file we are about to operate
    int pid = runningPCB->getPId();
    int operfile = runningPCB->getActiveFile();
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
                finishPCB(ERR);
                return ;
            }

            startAddr += BLOCK_SIZE;
        }

        FileManager::readFile(operfile,rmn,buff);

        status = mmgr->writeMem(pid,startAddr,rmn,buff);
        if(status == invalid_value)
        {
            finishPCB(ERR);
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
                finishPCB(ERR);
                return ;
            }

            startAddr += BLOCK_SIZE;
        }

        FileManager::writeFile(operfile,rmn,buff);

        status = mmgr->readMem(pid,startAddr,rmn,buff);
        if(status == invalid_value)
        {
            finishPCB(ERR);
            return ;
        }


    }

    //finish executing
    finishPCB(OK);
}


void DeviceDriver::handlePageFault()
{

    //qDebug() << "page_fault";
    int addr = runningPCB->getStartAddr();
    int pId = runningPCB->getPId();

    //qDebug() << "addr" << addr;
    //find the page we need in the disk
    int blockNum = (mmgr->page_tables)[pId].findBlockNumber(addr);

    //select a frame (logic page number) for replacement
    int wPage,wBlock;

    //wPage:写到物理页中的页号
    /*wBlock:页替换的时候将替换掉的那页内容写回磁盘，wBlock是要写的磁盘块号，如果为-1，表示
    页表没满，不需要写回*/
    mmgr->LRU(pId,addr,wPage,wBlock);

    //whether the page table is full or not
    if ( wBlock != -1 )
    {
 //       qDebug() << "页表满了";
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

       // qDebug() << buff[0] << buff[1] << buff[2];
        mmgr->writeMemPage(wPage,buff);

    //    qDebug() <<"进程" << runningPCB->getPId() << " "<< blockNum << " " << wPage;
//        for(int i = 0; i < page_size; i ++)
//        {
//             qDebug() << buff[i];
//        }

    }

}

void DeviceDriver::finishPCB(int res)
{
    if(res == OK)
    {
        // send signal to manager
 //       qDebug() << "进程"<<runningPCB->getPId()<<"响应正常";
        emit tellManFinIO(runningPCB);
    }
    else
    {
  //      qDebug() << "进程"<<runningPCB->getPId()<<"响应异常结束";
        emit tellError(runningPCB);
    }

    runningPCB = NULL;
}

// called by manager
void DeviceDriver::pushWaitingQue(PCB* pcb)
{
    this->waitingQue.push(pcb);
}
