#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <utility>
#include "file_dir.h"
using namespace std;

// dir目录初始化函数————init_dir(root_dir);初始化根目录
void DIR::init_dir(dir blankdir[]) {
    for (int i = 0; i < DIR_FILE_NUM; i++) {
        blankdir[i].file_name = "#";//初始文件名为#
        blankdir[i].iNode_no = iNode_NUM + 1;
    }
}

// 设置默认参数是当前目录————os_ls();列出当前目录下所有文件名称
vector<pair<string, unsigned short>> DIR::os_ls() {
    vector<pair<string, unsigned short>> files;
    for (int i = 0; i < DIR_FILE_NUM && FileManager::current_dir[i].file_name != "#"; i++) {
        pair<string, unsigned short> f = make_pair(FileManager::current_dir[i].file_name, FileManager::iNode_table[FileManager::current_dir[i].iNode_no].i_mode);
        files.push_back(f);
    }
    return files;
}

// 切换目录————os_cd(newpath);从当前目录切换到newpath下
// newpath == #绝对路径 #相对路径 #..(上层目录)
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
            cout << "ERROR——找不到正确的绝对路径" << endl;
        else
            cout << "ERROR——找不到正确的相对路径" << endl;
        return 0;
    }
    else {
        FileManager::current_dir = temp;
        return 1;
    }
}

// 删除空目录————os_rmdir(sondir);
bool DIR::os_rmdir(string dir_name) {
    int f_i = fileTOOLS::find_son_pos(dir_name);//找到要删除的目录在其父目录的标号 
    if (f_i == DIR_FILE_NUM + 1) {
        cout << "ERROR——找不到正确的文件或目录" << endl;
        return 0;
    }
    unsigned int find_iNode = (*(FileManager::current_dir + f_i)).iNode_no;
    if (FileManager::iNode_table[find_iNode].i_mode == 1) {
        cout << "ERROR——该文件为普通文件" << endl;
        return 0; //普通文件，错误
    }
    dir* f_dir = (dir*)malloc(sizeof(dir) * DIR_FILE_NUM);
    disk::get_dir(f_dir, &FileManager::iNode_table[find_iNode]);    //####磁盘部分
    int i;
    for (i = 0; i < DIR_FILE_NUM && f_dir[i].iNode_no == iNode_NUM + 1; i++) {}
    if (i == DIR_FILE_NUM) {//此目录下无文件
        free(f_dir);
        return fileTOOLS::Delete_File(f_i);
    }
    else {
        cout << "ERROR——该目录为非空目录" << endl;
        free(f_dir);
        return 0;
    }
}
