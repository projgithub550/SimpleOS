#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <malloc.h>
using namespace std;

/////////*DISK*//////
#define BLOCK_SIZE 64//磁盘块大小 64字节
#define MAX_FILE_NUM 256 //最大文件数量，也是最大inode的数量
#define MAX_BLOCK_NUM 1024 //最大磁盘块数量，即磁盘总容量
#define MAX_FILE_SIZE 256 //最大文件长度，256字节，占四个磁盘块
#define INODE_START 1 //inode区起始块号，0为第一块,为超级块块号
#define BITMAP_START 129//bitmap区起始块号（inode区占256*32B，占128个块
#define DATA_START (BITMAP_START + (sizeof(char)*MAX_BLOCK_NUM)/BLOCK_SIZE) //data区起始块号 (bitmap区起始块号加位图占用块数
#define DISK "disk.bin"
/////////*DIR*///////
#define Name_length 14	//文件名称最大长度
#define iNode_NUM 256	//iNode的数量
#define DIR_FILE_NUM 8	//每个目录文件下的文件最大个数
#define MAX_DIR_DEPTH 256 //限制最大的目录深度
#define PATH_LENGTH 100	//路径字符串最大长度
#define FBLK_NUM 4		//文件中block的个数
#define RDONLY 00 //只读
#define WRONLY 01 //只写
#define RDWR 02	  //读写
/////////*FILE*//////
#define DEV_NAME "disk.bin"
#define BLOCK_SIZE 64	//数据块大小，单位：字节
#define INODE_START 1 	//inode区起始块号（0为第一块）
/////////*DATA*//////
// iNode
typedef struct INODE {
	unsigned short i_mode; //文件类型
	int i_size;     //文件大小
	int nlinks;     //链接数
	int block_address[FBLK_NUM]; //文件数据block的位置
	int open_num;   //0未打开，1已打开
}iNode;

// 记录当前所处的路径
extern stack<string> WorkingDir;   //记录路径名称
extern stack<unsigned int> WorkingNo;    //记录路径上i节点的标号
extern bool debug;
// 文件目录项结构：当前目录下一系列的文件列表
typedef struct directory {
	string file_name;   //文件名称
	unsigned int iNode_no;  //iNode编号
}dir;

// 文件句柄
typedef struct OS_FILE {
	iNode* f_iNode; //指向对应的iNode
	long int f_pos; //读指针
}os_file;

// 全局变量
extern iNode iNode_table[iNode_NUM];  //iNode table的数组，数组下标对应iNode编号
extern dir root_dir[MAX_FILE_NUM];   //根目录 数组实现  往下的每个子目录也是dir类型的数组，每一项是一个文件目录项
extern dir* current_dir;   //保存每次更新analyse_path返回的dir数组，即当前目录的dir数组

class fileTOOLS {
private:
	static vector<string> split_dir(string path);
	static void format_iNode(iNode* oldiNode);
public:
	static int same_name(string f_name, dir item[]);
	static unsigned int find_son_iNode(string son_name, dir* father);
	static int find_son_pos(string f_name);
	static dir* analyse_Path(string path);
	static int Delete_File(int f_i);
};

class DIR {
public:
	static void init_dir(dir blankdir[]);
	static vector<pair<string, unsigned short>> os_ls();
	static bool os_cd(string newpath);
	static bool os_rmdir(string dir_name);
};

class File {
public:
	static void init_iNode(iNode* blankiNode);
	static iNode Fill_in_iNode(unsigned short f_type);
	static iNode* Create_File(string filename, unsigned short f_type);
	static os_file* Open_File(string f_name);
	static void Close_File(os_file* f);
	static int os_rm(string f_name);
};




typedef struct super_block {  //超级块结构
	unsigned  sb_inodenum; //磁盘中inode节点数
	unsigned  sb_blocknum; //磁盘中的物理块数
	unsigned  sb_filemaxsize; //文件的最大长度
}super_block;

// 格式化磁盘，向磁盘文件中写入'0'，成功返回True
bool format_disk();
// 初始化磁盘
bool init_disk();
// 启动磁盘
bool start_disk();

// 找到第一个空闲块号，修改其bitmap表示被占用，并将其返回
int first_free();
// 释放一个被占用的块
int release_block(int block);

// 获取文件大小
int get_filesize(os_file* fp);
// 面向进程的读文件接口
int os_readfile(void* v_buf, int size, os_file* fp);
// 面向进程的写文件接口
int os_writefile(void* v_buf, int size, os_file* fp);
//面向文件系统读目录接口
void get_dir(void* dir_buf, iNode* f_inode);