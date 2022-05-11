#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include "file_dir.h"
using namespace std;

// iNode��ʼ������ 
void File::init_iNode(iNode* blankiNode) {
	blankiNode->i_mode = 3;   //δ�����ļ�����
	blankiNode->i_size = 0;
	blankiNode->nlinks = 0;
	blankiNode->open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		if (blankiNode->block_address[i] < MAX_BLOCK_NUM)
			blankiNode->block_address[i] = MAX_BLOCK_NUM + 1;
	}
}

// �����½��ļ���iNode��Ϣ
iNode File::Fill_in_iNode(unsigned short f_type) {
	iNode new_iNode;
	new_iNode.i_mode = f_type; //0Ŀ¼��1 txt, 2 exe
	if (f_type)
		new_iNode.i_size = 0;//��ͨ�ļ���СΪ0
	else
		new_iNode.i_size = 1;//Ŀ¼��СΪ1
	new_iNode.nlinks = 1;
	new_iNode.open_num = 0;
	for (int i = 0; i < FBLK_NUM; i++) {
		new_iNode.block_address[i] = MAX_BLOCK_NUM + 1;
	}
	return new_iNode;
}

// �����ļ���������Create_File(filename, 0/1);
iNode* File::Create_File(string filename, unsigned short f_type) {
	dir* item;
	int cur_i_no;
	if (filename[0] != '/') {
		if (!FileManager::current_dir) {
			cout << "ERROR������ǰĿ¼��������" << endl;
			return NULL;
		}
		item = FileManager::current_dir;
		cur_i_no = FileManager::WorkingNo.top();
	}
	else {	//�ļ�����ʼ��Ϊ/�����Ϸ�
		cout << "ERROR�����ļ������Ϸ�" << endl;
		return NULL;
	}
	//�ж��ļ��������Ƿ�Ϸ�
	if (filename.length() > Name_length) {
		cout << "ERROR�����ļ������ȹ���" << endl;
		return NULL;
	}
	//�ж��Ƿ����ͬ�����ļ�
	if (fileTOOLS::same_name(filename, item)) {
		cout << "ERROR�����ļ��Ѿ�����" << endl;
		return NULL;
	}
	//�����ļ�Ŀ¼������ļ����ڵ�Ŀ¼��directory�б��� 
	int i;
	for (i = 0; i < DIR_FILE_NUM && (*(item + i)).iNode_no != iNode_NUM + 1; i++) {} //�ҵ�item��β��
	if (i == DIR_FILE_NUM) {
		cout << "ERROR������ǰĿ¼���ļ���������" << endl;
		return NULL;
	}
	unsigned int temp_no = 0;
	int x;	//���ҿյ�i�ڵ�
	for (x = 0; x < iNode_NUM; x++)
		if (FileManager::iNode_table[x].nlinks == 0) {
			temp_no = x;
			break;
		}
	if (x == iNode_NUM) {
		cout << "ERROR����û�п��õ�i�ڵ�" << endl;
		return NULL;
	}
	else { //�ڵ�ǰĿ¼����β������ļ�Ŀ¼��  	
		(*(item + i)).file_name = filename;
		(*(item + i)).iNode_no = temp_no;
	}
	//����iNode�ڵ���Ϣ,����iNode�ڵ����iNode table��  
	iNode newiNode = Fill_in_iNode(f_type);
	if (f_type == 0) { //f_typeΪ0ʱ˵������Ŀ¼�ļ�
		dir temp_dir[DIR_FILE_NUM];
		DIR::init_dir(temp_dir); //��ʼ��dir
		FILE* disk_p = fopen(DISK, "rb+");
		int blk_addr;
		//��ǰ�����һ��Ŀ¼ռ�ĸ�block
		dir* tmp_buf = temp_dir;
		for (int k = 0; k < FBLK_NUM; k++) {
			blk_addr = disk::first_free();//ÿ�δӴ�������һ���ռ�####���̲���
			newiNode.block_address[k] = blk_addr;
			fseek(disk_p, blk_addr * BLOCK_SIZE, SEEK_SET);//�ڴ�����д����Ӧ��Ϣ
			fwrite(((char*)tmp_buf + k * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
		}
		fclose(disk_p);
	}
	FileManager::iNode_table[(*(item + i)).iNode_no] = newiNode;
	//�ڴ�����д����Ӧ��Ϣ��FileManager::iNode_table���£�item��ָ��Ŀ¼���и���(����cur_iNode)
	FILE* disk_p = fopen(DISK, "rb+");
	fseek(disk_p, INODE_START * BLOCK_SIZE, SEEK_SET); //�ļ�ָ�붨λ��inode_table��
	fwrite(FileManager::iNode_table, sizeof(FileManager::iNode_table), 1, disk_p); //iNode_table����
	//��ǰĿ¼�µ�Ŀ¼��Ϣд�ش���
	char* buf = (char*)item;
	for (int k = 0; k < FBLK_NUM; k++) {
		fseek(disk_p, FileManager::iNode_table[cur_i_no].block_address[k] * BLOCK_SIZE, SEEK_SET);
		fwrite((buf + k * BLOCK_SIZE), BLOCK_SIZE, 1, disk_p);
	}
	fclose(disk_p);
	return &newiNode;
}

// ���ļ���������Open_File(filename);
// �����ļ�����λ����iNode���޸�open_num�������ļ����os_file 
os_file* File::Open_File(string f_name) {
	os_file* current_file = (os_file*)malloc(sizeof(os_file));
	dir* current = FileManager::current_dir;//��ǰ����Ŀ¼��dir*����
	if (!current) {
		cout << "ERROR������ǰ·������" << endl;
		return NULL;
	}
	unsigned int x = fileTOOLS::find_son_iNode(f_name, current);
	if (x == iNode_NUM + 1) {
		cout << "ERROR������ǰĿ¼���Ҳ������ļ�" << endl;
		return NULL;
	}
	current_file->f_iNode = FileManager::iNode_table + x;
	//�ж��Ƿ��Ѿ���
	if (current_file->f_iNode->open_num == 1) {
		cout << "ERROR�����ļ��Ѿ�����" << endl;
		return NULL;
	}
	current_file->f_pos = 0; //��ָ����0
	current_file->f_iNode->open_num = 1; //���ļ�����1 
	FileManager::WorkingDir.push(f_name);
	FileManager::WorkingNo.push(x);
	return current_file;
}

// �ر��ļ����޸�open_num���ͷž����������Close_File(fp);
void File::Close_File(os_file* f) {
	FileManager::WorkingDir.pop();
	FileManager::WorkingNo.pop();
	f->f_iNode->open_num = 0;
	f->f_pos = 0;
	free(f);
}

// ɾ����ͨ�ļ���������os_rm(filename);
int File::os_rm(string f_name) {
	int f_i = fileTOOLS::find_son_pos(f_name); //�ҵ���Ҫɾ�����ļ�Ŀ¼�� 
	if (f_i == DIR_FILE_NUM + 1) {
		cout << "ERROR��������ɾ�����ļ�" << endl;
		return 0;// ʧ�� 
	}
	if (FileManager::iNode_table[(*(FileManager::current_dir + f_i)).iNode_no].i_mode == 0) {
		cout << "ERROR�������ļ�ΪĿ¼�ļ�" << endl;
		return 0; //Ŀ¼�ļ�������
	}
	return fileTOOLS::Delete_File(f_i); //ɾ���ļ�
}