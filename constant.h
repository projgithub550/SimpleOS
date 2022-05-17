#ifndef CONSTANT_H
#define CONSTANT_H

#include<vector>
#include<string>
#include<map>
#include<queue>

using namespace std;

//process
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
    disk_io,
};


enum Operation {
    op_read,
    op_write
};

//cpu
const int BLOCK_IO = -2;   // 读写文件中断
const int BLOCK_DISK = -1; // 缺页中断
const int NEXT = 1;		   // 指令正常执行
const int END = 0;		   // 指令结束执行
const int BLOCK = -1;	   // 读写指令失败后返回值，意味着缺页中断的发生

#define REG_NUM 8

//driver


enum IOType
{
    stdrd,
    disk
};

#define N_DR 2
#define OK 1
#define ERR -1

//memory

#define Memory "memory.bin"		//定义模拟磁盘文件
#define Physical_blocks_Not_exist -3
#define disk_Not_exist -2
#define invalid_value -1
#define Valid 1

#define page_size 1024//页大小
#define occupancy 16 //页表实际占用的页

#define max_page_number 64 //虚拟页表最多含有的页
#define q_max_page_number 16 //快表最多含有的页
#define schedule_queue_length 16 //调度队列

//filesystem
#define BLOCK_SIZE 1024		//磁盘块大小\数据块大小 64字节#####改成了1024
#define MAX_FILE_NUM 256	//最大文件数量，也是最大inode的数量
#define MAX_BLOCK_NUM 1024	//最大磁盘块数量，即磁盘总容量
#define MAX_FILE_SIZE 256	//最大文件长度，256字节，占四个磁盘块
#define INODE_START 1		//inode区起始块号，0为第一块,为超级块块号
#define BITMAP_START 129	//bitmap区起始块号（inode区占256*32B，占128个块
#define DATA_START (BITMAP_START + (sizeof(char)*MAX_BLOCK_NUM)/BLOCK_SIZE) //data区起始块号 (bitmap区起始块号加位图占用块数
#define DISK "disk.bin"		//定义模拟磁盘文件
#define Name_length 14		//文件名称最大长度
#define iNode_NUM 256		//iNode的数量
#define DIR_FILE_NUM 8		//每个目录文件下的文件最大个数
#define MAX_DIR_DEPTH 256	//限制最大的目录深度
#define PATH_LENGTH 100		//路径字符串最大长度
#define FBLK_NUM 4			//文件中block的个数

#endif // CONSTANT_H
