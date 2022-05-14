#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <malloc.h>
using namespace std;

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

extern bool debug;

/////////*DATA*//////
typedef struct INODE {		// iNode
	unsigned short i_mode; //文件类型
	int i_size;     //文件大小
	int nlinks;     //链接数
	int block_address[FBLK_NUM]; //文件数据block的位置
	int open_num;   //0未打开，1已打开
}iNode;

typedef struct directory {	// 文件目录项结构：当前目录下一系列的文件列表
	string file_name;   //文件名称
	unsigned int iNode_no;  //iNode编号
}dir;

typedef struct OS_FILE {	// 文件句柄
	iNode* f_iNode; //指向对应的iNode
	long int f_pos; //读指针
}os_file;

//超级块结构
typedef struct super_block {
	unsigned  sb_inodenum; //磁盘中inode节点数
	unsigned  sb_blocknum; //磁盘中的物理块数
	unsigned  sb_filemaxsize; //文件的最大长度
}super_block;

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
	static void init_dir(dir blankdir[]);	//	实现目录的初始化
	static vector<pair<string, unsigned short>> os_ls();	// 实现ls命令，列出当前目录下所有文件
	static bool os_cd(string newpath);	//	实现cd命令，实现目录的跳转
	static bool os_rmdir(string dir_name);	//	实现rmdir filename命令，删除当前目录下名为filename的空目录
};

class File {
public:
	static void init_iNode(iNode* blankiNode);	//	实现一个iNode的初始化
	static iNode Fill_in_iNode(unsigned short f_type);	// 完善新建文件的iNode信息
	static iNode* Create_File(string filename, unsigned short f_type);	// 创建文件————Create_File(filename, 0/1);
	static os_file* Open_File(string f_name);	// 打开文件，返回一个文件句柄————Open_File(filename);
	static void Close_File(os_file* f);	// 关闭文件：修改open_num并释放句柄————Close_File(fp);
	static int os_rm(string f_name);	// 删除普通文件————os_rm(filename);
};

class disk {
public:
	static bool format_disk();//格式化磁盘，成功返回True
	static bool init_disk(); //初始化磁盘
	static bool start_disk();//启动磁盘
	static int first_free();//找到第一个空闲块号，修改其bitmap表示被占用，并将其返回
	static int release_block(int block);//释放一个被占用的块
	static int get_filesize(os_file* fp);//获取文件大小
	static void get_dir(void* dir_buf, iNode* f_inode);//面向文件系统的读目录接口
	static int os_readfile(void* v_buf, int size, os_file* fp);//内部读文件接口
	static int os_writefile(void* v_buf, int size, os_file* fp);//内部读文件接口

	/*	对外接口	*/
	static bool write_block(long block, char* buf);//磁盘块写入缓冲区
	static bool read_block(long block, char* buf);//磁盘块读入缓冲区
	/*	对外接口结束	*/
};

class FileManager {
	friend class DIR;
	friend class File;
	friend class FileTOOLS;
	friend class disk;
public:
	// 全局变量
	static iNode iNode_table[iNode_NUM];  //iNode table的数组，数组下标对应iNode编号
	static dir root_dir[MAX_FILE_NUM];    //根目录 数组实现  往下的每个子目录也是dir类型的数组，每一项是一个文件目录项
	static dir* current_dir;			  //保存每次更新analyse_path返回的dir数组，即当前目录的dir数组
	static stack<string> WorkingDir;	  //记录路径名称
	static stack<unsigned int> WorkingNo; //记录路径上i节点的标号
	static map<int, unsigned long int> NumOfFile;

	/*	对外接口	*/
	static void InitFileSys();			// 初始化文件管理系统，开机后就要做的事情
	static bool ls();					//	返回当前目录下的所有的文件名称、类型####
	static string pwd();				//	显示当前所在路径,返回当前路径字符串
	static bool cd(string dirname);		//	成功返回 T，不成功返回 F
	static bool cat(string filename); 	//	打开txt文件，并打印显示
	static bool mkdir(string dirname);	//	在当前目录下创建新目录
	static bool mkfile(string filename, unsigned short filetype);	//	在当前目录下创建新文件
	static bool rmdir(string dirname);		//	在当前目录下删除子目录，成功返回 T，不成功返回 F
	static bool rmfile(string filename);	//	在当前目录下删除子文件，成功返回 T，不成功返回 F
	static int openFile(string filename);	//	打开一个文件，返回文件描述符标识ID
	static void closeFile(int filenum);		//	关闭文件描述符为filenum的文件
	static int readfile(int filenum, int size, void* v_buf);//面向进程的读文件接口
	static int writefile(int filenum, int size, void* v_buf);//面向进程的写文件接口
	static unsigned short getFileType(string filename);	//返回当前目录下的一个文件的类型
	/*	对外接口结束	*/

	/*	文件内部测试所用函数	*/
	static void waitForInput();
	/*	文件内部测试所用函数结束	*/

private:
	/*	文件内部测试所用函数	*/
	static string trim(string str);
	static string displayPath(int flag);
	static void InputAnalyse(vector<string> args);
	static void InputCut(string input);
	/*	文件内部测试所用函数结束	*/
};

