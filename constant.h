#ifndef CONSTANT_H
#define CONSTANT_H

//process

//cpu
#define REG_NUM 4

//driver

//memory
#define NONE -2;
#define invalid_value -1;
#define Valid 1;
int page_size=4096;
int max_page_number=16;
int q_max_page_number=4;
int physical_blocks = 32;


//filesystem
#define BLOCK_SIZE 64		//磁盘块大小\数据块大小 64字节
#define MAX_FILE_NUM 256		//最大文件数量，也是最大inode的数量
#define MAX_BLOCK_NUM 1024	//最大磁盘块数量，即磁盘总容量
#define MAX_FILE_SIZE 256		//最大文件长度，256字节，占四个磁盘块
#define INODE_START 1		//inode区起始块号，0为第一块,为超级块块号
#define BITMAP_START 129		//bitmap区起始块号（inode区占256*32B，占128个块
#define DATA_START (BITMAP_START + (sizeof(char)*MAX_BLOCK_NUM)/BLOCK_SIZE) //data区起始块号 (bitmap区起始块号加位图占用块数
#define DISK "disk.bin"		//定义模拟磁盘文件
#define Name_length 14		//文件名称最大长度
#define iNode_NUM 256		//iNode的数量
#define DIR_FILE_NUM 8		//每个目录文件下的文件最大个数
#define MAX_DIR_DEPTH 256	//限制最大的目录深度
#define PATH_LENGTH 100		//路径字符串最大长度
#define FBLK_NUM 4		//文件中block的个数



#endif // CONSTANT_H
