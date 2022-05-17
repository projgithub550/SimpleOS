#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include "file_dir.h"
using namespace std;

// 判断重名文件
int fileTOOLS::same_name(string f_name, dir item[]) {
	int i = 0;
	for (i = 0; i < DIR_FILE_NUM && item[i].iNode_no != iNode_NUM + 1 && item[i].file_name != f_name; i++) {}
	if (item[i].iNode_no == iNode_NUM + 1 || i == DIR_FILE_NUM)
		return 0; //遍历一遍，没有重名
	else
		return 1; //有重名
}

// 根据父目录和子文件名称，找到子文件iNode的编号
unsigned int fileTOOLS::find_son_iNode(string son_name, dir* father) {
	int i;
	for (i = 0; i < DIR_FILE_NUM && son_name != (*(father + i)).file_name; i++) {}
	if (i == DIR_FILE_NUM) {
		cout << "ERROR——父目录下没有找到该文件" << endl;
		return iNode_NUM + 1;
	}
	return (*(father + i)).iNode_no;
}

// 从dir* FileManager::current_dir中查找到子文件在目录的位置（数组下标） 
int fileTOOLS::find_son_pos(string f_name) {
	int i;
	for (i = 0; i < DIR_FILE_NUM && (*(FileManager::current_dir + i)).file_name != f_name; i++) {}
	if (i == DIR_FILE_NUM) { //目录中没有该文件
		cout << "ERROR——当前目录没有找到该文件位置" << endl;
		return DIR_FILE_NUM + 1;
	}
	return i;
}

// 分割路径名，将整个路径名分为多个目录名
vector<string> fileTOOLS::split_dir(string path) {
	string temp;
	vector<string> dirs;
	int pos;
	while (path.size()) {
		pos = path.find('/');
		if (pos == string::npos) {
			temp = path;
			path = "";
		}
		else {
			temp = path.substr(0, pos);
			path = path.substr(pos + 1);
		}
		if (temp != "") {
			dirs.push_back(temp);
		}
	}
	return dirs;
}

// 解析绝对路径，找到当前目录/文件的目录指针dir*
dir* fileTOOLS::analyse_Path(string path) {
	vector<string> dirs = split_dir(path);
	dir* son_dir = (dir*)malloc(sizeof(dir) * DIR_FILE_NUM);
	dir* father_dir = (dir*)malloc(sizeof(dir) * DIR_FILE_NUM);
	int find_iNode = FileManager::WorkingNo.top();
	int i;
	string son, father;
	vector<string> working_dir;
	vector<unsigned int> working_no;

	if (path == "") {
		disk::get_dir(son_dir, &FileManager::iNode_table[FileManager::WorkingNo.top()]);	//返回当前目录的dir####磁盘部分
		return son_dir;
	}
	else if (path[0] == '/') {// 绝对路径
		if (dirs[0] != "root") {
			cout << "ERROR——错误的绝对路径" << endl;
			return NULL;
		}
		father_dir = FileManager::root_dir;
		working_dir.push_back("root");
		working_no.push_back(0);
		i = 1;
	}
	else {//相对路径
		father_dir = FileManager::current_dir;
		i = 0;
	}
	for (i; i < dirs.size(); i++) {
		son = dirs[i];
		find_iNode = find_son_iNode(son, father_dir);//找到父目录下对应子文件iNode的标号
		if (find_iNode == iNode_NUM + 1) {
			cout << "ERROR——错误目录:" << son[i] << endl;
			return NULL;
		}
		if (FileManager::iNode_table[find_iNode].i_mode == 1) {
			cout << "ERROR——这不是一个目录" << endl;
			return NULL;
		}
		//返回son文件的dir文件目录项数组son_dir####磁盘部分
		disk::get_dir(son_dir, &FileManager::iNode_table[find_iNode]);
		father_dir = son_dir;
		working_dir.push_back(son);
		working_no.push_back(find_iNode);
	}
	FileManager::current_dir = son_dir; //更新到表中
	if (path[0] == '/') {//绝对路径
		while (!FileManager::WorkingDir.empty()) {
			FileManager::WorkingDir.pop();
			FileManager::WorkingNo.pop();
		}
	}
	for (int i = 0; i < dirs.size(); i++) {
		FileManager::WorkingDir.push(working_dir[i]);
		FileManager::WorkingNo.push(working_no[i]);
	}
	return FileManager::current_dir;
}

// 格式化一个iNode
void fileTOOLS::format_iNode(iNode* oldiNode) {
	oldiNode->i_mode = 3;
	oldiNode->i_size = 0;
	oldiNode->nlinks = 0;
	oldiNode->open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		if (oldiNode->block_address[i] < MAX_BLOCK_NUM) {
			//如果块被占用则释放块,修改bitmap
			disk::release_block(oldiNode->block_address[i]);//####磁盘部分
			oldiNode->block_address[i] = MAX_BLOCK_NUM + 1;
		}
		else
			break;
	}
}

// 删除文件
int fileTOOLS::Delete_File(int f_i) {
	unsigned int crt_no = (*(FileManager::current_dir + f_i)).iNode_no;
	iNode crt_inode = FileManager::iNode_table[crt_no];
	//格式化要删除的文件的iNode
	format_iNode(FileManager::iNode_table + (*(FileManager::current_dir + f_i)).iNode_no);
	//调整要删除的文件的所在目录（后面的文件前移）
	int j;
	for (j = f_i; j < DIR_FILE_NUM && (*(FileManager::current_dir + j)).iNode_no != iNode_NUM + 1; j++) {
		(*(FileManager::current_dir + j)).file_name = (*(FileManager::current_dir + j + 1)).file_name;
		(*(FileManager::current_dir + j)).iNode_no = (*(FileManager::current_dir + j + 1)).iNode_no;
	}
	//buf保存当前目录的内容
	char* buf = (char*)malloc(sizeof(dir) * DIR_FILE_NUM);
	memcpy(buf, FileManager::current_dir, sizeof(dir) * DIR_FILE_NUM);
	//在磁盘上修改当前目录内容
	FILE* disk_p = fopen(DISK, "rb+");
	for (int i = 0; i < FBLK_NUM; i++) {
		fseek(disk_p, crt_inode.block_address[i] * BLOCK_SIZE, SEEK_SET);
		fwrite((buf + i * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
	}
	//在磁盘上修改iNode表的内容
	fseek(disk_p, INODE_START * BLOCK_SIZE, SEEK_SET);
	fwrite(FileManager::iNode_table, sizeof(FileManager::iNode_table), 1, disk_p);

	free(buf);
	fclose(disk_p);
	return 1;
}