#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <utility>
#include "file_dir.h"
using namespace std;

// dirĿ¼��ʼ��������������init_dir(root_dir);��ʼ����Ŀ¼
void DIR::init_dir(dir blankdir[]) {
    for (int i = 0; i < DIR_FILE_NUM; i++) {
        blankdir[i].file_name = "#";//��ʼ�ļ���Ϊ#
        blankdir[i].iNode_no = iNode_NUM + 1;
    }
}

// ����Ĭ�ϲ����ǵ�ǰĿ¼��������os_ls();�г���ǰĿ¼�������ļ�����
vector<pair<string, unsigned short>> DIR::os_ls() {
    vector<pair<string, unsigned short>> files;
    for (int i = 0; i < DIR_FILE_NUM && FileManager::current_dir[i].file_name != "#"; i++) {
        pair<string, unsigned short> f = make_pair(FileManager::current_dir[i].file_name, FileManager::iNode_table[FileManager::current_dir[i].iNode_no].i_mode);
        files.push_back(f);
    }
    return files;
}

// �л�Ŀ¼��������os_cd(newpath);�ӵ�ǰĿ¼�л���newpath��
// newpath == #����·�� #���·�� #..(�ϲ�Ŀ¼)
bool DIR::os_cd(string newpath) {
    int tmp_no = -1;
    if (newpath == "..") {
        if (FileManager::WorkingDir.size() > 1) {
            FileManager::WorkingNo.pop();
            disk::get_dir(FileManager::current_dir, &(FileManager::iNode_table[FileManager::WorkingNo.top()]));
            FileManager::WorkingDir.pop();
        }
        return 1;
    }
    dir* temp = fileTOOLS::analyse_Path(newpath);
    if (temp == NULL) {
        if (newpath[0] == '/')
            cout << "ERROR�����Ҳ�����ȷ�ľ���·��" << endl;
        else
            cout << "ERROR�����Ҳ�����ȷ�����·��" << endl;
        return 0;
    }
    else {
        FileManager::current_dir = temp;
        return 1;
    }
}

// ɾ����Ŀ¼��������os_rmdir(sondir);
bool DIR::os_rmdir(string dir_name) {
    int f_i = fileTOOLS::find_son_pos(dir_name);//�ҵ�Ҫɾ����Ŀ¼���丸Ŀ¼�ı�� 
    if (f_i == DIR_FILE_NUM + 1) {
        cout << "ERROR�����Ҳ�����ȷ���ļ���Ŀ¼" << endl;
        return 0;
    }
    unsigned int find_iNode = (*(FileManager::current_dir + f_i)).iNode_no;
    if (FileManager::iNode_table[find_iNode].i_mode == 1) {
        cout << "ERROR�������ļ�Ϊ��ͨ�ļ�" << endl;
        return 0; //��ͨ�ļ�������
    }
    dir* f_dir = (dir*)malloc(sizeof(dir) * DIR_FILE_NUM);
    disk::get_dir(f_dir, &FileManager::iNode_table[find_iNode]);    //####���̲���
    int i;
    for (i = 0; i < DIR_FILE_NUM && f_dir[i].iNode_no == iNode_NUM + 1; i++) {}
    if (i == DIR_FILE_NUM) {//��Ŀ¼�����ļ�
        free(f_dir);
        return fileTOOLS::Delete_File(f_i);
    }
    else {
        cout << "ERROR������Ŀ¼Ϊ�ǿ�Ŀ¼" << endl;
        free(f_dir);
        return 0;
    }
}
