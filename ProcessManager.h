#include "PCB.h"
#include "IODevice.h"
#include <queue>
#include <map>
#include <string>
#include <vector>

using namespace std;

class ProcessManager
{
	private:
		static bool cmp(PCB* a, PCB* b) { return a->getPriority() < b->getPriority(); }

		priority_queue<PCB*, vector<PCB*>, decltype(&cmp)> readyQue;
		PCB* runningPCB;					//运行中的PCB
		map<PCB*, IODevice*> blockedMap;	//存阻塞PCB对应的阻塞原因（设备）
		map<Event, IODevice*> deviceMap;	//设备表，根据阻塞事件对应具体的IO设备

		
	public:
		// keep listening signals from other threads(cpu, iodevices)
		void run();

		void createProcess(string workDir, char[] program);
		
		void ready2run();
		
		void run2blocked();
		
		// served to be called by IODevice
		void blocked2run(PCB* pcb);

		void run2dead();

		// When there is a new PCB into readyQue, try to use the top to preempt the runningPCB
		bool tryPreemp();
		
		void saveContext();
		
		void recoverContext();
		
				
};
