# include <iostream>
# include <thread>
# include <map>
# include "ProcessManager.h"
# include "CPU.h"

using namespace std;

thread* managerThread, *cpuThread;
map<IODevice*, thread*> ioThread;

int main() {
	// create object and init
	ProcessManager* manager = new ProcessManager;
	CPU* cpu = new CPU;
	IODevice* device = new IODevice;

	// create a manager thread
	managerThread = new thread(&ProcessManager::run, manager);

	// create a cpu thread
	cpuThread = new thread(&CPU::run, cpu);

	// create a thread for every io device
	ioThread[device] = new thread(&IODevice::run, device);

	return 0;
}