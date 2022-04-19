#include "DeviceDriver.h"

// execute a pcb(slot func)
void DeviceDriver::executePCB(QString name) {
    // 判断manager通知的io设备是不是自己（manager同时通知多个io设备）
    if(name!=this->name) return;

	// start executing
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

// change the running pcb
void DeviceDriver::wait2run() {
	// unable to fetch a PCB
	if (waitingQue.empty() || runningPCB != nullptr) return;

	PCB* pcb = waitingQue.front();
	waitingQue.pop();

	runningPCB = pcb;

	// signal iodevice thread to begin executing
    emit tellIOExec();
}

void DeviceDriver::finishPCB() {
	// send signal to manager
    emit tellManFinIO(runningPCB);
	
	runningPCB = nullptr;

	// change runningPCB
	wait2run();
}

// called by manager
void DeviceDriver::pushWaitingQue(PCB* pcb) {
	waitingQue.push(pcb); 

	// try executing, false when there is a pcb running
	wait2run();

}
