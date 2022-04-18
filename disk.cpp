#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "file_dir.h"
using namespace std;

//ʵ�ֽ���ģ������ļ���������㹻����ڴ沢д���ļ�������true��false��ʾ�����ɹ����
bool format_disk()
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
bool init_disk()
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
	dir root[MAX_FILE_NUM];
	DIR::init_dir(root);
	iNode root_iNode = File::Fill_in_iNode(0);//��Ŀ¼��Ӧ�ڵ�
	for (int i = 0; i < FBLK_NUM; i++)
	{
		root_iNode.block_address[i] = border + 1 + i;//ռ��data��ǰ�ĸ���
		*(bitmap + border + i + 1) = 1;//�޸�bitmap
	}
	*inode_table = root_iNode;//����inode��

	//д�����
	FILE* disk = fopen(DISK, "rb+"); //��д��һ���������ļ���ֻ�����д����
	fwrite(sb, sizeof(super_block), 1, disk); //д�볬��������
	fseek(disk, INODE_START * BLOCK_SIZE, SEEK_SET);//��λ������ָ����λ��,SEEK_SET 0 �ļ���ͷ
	fwrite(inode_table, sizeof(iNode) * iNode_NUM, 1, disk); //д��inode_table
	fseek(disk, BITMAP_START * BLOCK_SIZE, SEEK_SET);//��λ��bitmap��λ��
	fwrite(bitmap, MAX_BLOCK_NUM, 1, disk); //д��bitmap
	for (int i = 0; i < FBLK_NUM; i++) //root_dirд�����
	{
		fseek(disk, root_iNode.block_address[i] * BLOCK_SIZE, SEEK_SET);//��λ��root_dir��λ��
		fwrite((char*)root + i * BLOCK_SIZE, BLOCK_SIZE, 1, disk);
	}
	free(sb);
	free(inode_table);
	free(bitmap);
	fclose(disk);
	return true;
}

//ʵ�ִ��̸�ʽ���ͳ�ʼ���󣬶���ϵͳ����ĸ�������������iNode_table��root_dir������true��false��ʾ�����ɹ����
bool start_disk()
{
	dir* temp_root = root_dir;//��ʱ��Ŀ¼
	while (1)
	{
		FILE* diskp = fopen(DISK, "rb");
		if(debug){
			format_disk();
			init_disk();
		}
		if (diskp)
		{
			fseek(diskp, INODE_START * BLOCK_SIZE, SEEK_SET);//��λ��inode��λ��
			fread(&iNode_table, iNode_NUM * sizeof(iNode), 1, diskp);//����ȫ�ֱ���iNode_table��
			for (int i = 0; i < FBLK_NUM; i++) //����root_dir��Ŀ¼
			{
				fseek(diskp, iNode_table[0].block_address[i] * BLOCK_SIZE, SEEK_SET);
				fread((char*)temp_root + i * BLOCK_SIZE, BLOCK_SIZE, 1, diskp);
			}
			current_dir = root_dir;
			int j = 0;
			for (int j = 0; j < iNode_NUM; j++)
			{
				if (iNode_table[j].i_mode != 2)
					iNode_table[j].open_num = 0;
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

int first_free()//���ҵ�һ�����п�ţ��޸���bitmapΪ1��ʾ��ռ�ã��������е�λͼ���������ҿ��п�ţ��޸���Ϣ��дд�ش��̣����ظÿ�š�
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

int release_block(int block)//�൱�ڴ��̿�ĸ�ʽ�����������е�λͼ���������ÿ��Ƿ���ͷţ������ͷ��򽫶�Ӧλͼ��0����λͼд�ش��̣���ʽ���ô��̿飬�൱���ͷŸÿ顣
{
	if (block <= DATA_START)
	{
		return 0; //�ͷŲ��������Ĵ��̿飬����
	}
	char* bitmap = (char*)malloc(MAX_BLOCK_NUM);//��ʱ���λͼ
	char* block_format = (char*)malloc(BLOCK_SIZE); //���ڴ��̿�ĸ�ʽ��

	FILE* diskp = fopen(DISK, "rb+");//��д��һ���������ļ���ֻ�����д���ݡ�
	fseek(diskp, BITMAP_START * BLOCK_SIZE, SEEK_SET);//��λ��λͼλ��
	fread(bitmap, MAX_BLOCK_NUM, 1, diskp); //����bitmap

	*(bitmap + block) = 0; //bitmap��Ӧ����0
	fseek(diskp, BLOCK_SIZE * BITMAP_START, SEEK_SET); //�ļ�ָ�붨λ��bitmap���
	fwrite(bitmap, MAX_BLOCK_NUM, 1, diskp); //��bitmapд�ش���
	fseek(diskp, block * BLOCK_SIZE, SEEK_SET); //��ָ���ƶ���Ҫ��ʽ���Ĵ��̿�λ��
	fwrite(block_format, BLOCK_SIZE, 1, diskp); //���̿��ʽ�����൱���ͷ�
	free(bitmap);
	free(block_format);
	fclose(diskp);
	return 1;
}

//��ȡ�ļ���С
int get_filesize(os_file* fp)
{
	return fp->f_iNode->i_size;
}

//�ļ�������
int os_readfile(void* v_buf, int size, os_file* fp)//������������ݻ������������������ݴ�С�����ļ��ľ���������ļ���������ݴ�С���������ʼ��ţ������Ŀ����������ƫ����������Щ����д����������
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
int os_writefile(void* v_buf, int size, os_file* fp)//������������ݻ���������д������ݴ�С�����ļ��ľ�������ļ���������ݴ�С���������ʼ��ţ������Ŀ����������ƫ������������̿�ʱ����Ѱ�ҿ��п�ź���Ϊ�������п顣
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
			fp->f_iNode->block_address[i] = first_free();
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
			else //��Ҫ����ȫ�����ݵ��м��
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
	fwrite(iNode_table, sizeof(iNode) * iNode_NUM, 1, diskp); //iNode_table����
	fclose(diskp); //�رմ��̶�д
	free(buf);
	return 1;
}

//�����ļ�ϵͳ�Ķ�Ŀ¼�ӿ�
void get_dir(void* dir_buf, iNode* f_inode)
{
	//���ݼ����֪һ��Ŀ¼��ռ32�ֽ�,һ��Ŀ¼��8���ļ��൱��4��block
	//int blk_nr, off_set;
	//blk_nr = DIR_NUM * sizeof(dir) / BLOCK_SIZE + 1;
	//off_set = DIR_NUM * sizeof(dir) % BLOCK_SIZE;
	int read_bytes = 0; //�Ѷ��ֽ���
	FILE* disk_dir = fopen(DEV_NAME, "rb");
	if (disk_dir) {
		char* buf = (char*)dir_buf;
		for (int i = 0; i < FBLK_NUM; i++) {
			fseek(disk_dir, f_inode->block_address[i] * BLOCK_SIZE, SEEK_SET); //��λ�ļ�ָ��
			fread(buf + i * BLOCK_SIZE, BLOCK_SIZE, 1, disk_dir);
		}
		//fclose(disk_dir);
	}
}
