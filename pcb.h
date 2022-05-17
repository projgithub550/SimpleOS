#ifndef PCB_H
#define PCB_H

#include<string>
#include<map>
#include<vector>
#include<algorithm>
#include "constant.h"

using namespace std;


class PCB
{
    private:
        int pId;

        //进程调度信息
        Status status;
        int priority;
        Event event;

        //进程控制信息
        int textStart,textEnd;
        int base,top;

        //CPU控制信息
        int reg[REG_NUM];
        int PC;

        //文件系统信息
        string workDir;
        vector<int> fileIds;
        Operation ope;
        int activeFile;
        int size;
        int startAddr;

        bool isPageFault;


    //getter and setter
    public:
        int getPId()
        {
            return this->pId;
        }
        void setPId(int id)
        {
            this->pId = id;
        }

        Status getStatus()
        {
            return this->status;
        }
        void setStatus(Status new_status)
        {
            this->status = new_status;
        }

        int getPriority()
        {
            return this->priority;
        }
        void setPriority(int _priority)
        {
            this->priority = _priority;
        }

        Event getEvent()
        {
            return this->event;
        }
        void setEvent(Event _event)
        {
            this->event = _event;
        }
        int getTextStart()
        {
            return this->textStart;
        }
        void setTextStart(int start)
        {
            this->textStart = start;
        }

        int getTextEnd()
        {
            return this->textEnd;
        }
        void setTextEnd(int end)
        {
            this->textEnd = end;
        }

        int getBase()
        {
            return this->base;
        }
        void setBase(int _base)
        {
            this->base = _base;
        }

        int getTop()
        {
            return this->top;
        }
        void setTop(int _top)
        {
            this->top = _top;
        }

        int getReg(int i)
        {
            return this->reg[i];
        }

        void setReg(int i,int value)
        {
            this->reg[i] = value;
        }

        int getPC()
        {
            return this->PC;
        }
        void setPC(int pc)
        {
            this->PC = pc;
        }

        string getWorkDir()
        {
            return this->workDir;
        }
        void setWorkDir(string work_dir)
        {
            this->workDir = workDir;
        }

        vector<int> getFileIds()
        {
            return this->fileIds;
        }
        void pushFileIds(int fileId)
        {
            vector<int>::iterator ib = this->fileIds.begin();
             vector<int>::iterator ie = this->fileIds.end();
            if(find(ib,ie,fileId) == ie)
            {
                this->fileIds.push_back(fileId);
            }
        }

        int getActiveFile()
        {
            return activeFile;
        }
        void setActiveFile(int file)
        {
            activeFile = file;
        }

        Operation getOperation()
        {
            return ope;
        }
        void setOperation(Operation o)
        {
            ope=o;
        }

        int getSize()
        {
            return this->size;
        }
        void setSize(int _size)
        {
            this->size = _size;
        }

        int getStartAddr()
        {
            return this->startAddr;
        }
        void setStartAddr(int start_addr)
        {
           this->startAddr = start_addr;
        }

        bool getIsPageFault()
        {
            return this->isPageFault;
        }
        void setIsPageFault(bool isFault)
        {
           this->isPageFault = isFault;
        }

};

#endif
