#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

#include "file_dir.h"
using namespace std;

// iNode初始化函数 
void File::init_iNode(iNode* blankiNode) {
	blankiNode->i_mode = 3;   //未定义文件类型
	blankiNode->i_size = 0;
	blankiNode->nlinks = 0;
	blankiNode->open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		if (blankiNode->block_address[i] < MAX_BLOCK_NUM)
			blankiNode->block_address[i] = MAX_BLOCK_NUM + 1;
	}
}

// 完善新建文件的iNode信息
iNode File::Fill_in_iNode(unsigned short f_type) {
	iNode new_iNode;
	new_iNode.i_mode = f_type; //0目录，1 txt, 2 exe
	if (f_type)
		new_iNode.i_size = 0;//普通文件大小为0
	else
		new_iNode.i_size = 1;//目录大小为1
	new_iNode.nlinks = 1;
	new_iNode.open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		new_iNode.block_address[i] = MAX_BLOCK_NUM + 1;
	}
	return new_iNode;
}

// 创建文件————Create_File(filename, 0/1);
iNode* File::Create_File(string filename, unsigned short f_type) {
	dir* item;
	int cur_i_no;
	if (filename[0] != '/') {
		if (!FileManager::current_dir) {
            qDebug() << "ERROR——当前目录发生错误" << Qt::endl;
			return NULL;
		}
		item = FileManager::current_dir;
		cur_i_no = FileManager::WorkingNo.top();
	}
	else {	//文件名起始符为/，不合法
        qDebug() << "ERROR——文件名不合法" << Qt::endl;
		return NULL;
	}
	//判断文件名长度是否合法
	if (filename.length() > Name_length) {
        qDebug() << "ERROR——文件名长度过长" << Qt::endl;
		return NULL;
	}
	//判断是否存在同名的文件
	if (fileTOOLS::same_name(filename, item)) {
        qDebug() << "ERROR——文件已经存在" << Qt::endl;
		return NULL;
	}

	//创建文件目录项，放在文件所在的目录的directory列表中 
	int i;
	for (i = 0; i < DIR_FILE_NUM && (*(item + i)).iNode_no != iNode_NUM + 1; i++) {} //找到item的尾项
	if (i == DIR_FILE_NUM) {
        qDebug() << "ERROR——当前目录下文件容量已满" << Qt::endl;
		return NULL;
	}
	unsigned int temp_no = 0;
	int x;	//查找空的i节点
	for (x = 0; x < iNode_NUM; x++)
		if (FileManager::iNode_table[x].nlinks == 0) {
			temp_no = x;
			break;
		}
	if (x == iNode_NUM) {
        qDebug() << "ERROR——没有可用的i节点" << Qt::endl;
		return NULL;
	}
	else { //在当前目录数组尾项添加文件目录项  	
        (*(item + i)).file_name = QString::fromStdString(filename);
		(*(item + i)).iNode_no = temp_no;
	}
	//完善iNode节点信息,并将iNode节点放在iNode table中  
	iNode newiNode = Fill_in_iNode(f_type);

	if (f_type == 0) { //f_type为0时说明创建目录文件
        dir* temp_dir = new dir[DIR_FILE_NUM];
		DIR::init_dir(temp_dir); //初始化dir
		FILE* disk_p = fopen(DISK, "rb+");
		int blk_addr;
		//当前情况下一个目录占四个block
		for (int k = 0; k < FBLK_NUM; k++) {
			blk_addr = disk::first_free();//每次从磁盘上找一个空间####磁盘部分
			newiNode.block_address[k] = blk_addr;
			fseek(disk_p, blk_addr * BLOCK_SIZE, SEEK_SET);//在磁盘上写下相应信息
            fwrite(((char*)temp_dir + k * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
		}
		fclose(disk_p);
	}
	FileManager::iNode_table[(*(item + i)).iNode_no] = newiNode;
	//在磁盘上写下相应信息，FileManager::iNode_table更新，item所指的目录进行更新(利用cur_iNode)
	FILE* disk_p = fopen(DISK, "rb+");
	fseek(disk_p, INODE_START * BLOCK_SIZE, SEEK_SET); //文件指针定位至inode_table处
	fwrite(FileManager::iNode_table, sizeof(FileManager::iNode_table), 1, disk_p); //iNode_table更新
	//当前目录下的目录信息写回磁盘
	char* buf = (char*)item;
	for (int k = 0; k < FBLK_NUM; k++) {
		fseek(disk_p, FileManager::iNode_table[cur_i_no].block_address[k] * BLOCK_SIZE, SEEK_SET);
		fwrite((buf + k * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
	}
	fclose(disk_p);

//     if((iNode*)&newiNode == NULL)
//     {
//           qDebug() << "1112";
//     }
     iNode* nINode = new iNode();

     for(int i = 0; i < FBLK_NUM; i ++)
     {
          nINode->block_address[i] = newiNode.block_address[i];
     }
     nINode->i_mode = newiNode.i_mode;
     nINode->i_size = newiNode.i_size;
     nINode->nlinks = newiNode.nlinks;
     nINode->open_num = newiNode.open_num;

    return nINode;
}

// 打开文件————Open_File(filename);
// 根据文件名定位到其iNode，修改open_num，创建文件句柄os_file 
os_file* File::Open_File(string f_name)
{
	os_file* current_file = (os_file*)malloc(sizeof(os_file));
	dir* current = FileManager::current_dir;//当前所在目录的dir*数组
	if (!current) {
        qDebug() << "ERROR——当前路径出错" << Qt::endl;
		return NULL;
	}
	unsigned int x = fileTOOLS::find_son_iNode(f_name, current);
	if (x == iNode_NUM + 1) {
        qDebug() << "ERROR——当前目录下找不到该文件" << Qt::endl;
		return NULL;
	}
	current_file->f_iNode = FileManager::iNode_table + x;

	//判断是否已经打开
	if (current_file->f_iNode->open_num == 1) {
        qDebug() << "ERROR——文件已经被打开" << Qt::endl;
		return NULL;
	}

   // qDebug() << "-------------------------------";
	current_file->f_pos = 0; //读指针置0
	current_file->f_iNode->open_num = 1; //打开文件，置1 
	FileManager::WorkingDir.push(f_name);
	FileManager::WorkingNo.push(x);
	return current_file;
}

// 关闭文件：修改open_num并释放句柄————Close_File(fp);
void File::Close_File(os_file* f) {
	FileManager::WorkingDir.pop();
	FileManager::WorkingNo.pop();
	f->f_iNode->open_num = 0;
	f->f_pos = 0;
	free(f);
}

// 删除普通文件————os_rm(filename);
int File::os_rm(string f_name) {
	int f_i = fileTOOLS::find_son_pos(f_name); //找到了要删除的文件目录项 
	if (f_i == DIR_FILE_NUM + 1) {
        qDebug() << "ERROR——不可删除该文件" << Qt::endl;
		return 0;// 失败 
	}
	if (FileManager::iNode_table[(*(FileManager::current_dir + f_i)).iNode_no].i_mode == 0) {
        qDebug() << "ERROR——该文件为目录文件" << Qt::endl;
		return 0; //目录文件，错误
	}
	return fileTOOLS::Delete_File(f_i); //删除文件
}
