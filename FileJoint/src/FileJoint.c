/*
 ============================================================================
 Name        : FileJoint.c
 Author      : LiuNing
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "ini.h"

#define BUF_SIZE            (1024)

int main(void) {
    filejoint_ini_t fji;
    char buf[BUF_SIZE];
    FILE* fdest = NULL;
    FILE* fcur = NULL;
    int file_size = 0;
    int write_size = 0;
    int len = 0;
    int i;

	puts("!!!FileJoint!!!");

    /* 0. ��ʼ������ */
    if (0 != ini_get_info(&fji))
    {
        printf("ini get info err!\n");
        goto mainend;
    }
    //todo: log

    /* 1. У���ļ����� */
    for (i = 0; i < fji.files; i++)
    {
        fcur = fopen(fji.file[i].filename, "rb");
        if (fcur == NULL)
        {
            printf("%s������!\n", fji.file[i].filename);
            goto mainend;
        }
        fseek(fcur, 0, SEEK_END);
        file_size = ftell(fcur);
        if (file_size > fji.file[i].filemaxsize)
        {
            printf("%s:size[%d > %d]\n", fji.file[i].filename,
                    file_size, fji.file[i].filemaxsize);
            goto mainend;
        }
        (void)fclose(fcur);
        fcur = NULL;
    }

    /* 2. ����Ŀ���ļ� */
    fdest = fopen(fji.outfile, "w+b");
    if (fdest == NULL)
    {
        printf("�����ļ�%sʧ��!\n", fji.outfile);
        goto mainend;
    }

    /* 3. д������ */
    for (i = 0; i < fji.files; i++)
    {
        fcur = fopen(fji.file[i].filename, "rb");
        if (fcur == NULL)
        {
            printf("%s������!\n", fji.file[i].filename);
            goto mainend;
        }
        fseek(fcur, 0, SEEK_END);
        file_size = ftell(fcur);
        write_size = 0;
        fseek(fcur, 0, SEEK_SET);
        while (write_size < file_size)
        {
            len = fread(buf, 1, BUF_SIZE, fcur);
            if ((len <= 0) || (len != fwrite(buf, 1, len, fdest)))
            {
               printf("д�ļ�����[L%d]!\n", __LINE__);
               goto mainend;
            }
            write_size += len;
        }
        (void)fclose(fcur);
        fcur = NULL;
        //����ȱ�ռ�
        memset(buf, fji.blank, sizeof(buf));
        write_size = fji.file[i].filemaxsize - file_size;
        while (write_size > 0)
        {
            len = (write_size > BUF_SIZE) ? BUF_SIZE : write_size;
            if (len != fwrite(buf, 1, len, fdest))
            {
               printf("д�ļ�����[L%d]!\n", __LINE__);
               goto mainend;
            }
            write_size -= len;
        }
    }

mainend:
	if (fdest != NULL)
	{
	    fclose(fdest);
	}
    if (fcur != NULL)
    {
        fclose(fcur);
    }

	return EXIT_SUCCESS;
}
