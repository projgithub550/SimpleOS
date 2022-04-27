#include "device_driver.h"

// execute a pcb(slot func)
void DeviceDriver::handleEvent(IOType _type)
{
    // whether the signal is for me
    if(this->type != _type) return;

	// start executing
    runningPCB = waitingQue.front();

    //step 1: find the file we are about to operate
    int operFile = runningPCB->getActiveFile();

    //step 2: operate this file through fileSystem interface
    if(runningPCB->getOperation() == op_read)//read
    {
        char content[] = readFile(openfile);

        writeMem(content,startAddr);
    } else {
        char content[] = readMem(startAddr,size);

        writeFile(openFile,content);
    }

	// finish executing
	finishPCB();
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
