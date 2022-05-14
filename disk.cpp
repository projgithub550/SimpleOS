#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "file_dir.h"
using namespace std;

//ʵ�ֽ���ģ������ļ���������㹻����ڴ沢д���ļ�������true��false��ʾ�����ɹ����
bool disk::format_disk()
{
	FILE* diskp;//�����ļ�ָ��
	char* buf = (char*)malloc(MAX_BLOCK_NUM * BLOCK_SIZE);//�����㹻�Ĵ��̿ռ�
	if (diskp = fopen(DISK, "wb")) //ֻд�򿪻��½�һ���������ļ���ֻ����д����
	{
		fwrite(buf, MAX_BLOCK_NUM * BLOCK_SIZE, 1, diskp); //��������㹻����ڴ�д���ļ�
		fclose(diskp);
		return true;
	}
	else
		return false;

}
//���̳�ʼ������Ҫ������ɳ�����ĳ�ʼ����inode�ĳ�ʼ����λͼbitmap�ĳ�ʼ������Ŀ¼root_dir�ĳ�ʼ������ɸ����ʼ����д����̲����棬����true��false��ʾ�����ɹ����
bool disk::init_disk()
{
	//������ĳ�ʼ��
	super_block* sb = (super_block*)malloc(sizeof(super_block));//�ȶ�̬�����㹻���ڴ棬д���Ӧ���ݺ���д������ļ���Ȼ���ͷ�
	sb->sb_inodenum = MAX_FILE_NUM;
	sb->sb_blocknum = MAX_BLOCK_NUM;
	sb->sb_filemaxsize = MAX_FILE_SIZE;

	//inode�ĳ�ʼ��
	iNode* inode_table = (iNode*)malloc(iNode_NUM * sizeof(iNode));//����inode_table
	for (int i = 0; i < iNode_NUM; i++)
		File::init_iNode(inode_table + i);

	//bitmap�ĳ�ʼ��
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);
	int border = DATA_START - 1; //��data���Ժ�Ĵ��̿���ã�data��ǰ�Ĵ��̿鲻����
	for (int i = 0; i < MAX_BLOCK_NUM; i++) //��bitmap���г�ʼ��
	{
		if (i <= border)
			*(bitmap + i) = 1;
		else
			*(bitmap + i) = 0;
	}

	//root_dir��Ŀ¼�ĳ�ʼ��
	dir root[DIR_FILE_NUM];
	DIR::init_dir(root);
	iNode root_iNode = File::Fill_in_iNode(0);//��Ŀ¼��Ӧ�ڵ�
	for (int i = 0; i < FBLK_NUM; i++)
	{
		root_iNode.block_address[i] = border + 1 + i;//ռ��data��ǰ�ĸ���
		*(bitmap + border + i + 1) = 1;//�޸�bitmap
	}
	*inode_table = root_iNode;//����inode��

	//д�����
	FILE* diskp = fopen(DISK, "rb+"); //��д��һ���������ļ���ֻ�����д����
	fwrite(sb, sizeof(super_block), 1, diskp); //д�볬��������
	fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);//��λ������ָ����λ��,SEEK_SET 0 �ļ���ͷ
	fwrite(inode_table, sizeof(iNode) * iNode_NUM, 1, diskp); //д��inode_table
	fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET);//��λ��bitmap��λ��
	fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //д��bitmap
	for (int i = 0; i < FBLK_NUM; i++) //root_dirд�����
	{
		fseek(diskp, root_iNode.block_address[i] * BLOCK_SIZE, SEEK_SET);//��λ��root_dir��λ��
		fwrite((char*)root + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
	}
	free(sb);
	free(inode_table);
	free(bitmap);
	fclose(diskp);
	return true;
}

//ʵ�ִ��̸�ʽ���ͳ�ʼ���󣬶���ϵͳ����ĸ�������������iNode_table��root_dir������true��false��ʾ�����ɹ����
bool disk::start_disk()
{
	dir* temp_root = FileManager::root_dir;//��ʱ��Ŀ¼
	while (1)
	{
		FILE* diskp = fopen(DISK, "rb");
		if (debug) {//	����ר��
			format_disk();
			init_disk();
		}
		if (diskp)
		{
			fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);//��λ��inode��λ��
			fread(&FileManager::iNode_table, iNode_NUM * sizeof(iNode), 1, diskp);//����ȫ�ֱ���iNode_table��
			for (int i = 0; i < FBLK_NUM; i++) //����root_dir��Ŀ¼
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

int disk::first_free()//���ҵ�һ�����п�ţ��޸���bitmapΪ1��ʾ��ռ�ã��������е�λͼ���������ҿ��п�ţ��޸���Ϣ��дд�ش��̣����ظÿ�š�
{
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);//��ʱ���λͼ
	FILE* diskp = fopen(DISK, "rb+");//��д��һ���������ļ���ֻ�����д���ݡ�
	fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET);//��λ��λͼλ��
	fread(bitmap, MAX_BLOCK_NUM, 1, diskp);
	for (int i = 0; i < MAX_BLOCK_NUM; i++) //���ҵ�һ�����п��
	{
		if (*(bitmap + i) == 0)
		{
			*(bitmap + i) = 1; //�޸�bitmap��ʾ�ÿ鱻ռ��
			fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET); //�ļ�ָ�붨λ��bitmap���
			fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //��bitmapд�ش���
			fclose(diskp);
			free(bitmap);
			return i;//���ؿ��
		}
	}
	fclose(diskp);
	free(bitmap);
	return -1;//û�п��п�
}

int disk::release_block(int block)//�൱�ڴ��̿�ĸ�ʽ�����������е�λͼ���������ÿ��Ƿ���ͷţ������ͷ��򽫶�Ӧλͼ��0����λͼд�ش��̣���ʽ���ô��̿飬�൱���ͷŸÿ顣
{
	if (block <= DATA_START)
	{
		return 0; //�ͷŲ��������Ĵ��̿飬����
	}
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);//��ʱ���λͼ
	char* block_format = (char*)malloc(BLOCK_SIZE); //���ڴ��̿�ĸ�ʽ��

	FILE* diskp = fopen(DISK, "rb+");//��д��һ���������ļ���ֻ�����д���ݡ�
	fseek(diskp, BITMAP_START* BLOCK_SIZE, SEEK_SET);//��λ��λͼλ��
	fread(bitmap, MAX_BLOCK_NUM, 1, diskp); //����bitmap

	*(bitmap + block) = 0; //bitmap��Ӧ����0
	fseek(diskp, BLOCK_SIZE* BITMAP_START, SEEK_SET); //�ļ�ָ�붨λ��bitmap���
	fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //��bitmapд�ش���
	fseek(diskp, block* BLOCK_SIZE, SEEK_SET); //��ָ���ƶ���Ҫ��ʽ���Ĵ��̿�λ��
	fwrite(block_format, BLOCK_SIZE, 1, diskp); //���̿��ʽ�����൱���ͷ�
	free(bitmap);
	free(block_format);
	fclose(diskp);
	return 1;
}

//��ȡ�ļ���С
int disk::get_filesize(os_file* fp)
{
	return fp->f_iNode->i_size;
}

//�ļ�������
int disk::os_readfile(void* v_buf, int size, os_file* fp)//������������ݻ������������������ݴ�С�����ļ��ľ���������ļ���������ݴ�С���������ʼ��ţ������Ŀ����������ƫ����������Щ����д����������
{
	if (fp->f_pos + size > fp->f_iNode->i_size) //�ж�Ҫ���������Ƿ񳬳��ļ�����
		return 0;

	int start_block, block_num, offset;//�ֱ��¼�������ڵ� ��ʼ��� ����ռ�ÿ��� ���ռ�ÿ��ƫ����
	char* buf = (char*)v_buf;
	start_block = fp->f_pos / BLOCK_SIZE; //���ļ��ĵڼ���block��ʼ��
	block_num = (fp->f_pos + size) / BLOCK_SIZE - start_block + 1; //����������block
	offset = (fp->f_pos + size) % BLOCK_SIZE;//���һ��block��off_setƫ����
	if (offset == 0 && block_num > 1)
		offset = BLOCK_SIZE; //ƫ��Ϊ0˵��Ҫ����һ����

	FILE* diskp;
	diskp = fopen(DISK, "rb");
	if (block_num == 1)
	{
		fseek(diskp, fp->f_iNode->block_address[0] * BLOCK_SIZE + fp->f_pos, SEEK_SET);//��λ���ļ���ʼ��ַ�Ӷ�ָ��λ��
		fread(buf, size, 1, diskp);
	}
	else
	{
		int read_bytes = 0; //�ۼ��Ѷ����ֽ���
		int cur_bytes = 0; //��ǰ������Ҫ��ȡ���ֽ���
		for (int block = start_block; block < block_num + start_block; block++) //���������Ŀ�
		{
			if (block == start_block) //��һ��
				cur_bytes = BLOCK_SIZE - fp->f_pos % BLOCK_SIZE;//��һ�����������ֽ���

			else if (block == start_block + block_num - 1) //���һ��
			{
				cur_bytes = offset;//���һ���������ֽ���
			}
			else
			{
				cur_bytes = BLOCK_SIZE;//��Ҫ����ȫ�����ݵ��м��
			}
			rewind(diskp);//�ѵ�ǰ�Ķ�дλ�ûص��ļ���ʼ
			fseek(diskp, fp->f_iNode->block_address[block] * BLOCK_SIZE + fp->f_pos + read_bytes, SEEK_SET);//��λ
			fread(buf + read_bytes, cur_bytes, 1, diskp);//������������
			read_bytes += cur_bytes;
		}
	}
	fclose(diskp);
	v_buf = (void*)buf;
	return 1;
}

//�ļ�д����
int disk::os_writefile(void* v_buf, int size, os_file* fp)//������������ݻ���������д������ݴ�С�����ļ��ľ�������ļ���������ݴ�С���������ʼ��ţ������Ŀ����������ƫ������������̿�ʱ����Ѱ�ҿ��п�ź���Ϊ�������п顣
{
	//����Ҫд�Ŀ�����ƫ����
	int first_block, block_num, offset, total_block;//�ֱ��¼�������ڵ� ��ʼ��� ����ռ�ÿ��� ���ռ�ÿ��ƫ����
	char* buf = (char*)malloc(size);
	memcpy(buf, v_buf, size); //����

	total_block = (fp->f_pos + size) / BLOCK_SIZE + 1; //д���ļ����ļ�����block��
	if (total_block > FBLK_NUM)  //д�����ݿ��������ļ���������
		return 0;

	first_block = fp->f_pos / BLOCK_SIZE; //���ļ��ĵڼ���block��ʼд
	block_num = (fp->f_pos + size) / BLOCK_SIZE - first_block + 1; //����д����block
	offset = (fp->f_pos + size) % BLOCK_SIZE;//���һ��block��offset

	if (offset == 0 && block_num > 1)
		offset = BLOCK_SIZE; //ƫ��Ϊ0��д���ݿ����һ ˵�����һ��Ҫд��һ����

	//����Ƿ���Ҫ�����п������ļ�
	for (int i = 0; i < total_block; i++)
	{
		if (fp->f_iNode->block_address[i] > MAX_BLOCK_NUM) //˵���ļ��ĵ�i����δ������
		{
			fp->f_iNode->block_address[i] = disk::first_free();
			if (fp->f_iNode->block_address[i] == -1)
			{
				fp->f_iNode->block_address[i] = MAX_BLOCK_NUM + 1;
				return 0; //û�п��п���Է���
			}
		}
	}

	FILE* diskp;
	diskp = fopen(DISK, "rb+"); //�򿪴����ļ�
	if (block_num == 1)
	{
		rewind(diskp);
		fseek(diskp, fp->f_pos + fp->f_iNode->block_address[first_block] * BLOCK_SIZE, SEEK_SET);//��λ�ļ�ָ������д�봦
		fwrite(buf, size, 1, diskp);
	}
	else
	{
		int write_bytes = 0; //��д����ֽ���
		int cur_bytes = 0; //��ǰ������Ҫд����ֽ���
		for (int block = first_block; block < (first_block + block_num); block++)
		{
			if (block == first_block) //��һ��
				cur_bytes = BLOCK_SIZE - fp->f_pos % BLOCK_SIZE;
			else if (block == first_block + block_num - 1)  //���һ��
				cur_bytes = offset;
			else //��Ҫд��ȫ�����ݵ��м��
				cur_bytes = BLOCK_SIZE;
			rewind(diskp);
			fseek(diskp, fp->f_pos + fp->f_iNode->block_address[block] * BLOCK_SIZE + write_bytes, SEEK_SET);
			fwrite(buf + write_bytes, cur_bytes, 1, diskp);
			write_bytes += cur_bytes;
		}
	}
	fp->f_pos += size; //�����ļ�ָ��
	fp->f_iNode->i_size += size; //�����ļ���С
	fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);
	fwrite(FileManager::iNode_table, sizeof(iNode) * iNode_NUM, 1, diskp); //iNode_table����
	fclose(diskp); //�رմ��̶�д
	free(buf);
	return 1;
}

//�����ļ�ϵͳ�Ķ�Ŀ¼�ӿ�
void disk::get_dir(void* dir_buf, iNode* f_inode)
{
	FILE* diskp = fopen(DISK, "rb");
	if (diskp)
	{
		char* buf = (char*)dir_buf;
		for (int i = 0; i < FBLK_NUM; i++)
		{
			fseek(diskp, f_inode->block_address[i] * BLOCK_SIZE, SEEK_SET); //��λ�ļ�ָ��
			fread(buf + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
		}
	}
}

//���̿�д�뻺����
bool disk::write_block(long block, char* buf)
{
    FILE* diskp = fopen(DISK, "rb+");
    if (diskp) {
        fseek(diskp, (block * BLOCK_SIZE), SEEK_SET); //��λ����Ӧ���λ��
        fwrite(buf, BLOCK_SIZE, 1, diskp);
        fclose(diskp);
        return true;
    }
}

//���̿���뻺����
bool disk::read_block(long block, char* buf)
{
    FILE* diskp = fopen(DISK, "r");
    if (diskp) {
        fseek(diskp, (block * BLOCK_SIZE), SEEK_SET); //��λ����Ӧ���λ��
        fread(buf, BLOCK_SIZE, 1, diskp);
        fclose(diskp);
        return true;
    }
}