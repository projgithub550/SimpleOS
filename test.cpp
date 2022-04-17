#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <stack>
#include <vector>
#include <utility>
#include <iomanip>
#include "file_dir.h"
using namespace std;

bool isExit;
bool debug;
// 全局变量
iNode iNode_table[iNode_NUM];  //iNode table的数组，数组下标对应iNode编号
dir root_dir[MAX_FILE_NUM];   //根目录 数组实现  往下的每个子目录也是dir类型的数组，每一项是一个文件目录项
dir* current_dir;   //保存每次更新analyse_path返回的dir数组，即当前目录的dir数组
stack<string> WorkingDir;   //记录路径名称
stack<unsigned int> WorkingNo;    //记录路径上i节点的标号

string trim(string str) {
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}

bool ls() {
	vector<pair<string, unsigned short>> fileList = DIR::os_ls();
	vector<pair<string, unsigned short>>::iterator it = fileList.begin();
	cout << "  " << left << setw(7) << "TYPE    NAME" << endl;
	while (it != fileList.end()) {
		cout << "  ";
		if ((*it).second == 0)
			cout << left <<  setw(9) << "DIR ：";
		else 
			cout << left << setw(9) << "FILE：";
		cout << it->first << endl;
		it++;
	}
	return true;
}

bool cat(string filename){
	os_file* fp = File::Open_File(filename);
	if (!fp){
		cout << "open file error\n" << endl;
		return false;
	}
	int fileSize = get_filesize(fp);
	char* dst = (char*)malloc(fileSize + 3);
	if (os_readfile(dst, fileSize, fp)){
		dst[fileSize] = 0;
		cout << (char*)dst << endl;
		File::Close_File(fp);
		return true;
	}
	else{
		cout << "fread error" << endl;
		File::Close_File(fp);
		return false;
	}
}


void displayPath(int flag) {
	stack<string> tempdir(WorkingDir);
	vector<string> temppath;
	while (!tempdir.empty()) {
		temppath.push_back(tempdir.top());
		tempdir.pop();
	}
	string path;
	for (auto it = temppath.rbegin(); it != temppath.rend(); it++) {
		path += "/";
		path += *it;
	}
	if (!flag)
		cout << path << " $ ";
	else
		cout << path;
}

bool mkfile(string filename){
	if (!File::Create_File(filename, 1))
		return false;

	os_file* fp = File::Open_File(filename);
	if (!fp)
		return false;

	cout << "Please input the content of file: " << endl;
	string tmp;
	getline(cin, tmp);

	if (os_writefile((char*)tmp.c_str(), tmp.size(), fp)){
		File::Close_File(fp);
		return true;
	}
	else{
		File::Close_File(fp);
		return false;
	}
}


void InputAnalyse(vector<string> args){
	int inPos = 0;

	string command = args[0];
	//exit
	try{
		if (command == "exit"){
			isExit = true;
			cout << "EXIT!" << endl;
		}
		else if (command == "rmfile"){
			if (args.size() > 1)
				if (File::os_rm(args[1]))
					cout << "remove file successfully\n";
				else
					cout << "remove file error\n";
		}
		else if (command == "rmdir"){
			if (args.size() > 1)
				if (DIR::os_rmdir(args[1]))
					cout << "remove dir successfully\n";
				else
					cout << "remove dir error\n";
		}
		else if (command == "cd"){
			if (args.size() > 1)
				if (!DIR::os_cd(args[1]))
					cout << "error filename\n";
		}
		else if (command == "mkdir"){
			if (args.size() > 1)
				if (File::Create_File(args[1], 0))
					cout << "mkdir successfully\n";
				else
					cout << "mkdir error\n";
		}
		else if (command == "ls"){
			ls();
		}
		else if (command == "pwd"){
			displayPath(1);
		}
		else if (command == "cat"){
			if (args.size() > 1)
				if (!cat(args[1]))
					cout << "cat error\n";
		}
		else if (command == "mkfile"){
			if (args.size() > 1)
				if (mkfile(args[1]))
					cout << "mkfile successfully\n";
				else
					cout << "mkfile error\n";
		}
	}
	catch (const std::exception& e){
		cout << "args error\n" << e.what() << endl;
	}
}

void InputCut(string input){
	stringstream stream;
	stream << input;
	string tmp;
	vector<string> argv;
	while (!stream.eof()){
		stream >> tmp;
		argv.push_back(tmp);
	}
	InputAnalyse(argv);
}

void waitForInput(){
	char tmp[256];
	displayPath(0);

	cin.getline(tmp, 256);
	string tmpStr(tmp);
	tmpStr = trim(tmpStr);

	if (tmpStr != "")
		InputCut(tmpStr);
}

int main() {
	/*  首先激活磁盘（包括格式化和初始化）
	/*  磁盘格式化时会建立模拟磁盘文件
	/*  磁盘初始化时会初始化超级块、初始化iNode表、初始化位图、初始化目录表，最后写入磁盘   */
	debug = 0;
	start_disk();
	/*  初始化当前工作环境  */
	WorkingDir.push("root"); // 当前工作目录为root之下
	WorkingNo.push(0);  //  root目录的i节点号为0
	isExit = false;
	while (!isExit)
		waitForInput(); //  开始等待用户输入
	return 0;
}