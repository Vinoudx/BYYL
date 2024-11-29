// PL0Compiler1_Net.cpp : 定义控制台应用程序的入口点.
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

void PrintSourceFile()//打印源程序文件中的所有语句
{
char		cACharacter;

	fseek(fpSourceFile, 0L, SEEK_SET);//源程序文件的文件指针指向起始位置

	while(fscanf(fpSourceFile,"%c",&cACharacter)!=EOF)//从文件中读取一个字符
		printf("%c",cACharacter);//打印这个字符
}


