#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <malloc.h>
using namespace std;

/////////*DISK*//////
#define BLOCK_SIZE 64//���̿��С 64�ֽ�
#define MAX_FILE_NUM 256 //����ļ�������Ҳ�����inode������
#define MAX_BLOCK_NUM 1024 //�����̿�������������������
#define MAX_FILE_SIZE 256 //����ļ����ȣ�256�ֽڣ�ռ�ĸ����̿�
#define INODE_START 1 //inode����ʼ��ţ�0Ϊ��һ��,Ϊ��������
#define BITMAP_START 129//bitmap����ʼ��ţ�inode��ռ256*32B��ռ128����
#define DATA_START (BITMAP_START + (sizeof(char)*MAX_BLOCK_NUM)/BLOCK_SIZE) //data����ʼ��� (bitmap����ʼ��ż�λͼռ�ÿ���
#define DISK "disk.bin"
/////////*DIR*///////
#define Name_length 14	//�ļ�������󳤶�
#define iNode_NUM 256	//iNode������
#define DIR_FILE_NUM 8	//ÿ��Ŀ¼�ļ��µ��ļ�������
#define MAX_DIR_DEPTH 256 //��������Ŀ¼���
#define PATH_LENGTH 100	//·���ַ�����󳤶�
#define FBLK_NUM 4		//�ļ���block�ĸ���
#define RDONLY 00 //ֻ��
#define WRONLY 01 //ֻд
#define RDWR 02	  //��д
/////////*FILE*//////
#define DEV_NAME "disk.bin"
#define BLOCK_SIZE 64	//���ݿ��С����λ���ֽ�
#define INODE_START 1 	//inode����ʼ��ţ�0Ϊ��һ�飩
/////////*DATA*//////
// iNode
typedef struct INODE {
	unsigned short i_mode; //�ļ�����
	int i_size;     //�ļ���С
	int nlinks;     //������
	int block_address[FBLK_NUM]; //�ļ�����block��λ��
	int open_num;   //0δ�򿪣�1�Ѵ�
}iNode;

// ��¼��ǰ������·��
extern stack<string> WorkingDir;   //��¼·������
extern stack<unsigned int> WorkingNo;    //��¼·����i�ڵ�ı��
extern bool debug;
// �ļ�Ŀ¼��ṹ����ǰĿ¼��һϵ�е��ļ��б�
typedef struct directory {
	string file_name;   //�ļ�����
	unsigned int iNode_no;  //iNode���
}dir;

// �ļ����
typedef struct OS_FILE {
	iNode* f_iNode; //ָ���Ӧ��iNode
	long int f_pos; //��ָ��
}os_file;

// ȫ�ֱ���
extern iNode iNode_table[iNode_NUM];  //iNode table�����飬�����±��ӦiNode���
extern dir root_dir[MAX_FILE_NUM];   //��Ŀ¼ ����ʵ��  ���µ�ÿ����Ŀ¼Ҳ��dir���͵����飬ÿһ����һ���ļ�Ŀ¼��
extern dir* current_dir;   //����ÿ�θ���analyse_path���ص�dir���飬����ǰĿ¼��dir����

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




typedef struct super_block {  //������ṹ
	unsigned  sb_inodenum; //������inode�ڵ���
	unsigned  sb_blocknum; //�����е��������
	unsigned  sb_filemaxsize; //�ļ�����󳤶�
}super_block;

// ��ʽ�����̣�������ļ���д��'0'���ɹ�����True
bool format_disk();
// ��ʼ������
bool init_disk();
// ��������
bool start_disk();

// �ҵ���һ�����п�ţ��޸���bitmap��ʾ��ռ�ã������䷵��
int first_free();
// �ͷ�һ����ռ�õĿ�
int release_block(int block);

// ��ȡ�ļ���С
int get_filesize(os_file* fp);
// ������̵Ķ��ļ��ӿ�
int os_readfile(void* v_buf, int size, os_file* fp);
// ������̵�д�ļ��ӿ�
int os_writefile(void* v_buf, int size, os_file* fp);
//�����ļ�ϵͳ��Ŀ¼�ӿ�
void get_dir(void* dir_buf, iNode* f_inode);