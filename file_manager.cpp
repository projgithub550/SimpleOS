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

bool debug;		// 文件内部测试时所用，debug = 1，则清空root下所有已存在的文件再重新开始
// debug = 0,则不清空root下所有文件，若要删除debug,则要删除disk::start_disk()函数中的条件语句


bool isExit;	//	文件内部测试所用，之后可以删掉。


void FileManager::InitFileSys() {
    /*  首先激活磁盘（包括格式化和初始化）
    *  磁盘格式化时会建立模拟磁盘文件
    *  磁盘初始化时会初始化超级块、初始化iNode表、初始化位图、初始化目录表，最后写入磁盘   */
    debug = 1;
    disk::start_disk();
    /*  初始化当前工作环境  */
    WorkingDir.push("root"); // 当前工作目录为root之下
    WorkingNo.push(0);  //  root目录的i节点号为0
}

////vector<pair<string, unsigned short>>
// bool FileManager::ls() {
//    //return DIR::os_ls();
//    vector<pair<string, unsigned short>> fileList = DIR::os_ls();
//    vector<pair<string, unsigned short>>::iterator it = fileList.begin();
//    qDebug() << "  " << left << setw(7) << "TYPE    NAME" << Qt::endl;
//    while (it != fileList.end()) {
//        qDebug() << "  ";
//        if ((*it).second == 0)
//            qDebug() << left <<  setw(9) << "DIR ：";
//        else if (it->second == 1)
//            qDebug() << left << setw(9) << "TXT ：";
//        else if(it->second == 2){
//            qDebug() << left << setw(9) << "EXE ：";
//        }
//        else {
//            qDebug() << "FILETYPE ERROR" << it->first << Qt::endl;
//            return false;
//        }
//        qDebug() << it->first << Qt::endl;
//        it++;
//    }
//    return true;

//}

string FileManager::pwd() {	//	显示当前所在路径
    return displayPath(1);
}

bool FileManager::cd(string dirname) {//	成功返回 1，不成功返回 0
    return DIR::os_cd(dirname);
}

string FileManager::cat(string filename){	//	打开txt文件，并打印显示
    os_file* fp = File::Open_File(filename);
    if (!fp){
        qDebug() << "open file error\n" << Qt::endl;
        return "open file error";
    }
    int fileSize = disk::get_filesize(fp);
    char* dst = (char*)malloc(fileSize + 3);
    if (disk::os_readfile(dst, fileSize, fp)){
        dst[fileSize] = 0;
        File::Close_File(fp);
        return (string)dst;
    }
    qDebug() << "fread error" << Qt::endl;
    File::Close_File(fp);
    return "read file error";
}

// 创建目录
bool FileManager::mkdir(string dirname) {//	成功返回 1，不成功返回 0
    if (!File::Create_File(dirname, 0))
        return false;
    else
        return true;
}

// 创建txt文件
bool FileManager::mkfile(string filename, string content,unsigned short f_type) {//	成功返回 T，不成功返回 F
    if (!File::Create_File(filename, f_type))
        return false;
   // qDebug() << "1111";
    os_file* fp = File::Open_File(filename);


    if (!fp)
        return false;
    // qDebug() << "222";
    if (disk::os_writefile((char *)content.c_str(), content.size()+1, fp))
    {
     //   qDebug() << "333";
        File::Close_File(fp);
        return true;
    }
    else {
        File::Close_File(fp);
     //   qDebug() << "333";
        return false;
    }
}

bool FileManager::rmdir(string dirname) {//	成功返回 1，不成功返回 0
    return DIR::os_rmdir(dirname);
}

bool FileManager::rmfile(string filename) {//	成功返回 1，不成功返回 0
    if (File::os_rm(filename))
        return true;
    else
        return false;
}

int FileManager::openFile(string filename) {
    os_file* addr = File::Open_File(filename);
    unsigned long int address = (long long)(addr);
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

bool FileManager::writeBlock(long block, char* buf)//内存写入磁盘块
{
    return disk::write_block(block,buf);
}
bool FileManager::readBlock(long block, char* buf)//磁盘块写入内存
{
    return disk::read_block(block,buf);
}

unsigned short FileManager::getFileType(string filename) {
    vector<pair<string, unsigned short>> fileList = DIR::os_ls();
    vector<pair<string, unsigned short>>::iterator it = fileList.begin();
    unsigned short f_type = 3;	// 未知类型
    while (it != fileList.end()) {
        if (it->first == filename) {
            f_type = it->second;
            return f_type;
        }
        it++;
    }
    return f_type;
}

int FileManager::readFile(int filenum, int size, void* v_buf)//面向进程的读文件接口
{
    os_file* fp = (os_file*)NumOfFile[filenum];
    return disk::os_readfile(v_buf, size, fp);
}

int FileManager::writeFile(int filenum, int size, void* v_buf)//面向进程的写文件接口
{
    os_file* fp = (os_file*)NumOfFile[filenum];
    return disk::os_writefile(v_buf, size+1, fp);

}

/* 该函数为文件内部测试所用的函数 */
string FileManager::trim(string str) {
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

/* 该函数为文件内部测试所用的函数 */
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
        qDebug() << QString::fromStdString(path) << " $ ";
        return "1";
    }
    else
        return path;
}

/* 该函数为文件内部测试所用的函数 */
//void FileManager::InputAnalyse(vector<string> args){
//    int inPos = 0;

//    string command = args[0];
//    try{
//        if (command == "get" && args.size() > 1) {
//            unsigned short ftype = getFileType(args[1]);
//            if (ftype == 0)
//                cout << "DIR" << endl;
//            else if (ftype == 1)
//                cout << "TXT" << endl;
//            else if (ftype == 2)
//                cout << "EXE" << endl;
//            else
//                cout << "FILETYPE ERROR" << endl;
//        }
//        else if (command == "exit"){
//            isExit = true;
//            cout << "EXIT!" << endl;
//        }
//        else if (command == "rmfile"){
//            if (args.size() > 1)
//                if (rmfile(args[1]))
//                    cout << "remove file successfully\n";
//                else
//                    cout << "remove file error\n";
//        }
//        else if (command == "rmdir"){
//            if (args.size() > 1)
//                if (rmdir(args[1]))
//                    cout << "remove dir successfully\n";
//                else
//                    cout << "remove dir error\n";
//        }
//        else if (command == "cd"){
//            if (args.size() > 1)
//                if (cd(args[1]))
//                    cout << "error filename\n";
//        }
//        else if (command == "mkdir"){
//            if (args.size() > 1)
//                if (mkdir(args[1]))
//                    cout << "mkdir successfully\n";
//                else
//                    cout << "mkdir error\n";
//        }
//        else if (command == "ls"){
//            ls();
//        }
//        else if (command == "pwd"){
//            pwd();
//        }
//        else if (command == "cat"){
//            if (args.size() > 1)
//                if (cat(args[1]).find_last_of("error") == string::npos)
//                    cout << "cat error\n";
//        }
//        else if (command == "mkfile" && args.size() == 1) {
//            unsigned short filetype = 0;
//            string tmp;
//            string filename;
//            while (!(filetype == 1 || filetype == 2)) {
//                cout << "选择你要创建的文件类型：1.txt 2.exe" << endl;
//                getline(cin, tmp);
//                filetype = atoi(tmp.c_str());
//            }
//            cout << "输入你要创建的文件名称：";
//            getline(cin, filename);
//            if (FileManager::mkfile(filename, filetype))
//                cout << "mkfile successfully\n";
//            else
//                cout << "mkfile error\n";
//        }
//        else {
//            cout << "COMMAND ERROR" << endl;
//        }
//    }
//    catch (const std::exception& e){
//        cout << "args error\n" << e.what() << endl;
//    }
//}

///* 该函数为文件内部测试所用的函数 */
//void FileManager::InputCut(string input){
//    stringstream stream;
//    stream << input;
//    string tmp;
//    vector<string> argv;
//    while (!stream.eof()){
//        stream >> tmp;
//        argv.push_back(tmp);
//    }
//    InputAnalyse(argv);
//}

///* 该函数为文件内部测试所用的函数 */
//void FileManager::waitForInput(){
//    char tmp[256];
//    displayPath(0);

//    cin.getline(tmp, 256);
//    string tmpStr(tmp);
//    tmpStr = trim(tmpStr);

//    if (tmpStr != "")
//        InputCut(tmpStr);
//}

iNode FileManager::iNode_table[iNode_NUM];  //iNode table的数组，数组下标对应iNode编号
dir FileManager::root_dir[DIR_FILE_NUM];    //根目录 数组实现  往下的每个子目录也是dir类型的数组，每一项是一个文件目录项
dir* FileManager::current_dir;				//保存每次更新analyse_path返回的dir数组，即当前目录的dir数组
stack<string> FileManager::WorkingDir;		//记录路径名称
stack<unsigned int> FileManager::WorkingNo; //记录路径上i节点的标号
map<int, unsigned long int> FileManager::NumOfFile;
