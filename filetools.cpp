#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include "file_dir.h"
using namespace std;

// �ж������ļ�
int fileTOOLS::same_name(string f_name, dir item[]) {
	int i = 0;
	for (i = 0; i < DIR_FILE_NUM && item[i].iNode_no != iNode_NUM + 1 && item[i].file_name != f_name; i++) {}
	if (item[i].iNode_no == iNode_NUM + 1 || i == DIR_FILE_NUM)
		return 0; //����һ�飬û������
	else
		return 1; //������
}

// ���ݸ�Ŀ¼�����ļ����ƣ��ҵ����ļ�iNode�ı��
unsigned int fileTOOLS::find_son_iNode(string son_name, dir* father) {
	int i;
	for (i = 0; i < DIR_FILE_NUM && son_name != (*(father + i)).file_name; i++) {}
	if (i == DIR_FILE_NUM) {
		cout << "ERROR������Ŀ¼��û���ҵ����ļ�" << endl;
		return iNode_NUM + 1;
	}
	return (*(father + i)).iNode_no;
}

// ��dir* FileManager::current_dir�в��ҵ����ļ���Ŀ¼��λ�ã������±꣩ 
int fileTOOLS::find_son_pos(string f_name) {
	int i;
	for (i = 0; i < DIR_FILE_NUM && (*(FileManager::current_dir + i)).file_name != f_name; i++) {}
	if (i == DIR_FILE_NUM) { //Ŀ¼��û�и��ļ�
		cout << "ERROR������ǰĿ¼û���ҵ����ļ�λ��" << endl;
		return DIR_FILE_NUM + 1;
	}
	return i;
}

// �ָ�·������������·������Ϊ���Ŀ¼��
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

// ��������·�����ҵ���ǰĿ¼/�ļ���Ŀ¼ָ��dir*
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
		disk::get_dir(son_dir, &FileManager::iNode_table[FileManager::WorkingNo.top()]);	//���ص�ǰĿ¼��dir####���̲���
		return son_dir;
	}
	else if (path[0] == '/') {// ����·��
		if (dirs[0] != "root") {
			cout << "ERROR��������ľ���·��" << endl;
			return NULL;
		}
		father_dir = FileManager::root_dir;
		working_dir.push_back("root");
		working_no.push_back(0);
		i = 1;
	}
	else {//���·��
		father_dir = FileManager::current_dir;
		i = 0;
	}
	for (i; i < dirs.size(); i++) {
		son = dirs[i];
		find_iNode = find_son_iNode(son, father_dir);//�ҵ���Ŀ¼�¶�Ӧ���ļ�iNode�ı��
		if (find_iNode == iNode_NUM + 1) {
			cout << "ERROR��������Ŀ¼:" << son[i] << endl;
			return NULL;
		}
		if (FileManager::iNode_table[find_iNode].i_mode == 1) {
			cout << "ERROR�����ⲻ��һ��Ŀ¼" << endl;
			return NULL;
		}
		//����son�ļ���dir�ļ�Ŀ¼������son_dir####���̲���
		disk::get_dir(son_dir, &FileManager::iNode_table[find_iNode]);
		father_dir = son_dir;
		working_dir.push_back(son);
		working_no.push_back(find_iNode);
	}
	FileManager::current_dir = son_dir; //���µ�����
	if (path[0] == '/') {//����·��
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

// ��ʽ��һ��iNode
void fileTOOLS::format_iNode(iNode* oldiNode) {
	oldiNode->i_mode = 3;
	oldiNode->i_size = 0;
	oldiNode->nlinks = 0;
	oldiNode->open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		if (oldiNode->block_address[i] < MAX_BLOCK_NUM) {
			//����鱻ռ�����ͷſ�,�޸�bitmap
			disk::release_block(oldiNode->block_address[i]);//####���̲���
			oldiNode->block_address[i] = MAX_BLOCK_NUM + 1;
		}
		else
			break;
	}
}

// ɾ���ļ�
int fileTOOLS::Delete_File(int f_i) {
	unsigned int crt_no = (*(FileManager::current_dir + f_i)).iNode_no;
	iNode crt_inode = FileManager::iNode_table[crt_no];
	//��ʽ��Ҫɾ�����ļ���iNode
	format_iNode(FileManager::iNode_table + (*(FileManager::current_dir + f_i)).iNode_no);
	//����Ҫɾ�����ļ�������Ŀ¼��������ļ�ǰ�ƣ�
	int j;
	for (j = f_i; j < DIR_FILE_NUM && (*(FileManager::current_dir + j)).iNode_no != iNode_NUM + 1; j++) {
		(*(FileManager::current_dir + j)).file_name = (*(FileManager::current_dir + j + 1)).file_name;
		(*(FileManager::current_dir + j)).iNode_no = (*(FileManager::current_dir + j + 1)).iNode_no;
	}
	//buf���浱ǰĿ¼������
	char* buf = (char*)malloc(sizeof(dir) * DIR_FILE_NUM);
	memcpy(buf, FileManager::current_dir, sizeof(dir) * DIR_FILE_NUM);
	//�ڴ������޸ĵ�ǰĿ¼����
	FILE* disk_p = fopen(DISK, "rb+");
	for (int i = 0; i < FBLK_NUM; i++) {
		fseek(disk_p, crt_inode.block_address[i] * BLOCK_SIZE, SEEK_SET);
		fwrite((buf + i * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
	}
	//�ڴ������޸�iNode�������
	fseek(disk_p, INODE_START * BLOCK_SIZE, SEEK_SET);
	fwrite(FileManager::iNode_table, sizeof(FileManager::iNode_table), 1, disk_p);

	free(buf);
	fclose(disk_p);
	return 1;
}