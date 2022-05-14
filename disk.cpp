#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "file_dir.h"
using namespace std;

//实现建立模拟磁盘文件，申请的足够大的内存并写入文件，返回true或false表示操作成功与否
bool disk::format_disk()
{
	FILE* diskp;//磁盘文件指针
	char* buf = (char*)malloc(MAX_BLOCK_NUM * BLOCK_SIZE);//申请足够的磁盘空间
	if (diskp = fopen(DISK, "wb")) //只写打开或新建一个二进制文件；只允许写数据
	{
		fwrite(buf, MAX_BLOCK_NUM * BLOCK_SIZE, 1, diskp); //将申请的足够大的内存写入文件
		fclose(diskp);
		return true;
	}
	else
		return false;

}
//磁盘初始化，需要依次完成超级块的初始化、inode的初始化、位图bitmap的初始化、根目录root_dir的初始化，完成各项初始化后写入磁盘并保存，返回true或false表示操作成功与否。
bool disk::init_disk()
{
	//超级块的初始化
	super_block* sb = (super_block*)malloc(sizeof(super_block));//先动态申请足够的内存，写入对应数据后再写入磁盘文件，然后释放
	sb->sb_inodenum = MAX_FILE_NUM;
	sb->sb_blocknum = MAX_BLOCK_NUM;
	sb->sb_filemaxsize = MAX_FILE_SIZE;

	//inode的初始化
	iNode* inode_table = (iNode*)malloc(iNode_NUM * sizeof(iNode));//建立inode_table
	for (int i = 0; i < iNode_NUM; i++)
		File::init_iNode(inode_table + i);

	//bitmap的初始化
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);
	int border = DATA_START - 1; //从data区以后的磁盘块可用，data区前的磁盘块不可用
	for (int i = 0; i < MAX_BLOCK_NUM; i++) //对bitmap进行初始化
	{
		if (i <= border)
			*(bitmap + i) = 1;
		else
			*(bitmap + i) = 0;
	}

	//root_dir根目录的初始化
	dir root[DIR_FILE_NUM];
	DIR::init_dir(root);
	iNode root_iNode = File::Fill_in_iNode(0);//根目录对应节点
	for (int i = 0; i < FBLK_NUM; i++)
	{
		root_iNode.block_address[i] = border + 1 + i;//占用data区前四个块
		*(bitmap + border + i + 1) = 1;//修改bitmap
	}
	*inode_table = root_iNode;//放入inode表

	//写入磁盘
	FILE* diskp = fopen(DISK, "rb+"); //读写打开一个二进制文件，只允许读写数据
	fwrite(sb, sizeof(super_block), 1, diskp); //写入超级块内容
	fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);//定位到流中指定的位置,SEEK_SET 0 文件开头
	fwrite(inode_table, sizeof(iNode) * iNode_NUM, 1, diskp); //写入inode_table
	fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET);//定位到bitmap的位置
	fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //写入bitmap
	for (int i = 0; i < FBLK_NUM; i++) //root_dir写入磁盘
	{
		fseek(diskp, root_iNode.block_address[i] * BLOCK_SIZE, SEEK_SET);//定位到root_dir的位置
		fwrite((char*)root + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
	}
	free(sb);
	free(inode_table);
	free(bitmap);
	fclose(diskp);
	return true;
}

//实现磁盘格式化和初始化后，读入系统所需的各个参数，读入iNode_table，root_dir，返回true或false表示操作成功与否。
bool disk::start_disk()
{
	dir* temp_root = FileManager::root_dir;//临时根目录
	while (1)
	{
		FILE* diskp = fopen(DISK, "rb");
		if (debug) {//	调试专用
			format_disk();
			init_disk();
		}
		if (diskp)
		{
			fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);//定位到inode表位置
			fread(&FileManager::iNode_table, iNode_NUM * sizeof(iNode), 1, diskp);//读入全局变量iNode_table中
			for (int i = 0; i < FBLK_NUM; i++) //读入root_dir根目录
			{
				fseek(diskp, FileManager::iNode_table[0].block_address[i] * BLOCK_SIZE, SEEK_SET);
				fread((char*)temp_root + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
			}
			FileManager::current_dir = temp_root;
			int j = 0;
			for (int j = 0; j < iNode_NUM; j++)
			{
				if (FileManager::iNode_table[j].i_mode != 2)
					FileManager::iNode_table[j].open_num = 0;
			}
			fclose(diskp);
			return true;
		}
		else
		{
			format_disk();
			init_disk();
		}
	}
}

int disk::first_free()//查找第一个空闲块号，修改其bitmap为1表示被占用，将磁盘中的位图读出，查找空闲块号，修改信息后写写回磁盘，返回该块号。
{
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);//临时存放位图
	FILE* diskp = fopen(DISK, "rb+");//读写打开一个二进制文件，只允许读写数据。
	fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET);//定位到位图位置
	fread(bitmap, MAX_BLOCK_NUM, 1, diskp);
	for (int i = 0; i < MAX_BLOCK_NUM; i++) //查找第一个空闲块号
	{
		if (*(bitmap + i) == 0)
		{
			*(bitmap + i) = 1; //修改bitmap表示该块被占用
			fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET); //文件指针定位到bitmap起点
			fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //将bitmap写回磁盘
			fclose(diskp);
			free(bitmap);
			return i;//返回块号
		}
	}
	fclose(diskp);
	free(bitmap);
	return -1;//没有空闲块
}

int disk::release_block(int block)//相当于磁盘块的格式化，将磁盘中的位图读出，检查该块是否可释放，若可释放则将对应位图置0，将位图写回磁盘，格式化该磁盘块，相当于释放该块。
{
	if (block <= DATA_START)
	{
		return 0; //释放不可用区的磁盘块，报错
	}
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);//临时存放位图
	char* block_format = (char*)malloc(BLOCK_SIZE); //用于磁盘块的格式化

	FILE* diskp = fopen(DISK, "rb+");//读写打开一个二进制文件，只允许读写数据。
	fseek(diskp, BITMAP_START* BLOCK_SIZE, SEEK_SET);//定位到位图位置
	fread(bitmap, MAX_BLOCK_NUM, 1, diskp); //读入bitmap

	*(bitmap + block) = 0; //bitmap对应块置0
	fseek(diskp, BLOCK_SIZE* BITMAP_START, SEEK_SET); //文件指针定位到bitmap起点
	fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //将bitmap写回磁盘
	fseek(diskp, block* BLOCK_SIZE, SEEK_SET); //将指针移动到要格式化的磁盘块位置
	fwrite(block_format, BLOCK_SIZE, 1, diskp); //磁盘块格式化，相当于释放
	free(bitmap);
	free(block_format);
	fclose(diskp);
	return 1;
}

//获取文件大小
int disk::get_filesize(os_file* fp)
{
	return fp->f_iNode->i_size;
}

//文件读函数
int disk::os_readfile(void* v_buf, int size, os_file* fp)//传入参数：数据缓冲区，待读出的数据大小，该文件的句柄。根据文件句柄和数据大小，计算出起始块号，读出的块数，最后块的偏移量，将这些数据写到缓冲区。
{
	if (fp->f_pos + size > fp->f_iNode->i_size) //判断要读的数据是否超出文件界限
		return 0;

	int start_block, block_num, offset;//分别记录数据所在的 起始块号 连续占用块数 最后占用块的偏移量
	char* buf = (char*)v_buf;
	start_block = fp->f_pos / BLOCK_SIZE; //从文件的第几个block开始读
	block_num = (fp->f_pos + size) / BLOCK_SIZE - start_block + 1; //连续读几个block
	offset = (fp->f_pos + size) % BLOCK_SIZE;//最后一个block的off_set偏移量
	if (offset == 0 && block_num > 1)
		offset = BLOCK_SIZE; //偏移为0说明要读入一整块

	FILE* diskp;
	diskp = fopen(DISK, "rb");
	if (block_num == 1)
	{
		fseek(diskp, fp->f_iNode->block_address[0] * BLOCK_SIZE + fp->f_pos, SEEK_SET);//定位至文件起始地址加读指针位置
		fread(buf, size, 1, diskp);
	}
	else
	{
		int read_bytes = 0; //累计已读入字节数
		int cur_bytes = 0; //当前块内需要读取的字节数
		for (int block = start_block; block < block_num + start_block; block++) //遍历连续的块
		{
			if (block == start_block) //第一块
				cur_bytes = BLOCK_SIZE - fp->f_pos % BLOCK_SIZE;//第一块内需读入的字节数

			else if (block == start_block + block_num - 1) //最后一块
			{
				cur_bytes = offset;//最后一块需读入的字节数
			}
			else
			{
				cur_bytes = BLOCK_SIZE;//需要读出全部数据的中间块
			}
			rewind(diskp);//把当前的读写位置回到文件开始
			fseek(diskp, fp->f_iNode->block_address[block] * BLOCK_SIZE + fp->f_pos + read_bytes, SEEK_SET);//定位
			fread(buf + read_bytes, cur_bytes, 1, diskp);//读出到缓冲区
			read_bytes += cur_bytes;
		}
	}
	fclose(diskp);
	v_buf = (void*)buf;
	return 1;
}

//文件写函数
int disk::os_writefile(void* v_buf, int size, os_file* fp)//传入参数：数据缓冲区，待写入的数据大小，该文件的句柄。据文件句柄和数据大小，计算出起始块号，读出的块数，最后块的偏移量，分配磁盘块时调用寻找空闲块号函数为其分配空闲块。
{
	//计算要写的块数及偏移量
	int first_block, block_num, offset, total_block;//分别记录数据所在的 起始块号 连续占用块数 最后占用块的偏移量
	char* buf = (char*)malloc(size);
	memcpy(buf, v_buf, size); //拷贝

	total_block = (fp->f_pos + size) / BLOCK_SIZE + 1; //写入文件后文件的总block数
	if (total_block > FBLK_NUM)  //写入数据块数超出文件的最大块数
		return 0;

	first_block = fp->f_pos / BLOCK_SIZE; //从文件的第几个block开始写
	block_num = (fp->f_pos + size) / BLOCK_SIZE - first_block + 1; //连续写几个block
	offset = (fp->f_pos + size) % BLOCK_SIZE;//最后一个block的offset

	if (offset == 0 && block_num > 1)
		offset = BLOCK_SIZE; //偏移为0且写数据块大于一 说明最后一块要写入一整块

	//检查是否需要将空闲块分配给文件
	for (int i = 0; i < total_block; i++)
	{
		if (fp->f_iNode->block_address[i] > MAX_BLOCK_NUM) //说明文件的第i块尚未被分配
		{
			fp->f_iNode->block_address[i] = disk::first_free();
			if (fp->f_iNode->block_address[i] == -1)
			{
				fp->f_iNode->block_address[i] = MAX_BLOCK_NUM + 1;
				return 0; //没有空闲块可以分配
			}
		}
	}

	FILE* diskp;
	diskp = fopen(DISK, "rb+"); //打开磁盘文件
	if (block_num == 1)
	{
		rewind(diskp);
		fseek(diskp, fp->f_pos + fp->f_iNode->block_address[first_block] * BLOCK_SIZE, SEEK_SET);//定位文件指针至待写入处
		fwrite(buf, size, 1, diskp);
	}
	else
	{
		int write_bytes = 0; //已写入的字节数
		int cur_bytes = 0; //当前块内需要写入的字节数
		for (int block = first_block; block < (first_block + block_num); block++)
		{
			if (block == first_block) //第一块
				cur_bytes = BLOCK_SIZE - fp->f_pos % BLOCK_SIZE;
			else if (block == first_block + block_num - 1)  //最后一块
				cur_bytes = offset;
			else //需要写入全部数据的中间块
				cur_bytes = BLOCK_SIZE;
			rewind(diskp);
			fseek(diskp, fp->f_pos + fp->f_iNode->block_address[block] * BLOCK_SIZE + write_bytes, SEEK_SET);
			fwrite(buf + write_bytes, cur_bytes, 1, diskp);
			write_bytes += cur_bytes;
		}
	}
	fp->f_pos += size; //更新文件指针
	fp->f_iNode->i_size += size; //更新文件大小
	fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);
	fwrite(FileManager::iNode_table, sizeof(iNode) * iNode_NUM, 1, diskp); //iNode_table更新
	fclose(diskp); //关闭磁盘读写
	free(buf);
	return 1;
}

//面向文件系统的读目录接口
void disk::get_dir(void* dir_buf, iNode* f_inode)
{
	FILE* diskp = fopen(DISK, "rb");
	if (diskp)
	{
		char* buf = (char*)dir_buf;
		for (int i = 0; i < FBLK_NUM; i++)
		{
			fseek(diskp, f_inode->block_address[i] * BLOCK_SIZE, SEEK_SET); //定位文件指针
			fread(buf + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
		}
	}
}

//磁盘块写入缓冲区
bool disk::write_block(long block, char* buf)
{
    FILE* diskp = fopen(DISK, "rb+");
    if (diskp) {
        fseek(diskp, (block * BLOCK_SIZE), SEEK_SET); //定位到对应块的位置
        fwrite(buf, BLOCK_SIZE, 1, diskp);
        fclose(diskp);
        return true;
    }
}

//磁盘块读入缓冲区
bool disk::read_block(long block, char* buf)
{
    FILE* diskp = fopen(DISK, "r");
    if (diskp) {
        fseek(diskp, (block * BLOCK_SIZE), SEEK_SET); //定位到对应块的位置
        fread(buf, BLOCK_SIZE, 1, diskp);
        fclose(diskp);
        return true;
    }
}