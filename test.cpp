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

bool debug;		// �ļ��ڲ�����ʱ���ã�debug = 1�������root�������Ѵ��ڵ��ļ������¿�ʼ
// debug = 0,�����root�������ļ�����Ҫɾ��debug,��Ҫɾ��disk::start_disk()�����е��������
				

bool isExit;	//	�ļ��ڲ��������ã�֮�����ɾ����


void FileManager::InitFileSys() {
	/*  ���ȼ�����̣�������ʽ���ͳ�ʼ����
	/*  ���̸�ʽ��ʱ�Ὠ��ģ������ļ�
	/*  ���̳�ʼ��ʱ���ʼ�������顢��ʼ��iNode����ʼ��λͼ����ʼ��Ŀ¼�����д�����   */
	debug = 0;	// debug = 1 ʱÿ��������ʼ������
	disk::start_disk();
	/*  ��ʼ����ǰ��������  */
	WorkingDir.push("root"); // ��ǰ����Ŀ¼Ϊroot֮��
	WorkingNo.push(0);  //  rootĿ¼��i�ڵ��Ϊ0
}

//vector<pair<string, unsigned short>> 
 bool FileManager::ls() {
	//return DIR::os_ls();
	vector<pair<string, unsigned short>> fileList = DIR::os_ls();
	vector<pair<string, unsigned short>>::iterator it = fileList.begin();
	cout << "  " << left << setw(7) << "TYPE    NAME" << endl;
	while (it != fileList.end()) {
		cout << "  ";
		if ((*it).second == 0)
			cout << left <<  setw(9) << "DIR ��";
		else if (it->second == 1)
			cout << left << setw(9) << "TXT ��";
		else if(it->second == 2){
			cout << left << setw(9) << "EXE ��";
		}
		else {
			cout << "FILETYPE ERROR" << it->first << endl;
			return false;
		}
		cout << it->first << endl;
		it++;
	}
	return true;

}

string FileManager::pwd() {	//	��ʾ��ǰ����·��
	return displayPath(1);
}

bool FileManager::cd(string dirname) {//	�ɹ����� 1�����ɹ����� 0
	return DIR::os_cd(dirname);
}

bool FileManager::cat(string filename){	//	��txt�ļ�������ӡ��ʾ
	os_file* fp = File::Open_File(filename);
	if (!fp){
		cout << "open file error\n" << endl;
		return false;
	}
	int fileSize = disk::get_filesize(fp);
	char* dst = (char*)malloc(fileSize + 3);
	if (disk::os_readfile(dst, fileSize, fp)){
		dst[fileSize] = 0;
		cout << (char*)dst << endl;
		File::Close_File(fp);
		return true;
	}
	cout << "fread error" << endl;
	File::Close_File(fp);
	return false;
}

bool FileManager::mkdir(string dirname) {//	�ɹ����� 1�����ɹ����� 0
	if (!File::Create_File(dirname, 0))
		return false;
	else
		return true;
}

bool FileManager::mkfile(string filename, unsigned short filetype) {//	�ɹ����� T�����ɹ����� F
	if (!File::Create_File(filename, filetype))
		return false;

	os_file* fp = File::Open_File(filename);
	if (!fp)
		return false;

	cout << "Please input the content of file: " << endl;
	string tmp;
	getline(cin, tmp);

	if (disk::os_writefile((char*)tmp.c_str(), tmp.size(), fp)) {
		File::Close_File(fp);
		return true;
	}
	else {
		File::Close_File(fp);
		return false;
	}
}

bool FileManager::rmdir(string dirname) {//	�ɹ����� 1�����ɹ����� 0
	return DIR::os_rmdir(dirname);
}

bool FileManager::rmfile(string filename) {//	�ɹ����� 1�����ɹ����� 0
	if (File::os_rm(filename))
		return true;
	else
		return false;
}

int FileManager::openFile(string filename) {
	unsigned long int address = (unsigned long int)File::Open_File(filename);
	int num = NumOfFile.size();
	while (NumOfFile.find(num) != NumOfFile.end())
		num++;
	NumOfFile[num] = address;
	return num;
}

void FileManager::closeFile(int filenum) {
	unsigned long int address = NumOfFile[filenum];
	NumOfFile.erase(filenum);
	File::Close_File((os_file *)address);
}

unsigned short FileManager::getFileType(string filename) {
	vector<pair<string, unsigned short>> fileList = DIR::os_ls();
	vector<pair<string, unsigned short>>::iterator it = fileList.begin();
	unsigned short f_type = 3;	// δ֪����
	while (it != fileList.end()) {
		if (it->first == filename) {
			f_type = it->second;
			return f_type;
		}
		it++;
	}
	return f_type;
}

int FileManager::readfile(int filenum, int size, void* v_buf)//������̵Ķ��ļ��ӿ�
{
	os_file* fp = (os_file*)NumOfFile[filenum];
	return disk::os_readfile(v_buf, size, fp);
}

int FileManager::writefile(int filenum, int size, void* v_buf)//������̵�д�ļ��ӿ�
{
	os_file* fp = (os_file*)NumOfFile[filenum];
	return disk::os_writefile(v_buf, size, fp);

}




/* �ú���Ϊ�ļ��ڲ��������õĺ��� */
string FileManager::trim(string str) {
	str.erase(0, str.find_first_not_of(" "));
	str.erase(str.find_last_not_of(" ") + 1);
	return str;
}
/* �ú���Ϊ�ļ��ڲ��������õĺ��� */
string FileManager::displayPath(int flag) {
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
	if (!flag) {
		cout << path << " $ ";
		return "1";
	}
	else
		return path;
}
/* �ú���Ϊ�ļ��ڲ��������õĺ��� */
void FileManager::InputAnalyse(vector<string> args){
	int inPos = 0;

	string command = args[0];
	try{
		if (command == "get" && args.size() > 1) {
			unsigned short ftype = getFileType(args[1]);
			if (ftype == 0)
				cout << "DIR" << endl;
			else if (ftype == 1)
				cout << "TXT" << endl;
			else if (ftype == 2)
				cout << "EXE" << endl;
			else
				cout << "FILETYPE ERROR" << endl;
		}
		else if (command == "exit"){
			isExit = true;
			cout << "EXIT!" << endl;
		}
		else if (command == "rmfile"){
			if (args.size() > 1)
				if (rmfile(args[1]))
					cout << "remove file successfully\n";
				else
					cout << "remove file error\n";
		}
		else if (command == "rmdir"){
			if (args.size() > 1)
				if (rmdir(args[1]))
					cout << "remove dir successfully\n";
				else
					cout << "remove dir error\n";
		}
		else if (command == "cd"){
			if (args.size() > 1)
				if (cd(args[1]))
					cout << "error filename\n";
		}
		else if (command == "mkdir"){
			if (args.size() > 1)
				if (mkdir(args[1]))
					cout << "mkdir successfully\n";
				else
					cout << "mkdir error\n";
		}
		else if (command == "ls"){
			ls();
		}
		else if (command == "pwd"){
			pwd();
		}
		else if (command == "cat"){
			if (args.size() > 1)
				if (!cat(args[1]))
					cout << "cat error\n";
		}
		else if (command == "mkfile" && args.size() == 1) {
			unsigned short filetype = 0;
			string tmp;
			string filename;
			while (!(filetype == 1 || filetype == 2)) {
				cout << "ѡ����Ҫ�������ļ����ͣ�1.txt 2.exe" << endl;
				getline(cin, tmp);
				filetype = atoi(tmp.c_str());
			}
			cout << "������Ҫ�������ļ����ƣ�";
			getline(cin, filename);
			if (FileManager::mkfile(filename, filetype))
				cout << "mkfile successfully\n";
			else
				cout << "mkfile error\n";
		}
		else {
			cout << "COMMAND ERROR" << endl;
		}
	}
	catch (const std::exception& e){
		cout << "args error\n" << e.what() << endl;
	}
}

/* �ú���Ϊ�ļ��ڲ��������õĺ��� */
void FileManager::InputCut(string input){
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

/* �ú���Ϊ�ļ��ڲ��������õĺ��� */
void FileManager::waitForInput(){
	char tmp[256];
	displayPath(0);

	cin.getline(tmp, 256);
	string tmpStr(tmp);
	tmpStr = trim(tmpStr);

	if (tmpStr != "")
		InputCut(tmpStr);
}

iNode FileManager::iNode_table[iNode_NUM];  //iNode table�����飬�����±��ӦiNode���
dir FileManager::root_dir[MAX_FILE_NUM];    //��Ŀ¼ ����ʵ��  ���µ�ÿ����Ŀ¼Ҳ��dir���͵����飬ÿһ����һ���ļ�Ŀ¼��
dir* FileManager::current_dir;				//����ÿ�θ���analyse_path���ص�dir���飬����ǰĿ¼��dir����
stack<string> FileManager::WorkingDir;		//��¼·������
stack<unsigned int> FileManager::WorkingNo; //��¼·����i�ڵ�ı��
map<int, unsigned long int> FileManager::NumOfFile;

/* main����Ϊ�ļ��ڲ��������õĺ��� */
int main() {
	/*  ��ʼ����ǰ��������  */
	/*  ���ȼ�����̣�������ʽ���ͳ�ʼ����
	/*  ���̸�ʽ��ʱ�Ὠ��ģ������ļ�
	/*  ���̳�ʼ��ʱ���ʼ�������顢��ʼ��iNode����ʼ��λͼ����ʼ��Ŀ¼�����д�����   */
	FileManager::InitFileSys();
	isExit = false;
	while (!isExit)
		FileManager::waitForInput(); //  ��ʼ�ȴ��û�����
	return 0;
}