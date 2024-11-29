// PL0Compiler1_Net.cpp : �������̨Ӧ�ó������ڵ�.
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "stdio.h"
#include "string.h"


enum	WORD_TYPE_ENUM;

void InitializeSingleCharacterTable();
void	InitializeReservedWordTable();
void PrintSourceFile();
int	 Initialization();
void LexicalAnalysis();
int ParsingAnalysis();
int GenerateAnalysis();
void Interpreter();

#define		SOURCE_FILE_NAME				"D:\\work\\BYYLL\\BYYLL\\test1.pl0"
//#define		SOURCE_FILE_NAME				"SourceFile1.pl0"

FILE*														fpSourceFile;

int main(int argc, char* argv[])
{
	Initialization();
	PrintSourceFile();

	
	LexicalAnalysis();


	ParsingAnalysis();


	GenerateAnalysis();


	Interpreter();


	return 0;
}

int Initialization()
{
	char	szFileName[100];

	strcpy(szFileName,SOURCE_FILE_NAME);
	if((fpSourceFile=fopen(szFileName,"r"))==NULL)
	{
		printf("Can not open source file %s",szFileName);
		return ERROR;
	}

	return OK;
}

void PrintSourceFile()//��ӡԴ�����ļ��е��������
{
char		cACharacter;

	fseek(fpSourceFile, 0L, SEEK_SET);//Դ�����ļ����ļ�ָ��ָ����ʼλ��

	while(fscanf(fpSourceFile,"%c",&cACharacter)!=EOF)//���ļ��ж�ȡһ���ַ�
		printf("%c",cACharacter);//��ӡ����ַ�
}


