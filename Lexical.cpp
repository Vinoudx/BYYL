#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Error.h"

#define PASS_PRINTINLEXIS 0 //ss 注释时跳过输出部分

enum	WORD_TYPE_ENUM {//单词类型枚举值
	INVALID_WORD,
	IDENTIFIER,
	NUMBER,
	CONST,
	VAR,
	PROCEDURE,
	BEGIN,
	END,
	IF,
	//THEN, // ss
	WHILE,
	DO,
	WRITE,
	READ,
	CALL,
	LEFT_PARENTHESIS,
	RIGHT_PARENTHESIS,
	COMMA,
	SEMICOLON,
	PERIOD,
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	ODD,
	EQL,
	NEQ,
	LES,
	LEQ,
	GTR,
	GEQ,
	ASSIGN,
	ELSE,//ss
	SWITCH,//ss
	CASE,//ss
	BREAK,//ss
	DEFAULT,//ss
	ENDSWITCH,//ss
	COLON//ss
};
//保留字的名字字符串和类型对照表结构
struct	RESERVED_WORD_NAME_VS_TYPE_STRUCT {
	char											szName[20];//保留字的名字字符串
	enum	WORD_TYPE_ENUM							eType;//保留字的单词类型枚举值
};

#define		MAX_LENGTH_OF_A_WORD								10		//一个单词的最多字符个数
#define		MAX_NUMBER_OF_WORDS									1000	//可识别的最多单词个数
#define		NUMBER_OF_RESERVED_WORDS							18		//保留字个数 ss

struct	WORD_STRUCT {//一个单词的数据结构
	char											szName[MAX_LENGTH_OF_A_WORD];//单词名字的字符串
	enum WORD_TYPE_ENUM								eType;//单词类型枚举值
	char											nIdentifierValue[MAX_LENGTH_OF_A_WORD];//标识符单词的值 ss
	char											nReserveValue[MAX_LENGTH_OF_A_WORD];//保留字标识符单词的值 ss
	char											nCharValue[5];//运算符单词的值 ss
	int												nNumberValue;//数单词的值
	int												nLineNo;//在源代码文件中单词所在的行数
};

RESERVED_WORD_NAME_VS_TYPE_STRUCT				ReservedWordNameVsTypeTable[NUMBER_OF_RESERVED_WORDS];//保留字的名字字符串和类型对照表
WORD_TYPE_ENUM													SingleCharacterWordTypeTable[256];//单字符单词的字符和类型对照表
WORD_STRUCT							g_Words[MAX_NUMBER_OF_WORDS];//已识别出的单词队列
int													g_nWordsIndex;//已识别出的单词的个数或序号
int													g_nLineNo = 1;//文件中源代码的行数

extern	FILE* fpSourceFile;//源程序文件的文件指针

int	WordTypeToString(char string[100], WORD_TYPE_ENUM eWordType);
void PrintInLexis(int nWordsIndex);
int	GetAWord();

void	InitializeReservedWordTable()//设置保留字单词的名字字符串和相应类型的对照表
{
	strcpy(ReservedWordNameVsTypeTable[0].szName, "begin");				ReservedWordNameVsTypeTable[0].eType = BEGIN;
	strcpy(ReservedWordNameVsTypeTable[1].szName, "call");				ReservedWordNameVsTypeTable[1].eType = CALL;
	strcpy(ReservedWordNameVsTypeTable[2].szName, "const");				ReservedWordNameVsTypeTable[2].eType = CONST;
	strcpy(ReservedWordNameVsTypeTable[3].szName, "do");				ReservedWordNameVsTypeTable[3].eType = DO;
	strcpy(ReservedWordNameVsTypeTable[4].szName, "end");				ReservedWordNameVsTypeTable[4].eType = END;
	strcpy(ReservedWordNameVsTypeTable[5].szName, "if");				ReservedWordNameVsTypeTable[5].eType = IF;
	strcpy(ReservedWordNameVsTypeTable[6].szName, "odd");				ReservedWordNameVsTypeTable[6].eType = ODD;
	strcpy(ReservedWordNameVsTypeTable[7].szName, "procedure");			ReservedWordNameVsTypeTable[7].eType = PROCEDURE;
	strcpy(ReservedWordNameVsTypeTable[8].szName, "read");				ReservedWordNameVsTypeTable[8].eType = READ;
	strcpy(ReservedWordNameVsTypeTable[9].szName, "var");				ReservedWordNameVsTypeTable[9].eType = VAR;
	strcpy(ReservedWordNameVsTypeTable[10].szName, "while");			ReservedWordNameVsTypeTable[10].eType = WHILE;
	strcpy(ReservedWordNameVsTypeTable[11].szName, "write");			ReservedWordNameVsTypeTable[11].eType = WRITE;
	strcpy(ReservedWordNameVsTypeTable[12].szName, "else");				ReservedWordNameVsTypeTable[12].eType = ELSE;// ss
	strcpy(ReservedWordNameVsTypeTable[13].szName, "switch");			ReservedWordNameVsTypeTable[13].eType = SWITCH;// ss
	strcpy(ReservedWordNameVsTypeTable[14].szName, "case");				ReservedWordNameVsTypeTable[14].eType = CASE;// ss
	strcpy(ReservedWordNameVsTypeTable[15].szName, "break");			ReservedWordNameVsTypeTable[15].eType = BREAK;// ss
	strcpy(ReservedWordNameVsTypeTable[16].szName, "default");			ReservedWordNameVsTypeTable[16].eType = DEFAULT;// ss
	strcpy(ReservedWordNameVsTypeTable[17].szName, "endswitch");		ReservedWordNameVsTypeTable[17].eType = ENDSWITCH;// ss
}

void InitializeSingleCharacterTable()//设置单字符单词的字符和相应类型的对照表
{
	int i;
	for (i = 0; i <= 255; i++)
	{
		SingleCharacterWordTypeTable[i] = INVALID_WORD;
	}

	SingleCharacterWordTypeTable['+'] = PLUS;
	SingleCharacterWordTypeTable['-'] = MINUS;
	SingleCharacterWordTypeTable['*'] = MULTIPLY;
	SingleCharacterWordTypeTable['/'] = DIVIDE;
	SingleCharacterWordTypeTable['('] = LEFT_PARENTHESIS;
	SingleCharacterWordTypeTable[')'] = RIGHT_PARENTHESIS;
	SingleCharacterWordTypeTable['='] = EQL;
	SingleCharacterWordTypeTable[','] = COMMA;
	SingleCharacterWordTypeTable['.'] = PERIOD;
	SingleCharacterWordTypeTable['#'] = NEQ;
	SingleCharacterWordTypeTable[';'] = SEMICOLON;
	SingleCharacterWordTypeTable[':'] = COLON;
}

//词法分析
void LexicalAnalysis()
{
	int		nResult;

	InitializeReservedWordTable();//设置保留字单词的名字字符串和相应类型的对照表
	InitializeSingleCharacterTable();//设置单字符单词的字符和相应类型的对照表
	fseek(fpSourceFile, 0L, SEEK_SET);//源程序文件的文件指针指向起始位置

	printf("\n----------------Lexical Analysis Begin!--------------\n");

	g_nWordsIndex = 0;//g_nWordsIndex是已识别单词的序号

	nResult = GetAWord();//词法分析,识别第一个单词
	while ((nResult == OK || nResult == PASS_PRINTINLEXIS) && g_nWordsIndex >= 1)
	{
		if (nResult != PASS_PRINTINLEXIS) {
			PrintInLexis(g_nWordsIndex - 1);//打印一个单词
		}
		nResult = GetAWord();//词法分析,识别下一个单词
	}

	printf("----------------Lexical Analysis Finished!--------------\n");
}
//从文件中读取一个字符然后返回
char GetACharacterFromFile()
{
	char	cACharacter;

	//从文件中读取的一个字符
	if (fscanf(fpSourceFile, "%c", &cACharacter) == EOF)
		cACharacter = EOF;

	if (cACharacter == RETURN)//如果是回车符
		g_nLineNo++;//源代码行数加一

	return cACharacter;//返回从文件中读取的一个字符
}

int GetAWord()//词法分析,获取一个单词
{
	static		char	cACharacter = SPACE;//当前字符
	int			nAWordIndex;//单词数组下标,表示单词第几个字符
	int			nDigitNumber;//数单词的字符个数
	int			i;
	char		szAWord[MAX_LENGTH_OF_A_WORD + 1];
	int			nNumberValue;	//数的值

	//忽略空格、换行和TAB
	while ((cACharacter == SPACE || cACharacter == RETURN || cACharacter == TABLE) && cACharacter != EOF)
		cACharacter = GetACharacterFromFile();//从文件中读取一个字符然后返回

	if (cACharacter != EOF)//如果不是文件末尾
	{
		if (cACharacter >= 'a' && cACharacter <= 'z')//如果当前字符是小写的字母
		{//标识符或保留字以a..z开头
			nAWordIndex = 0;//单词的第一个字符
			do {
				if (nAWordIndex < MAX_LENGTH_OF_A_WORD)
					szAWord[nAWordIndex++] = cACharacter;//把当前字符放进单词数组里,单词数组下标加一

				cACharacter = GetACharacterFromFile();//从文件中再读取一个字符
			} while ((cACharacter >= 'a' && cACharacter <= 'z' || cACharacter >= '0' && cACharacter <= '9' || cACharacter == '_') && cACharacter != EOF);
			//只要后面继续跟a..z或0..9,就要继续看后面的字符 ss 标识符可以含有'_'

			//当前字符不再是a..z或0..9
			if (cACharacter != EOF)//如果不是文件末尾
			{
				szAWord[nAWordIndex] = 0;//以0结尾,在单词数组中组成一个单词字符串

				for (i = 0; i < NUMBER_OF_RESERVED_WORDS; i++)//从保留字表中查询当前单词字符串是否为某一保留字字符串
					if (strcmp(szAWord, ReservedWordNameVsTypeTable[i].szName) == 0)
					{
						//如果是保留字,则赋当前单词类型为相应保留字的单词类型枚举值
						g_Words[g_nWordsIndex].eType = ReservedWordNameVsTypeTable[i].eType;
						strcpy(g_Words[g_nWordsIndex].nReserveValue, szAWord);//将保留字的值保存 ss
						break;
					}
				if (i >= NUMBER_OF_RESERVED_WORDS) {
					//如果在保留字表中查询不出,则当前单词是标识符,单词类型为单词类型枚举值IDENTIFIER
					g_Words[g_nWordsIndex].eType = IDENTIFIER;
					strcpy(g_Words[g_nWordsIndex].nIdentifierValue, szAWord);//将标识符的值保存 ss
				}
				strcpy(g_Words[g_nWordsIndex].szName, szAWord);//识别出的单词放进单词队列g_Words中
				g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数

				g_nWordsIndex++;//识别出的单词个数加一

				return OK;
			}
			else

				return ERROR;
		}
		else
			if (cACharacter >= '0' && cACharacter <= '9')
			{//数字是以0..9开头
				nDigitNumber = 0;//数单词的字符个数
				nNumberValue = 0;//数单词的值
				do {
					szAWord[nDigitNumber++] = cACharacter;
					nNumberValue = 10 * nNumberValue + cACharacter - '0';//计算数单词的值

					cACharacter = GetACharacterFromFile();//再取下一个字符
				} while (cACharacter >= '0' && cACharacter <= '9' && cACharacter != EOF);//如果是0..9,则继续拼装数单词

				//如果不是0..9则数单词结束
				if (cACharacter != EOF)
				{
					szAWord[nDigitNumber] = 0;//以0结尾在单词数组中组成一个数单词字符串

					g_Words[g_nWordsIndex].eType = NUMBER;//单词类型为单词类型枚举值NUMBER
					strcpy(g_Words[g_nWordsIndex].szName, szAWord);//识别出的单词放进单词队列g_Words中
					g_Words[g_nWordsIndex].nNumberValue = nNumberValue;//同时将数的值赋给单词的nNumberValue
					g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数

					g_nWordsIndex++;//识别出的单词个数加一

					return OK;
				}
				else
					return ERROR;
			}
			else
				if (cACharacter == ':')//当前符号是':'？
				{
					cACharacter = GetACharacterFromFile();//如果当前符号是':',则要再看下一个符号
					if (cACharacter == '=')//如果下一个符号是'='
					{//则单词是赋值符号":="
						g_Words[g_nWordsIndex].eType = ASSIGN;//单词类型为单词类型枚举值ASSIGN
						strcpy(g_Words[g_nWordsIndex].szName, ":=");//识别出的单词放进单词队列g_Words中
						g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数
						strcpy(g_Words[g_nWordsIndex].nCharValue, ":="); //ss
						g_nWordsIndex++;//识别出的单词个数加一

						cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备

						return OK;
					}
					else {
						g_Words[g_nWordsIndex].eType = COLON;//单词类型为单词类型枚举值ASSIGN
						strcpy(g_Words[g_nWordsIndex].szName, ":");//识别出的单词放进单词队列g_Words中
						g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数
						strcpy(g_Words[g_nWordsIndex].nCharValue, ":"); //ss
						g_nWordsIndex++;//识别出的单词个数加一

						cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备

						return OK;
					}
				}
				else
					if (cACharacter == '<')//检测是"<"还是"<="单词?
					{
						cACharacter = GetACharacterFromFile();//再取一个符号
						if (cACharacter == '=')
						{//单词是"<="
							g_Words[g_nWordsIndex].eType = LEQ;//单词类型为单词类型枚举值LEQ
							strcpy(g_Words[g_nWordsIndex].szName, "<=");//识别出的单词放进单词队列g_Words中
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数
							strcpy(g_Words[g_nWordsIndex].nCharValue, "<="); //ss
							g_nWordsIndex++;//识别出的单词个数加一

							cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备

							return OK;
						}
						else
						{//单词仅是"<"
							g_Words[g_nWordsIndex].eType = LES;//单词类型为单词类型枚举值LES
							strcpy(g_Words[g_nWordsIndex].szName, "<");//识别出的单词放进单词队列g_Words中
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//在源代码文件中单词所在的行数
							strcpy(g_Words[g_nWordsIndex].nCharValue, "<"); //ss
							g_nWordsIndex++;//识别出的单词个数加一

							return OK;
						}
					}
					else
						if (cACharacter == '>')//检测是">"还是">="单词?
						{
							cACharacter = GetACharacterFromFile();//再取一个符号
							if (cACharacter == '=')
							{//单词是">="
								g_Words[g_nWordsIndex].eType = GEQ;//单词类型为单词类型枚举值GEQ
								strcpy(g_Words[g_nWordsIndex].szName, ">=");//识别出的单词放进单词队列g_Words中
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, ">="); //ss
								g_nWordsIndex++;//识别出的单词个数加一

								cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备
							}
							else
							{//单词仅是">"
								g_Words[g_nWordsIndex].eType = GTR;//单词类型为单词类型枚举值GTR
								strcpy(g_Words[g_nWordsIndex].szName, ">");//识别出的单词放进单词队列g_Words中
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, ">"); //ss
								g_nWordsIndex++;//识别出的单词个数加一
							}
							return OK;
						}
						else if (cACharacter == '/') {// 检测是“/”还是“/**/” ss
							cACharacter = GetACharacterFromFile();
							if (cACharacter == '*') {//是注释
								while (true) {
									cACharacter = GetACharacterFromFile();
									if (cACharacter == '*') {
										cACharacter = GetACharacterFromFile();
										if (cACharacter == '/') {
											break;
										}
									}
								}
								cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备
								return PASS_PRINTINLEXIS;
							}
							else { //是除法
								g_Words[g_nWordsIndex].eType = DIVIDE;//单词类型为单词类型枚举值GTR
								strcpy(g_Words[g_nWordsIndex].szName, "/");//识别出的单词放进单词队列g_Words中
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, "/");
								g_nWordsIndex++;//识别出的单词个数加一
							}
							return OK;
						}
						else
						{
							//当不满足上述条件时,则是单字符
							//通过查表寻找单字符的单词类型枚举值
							g_Words[g_nWordsIndex].eType = SingleCharacterWordTypeTable[cACharacter];
							g_Words[g_nWordsIndex].szName[0] = cACharacter;//单词放进单词队列g_Words中
							g_Words[g_nWordsIndex].szName[1] = 0;//以0结尾组成一个字符串
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
							g_Words[g_nWordsIndex].nCharValue[0] = cACharacter; //ss
							g_nWordsIndex++;//识别出的单词个数加一

							cACharacter = GetACharacterFromFile();//再取一个符号,为下一步准备

							return OK;
						}
	}
	return ERROR;
}

void PrintInLexis(int nIndex)//打印单词队列中的一个单词
{
	char	szWordName[100];
	char	szWordType[100];

	strcpy(szWordName, g_Words[nIndex].szName);
	WordTypeToString(szWordType, g_Words[nIndex].eType);//将单词类型的枚举值转换成字符串

	switch (g_Words[nIndex].eType)
	{
	case	IDENTIFIER:
		printf("%-4d%-15s%-20s%-10s\n", nIndex, szWordName, szWordType, g_Words[nIndex].nIdentifierValue);
		break;
	case NUMBER:
		printf("%-4d%-15s%-20s%-10d\n", nIndex, szWordName, szWordType, g_Words[nIndex].nNumberValue);
		break;
	case	CONST:
	case	VAR:
	case	PROCEDURE:
	case	BEGIN:
	case	END:
	case	IF:
	case	ELSE: // ss
	case    SWITCH:
	case    CASE:
	case	BREAK:
	case	DEFAULT:
	case	ENDSWITCH:
	case	COLON://ss
	case	WHILE:
	case	DO:
	case	WRITE:
	case	READ:
	case	CALL:
	case	LEFT_PARENTHESIS:
	case	RIGHT_PARENTHESIS:
	case	COMMA:
	case	SEMICOLON:
	case	PERIOD:
	case	PLUS:
	case	MINUS:
	case	MULTIPLY:
	case	DIVIDE:
	case	ODD:
	case	EQL:
	case	NEQ:
	case	LES:
	case	LEQ:
	case	GTR:
	case	GEQ:
	case	ASSIGN:
		printf("%-4d%-15s%-20s%-10s\n", nIndex, szWordName, szWordType,
			strlen(g_Words[nIndex].nReserveValue) == 0 ? g_Words[nIndex].nCharValue : g_Words[nIndex].nReserveValue);
		break;
	default:
		printf("%-4d%-15s%-20s%-10s\n", nIndex, szWordName, szWordType, g_Words[nIndex].nCharValue);
	}
}
//将单词类型的枚举值转换成字符串
int WordTypeToString(char strString[100], WORD_TYPE_ENUM eWordType)
{
	switch (eWordType)
	{
	case	IDENTIFIER:							strcpy(strString, "IDENTIFIER");
		break;
	case	NUMBER:								strcpy(strString, "NUMBER");
		break;
	case	PLUS:								strcpy(strString, "PLUS");
		break;
	case	MULTIPLY:							strcpy(strString, "MULTIPLY");
		break;
	case	MINUS:								strcpy(strString, "MINUS");
		break;
	case	DIVIDE:								strcpy(strString, "DIVIDE");
		break;
	case	LES:								strcpy(strString, "LES");
		break;
	case	LEQ:								strcpy(strString, "LEQ");
		break;
	case	GTR:								strcpy(strString, "GTR");
		break;
	case	GEQ:								strcpy(strString, "GEQ");
		break;
	case	EQL:								strcpy(strString, "EQL");
		break;
	case	NEQ:								strcpy(strString, "NEQ");
		break;
	case	LEFT_PARENTHESIS:					strcpy(strString, "LEFT_PARENTHESIS");
		break;
	case	RIGHT_PARENTHESIS:					strcpy(strString, "RIGHT_PARENTHESIS");
		break;
	case	COMMA:								strcpy(strString, "COMMA");
		break;
	case	SEMICOLON:							strcpy(strString, "SEMICOLON");
		break;
	case	PERIOD:								strcpy(strString, "PERIOD");
		break;
	case	ASSIGN:								strcpy(strString, "ASSIGN");
		break;
	case	CONST:								strcpy(strString, "CONST");
		break;
	case	VAR:								strcpy(strString, "VAR");
		break;
	case	PROCEDURE:							strcpy(strString, "PROCEDURE");
		break;
	case	BEGIN:								strcpy(strString, "BEGIN");
		break;
	case	END:								strcpy(strString, "END");
		break;
	case	IF:									strcpy(strString, "IF");
		break;
	case	ELSE:								strcpy(strString, "ELSE"); // ss
		break;
	case	SWITCH:								strcpy(strString, "SWITCH"); // ss
		break;
	case	CASE:								strcpy(strString, "CASE"); // ss
		break;
	case	BREAK:								strcpy(strString, "BREAK"); // ss
		break;
	case	DEFAULT:							strcpy(strString, "DEFAULT"); // ss
		break;
	case	ENDSWITCH:							strcpy(strString, "ENDSWITCH"); // ss
		break;
	case	COLON:								strcpy(strString, "COLON"); // ss
		break;
	case	ODD:								strcpy(strString, "ODD");
		break;
	case	WHILE:								strcpy(strString, "WHILE");
		break;
	case	DO:									strcpy(strString, "DO");
		break;
	case	CALL:								strcpy(strString, "CALL");
		break;
	case	READ:								strcpy(strString, "READ");
		break;
	case	WRITE:								strcpy(strString, "WRITE");
		break;
	case	INVALID_WORD:						strcpy(strString, "INVALID_WORD");
		break;
	}
	return NULL;
}
