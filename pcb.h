#pragma once
#include<string>
#include<map>

#define REG_NUM 4

using namespace std;

enum Status
{
    ready,
    running,
    blocked,
    dead
};

enum Event
{
    normal,
    std_io,
    disk_io
};

//enum IOtype
//{
//    type_stdio,
//    type_diskio
//};

enum Operation {
    op_read,
    op_write
};

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
        int dataStart,dataEnd;
        int base,top;

        //CPU控制信息
        int reg[REG_NUM];
        int PC;

        //文件系统信息
        string workDir;
        map<int,int> fileTable;
        Operation ope;
        int activeFile;

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

//        IOtype getIOtype()
//        {
//            return this->ioType;
//        }
//        void setIOtype(IOtype type)
//        {
//            this->ioType = type;
//        }

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

        int getDataStart()
        {
            return this->dataStart;
        }
        void setDataStart(int start)
        {
            this->dataStart = start;
        }

        int getDataEnd()
        {
            return this->dataEnd;
        }
        void setDataEnd(int end)
        {
            this->dataEnd = end;
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

        int* getReg()
        {
            return this->reg;
        }

        void setReg(int r[])
        {
            this->reg = r;
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

        map<int,int> getFileTable()
        {
            return this->fileTable;
        }
        void setFileTable(map<int,int> file_table)
        {
            this->fileTable = file_table;
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
};
