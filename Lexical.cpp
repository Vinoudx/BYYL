#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Error.h"

#define PASS_PRINTINLEXIS 0 //ss ע��ʱ�����������

enum	WORD_TYPE_ENUM {//��������ö��ֵ
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
//�����ֵ������ַ��������Ͷ��ձ�ṹ
struct	RESERVED_WORD_NAME_VS_TYPE_STRUCT {
	char											szName[20];//�����ֵ������ַ���
	enum	WORD_TYPE_ENUM							eType;//�����ֵĵ�������ö��ֵ
};

#define		MAX_LENGTH_OF_A_WORD								10		//һ�����ʵ�����ַ�����
#define		MAX_NUMBER_OF_WORDS									1000	//��ʶ�����൥�ʸ���
#define		NUMBER_OF_RESERVED_WORDS							18		//�����ָ��� ss

struct	WORD_STRUCT {//һ�����ʵ����ݽṹ
	char											szName[MAX_LENGTH_OF_A_WORD];//�������ֵ��ַ���
	enum WORD_TYPE_ENUM								eType;//��������ö��ֵ
	char											nIdentifierValue[MAX_LENGTH_OF_A_WORD];//��ʶ�����ʵ�ֵ ss
	char											nReserveValue[MAX_LENGTH_OF_A_WORD];//�����ֱ�ʶ�����ʵ�ֵ ss
	char											nCharValue[5];//��������ʵ�ֵ ss
	int												nNumberValue;//�����ʵ�ֵ
	int												nLineNo;//��Դ�����ļ��е������ڵ�����
};

RESERVED_WORD_NAME_VS_TYPE_STRUCT				ReservedWordNameVsTypeTable[NUMBER_OF_RESERVED_WORDS];//�����ֵ������ַ��������Ͷ��ձ�
WORD_TYPE_ENUM													SingleCharacterWordTypeTable[256];//���ַ����ʵ��ַ������Ͷ��ձ�
WORD_STRUCT							g_Words[MAX_NUMBER_OF_WORDS];//��ʶ����ĵ��ʶ���
int													g_nWordsIndex;//��ʶ����ĵ��ʵĸ��������
int													g_nLineNo = 1;//�ļ���Դ���������

extern	FILE* fpSourceFile;//Դ�����ļ����ļ�ָ��

int	WordTypeToString(char string[100], WORD_TYPE_ENUM eWordType);
void PrintInLexis(int nWordsIndex);
int	GetAWord();

void	InitializeReservedWordTable()//���ñ����ֵ��ʵ������ַ�������Ӧ���͵Ķ��ձ�
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

void InitializeSingleCharacterTable()//���õ��ַ����ʵ��ַ�����Ӧ���͵Ķ��ձ�
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

//�ʷ�����
void LexicalAnalysis()
{
	int		nResult;

	InitializeReservedWordTable();//���ñ����ֵ��ʵ������ַ�������Ӧ���͵Ķ��ձ�
	InitializeSingleCharacterTable();//���õ��ַ����ʵ��ַ�����Ӧ���͵Ķ��ձ�
	fseek(fpSourceFile, 0L, SEEK_SET);//Դ�����ļ����ļ�ָ��ָ����ʼλ��

	printf("\n----------------Lexical Analysis Begin!--------------\n");

	g_nWordsIndex = 0;//g_nWordsIndex����ʶ�𵥴ʵ����

	nResult = GetAWord();//�ʷ�����,ʶ���һ������
	while ((nResult == OK || nResult == PASS_PRINTINLEXIS) && g_nWordsIndex >= 1)
	{
		if (nResult != PASS_PRINTINLEXIS) {
			PrintInLexis(g_nWordsIndex - 1);//��ӡһ������
		}
		nResult = GetAWord();//�ʷ�����,ʶ����һ������
	}

	printf("----------------Lexical Analysis Finished!--------------\n");
}
//���ļ��ж�ȡһ���ַ�Ȼ�󷵻�
char GetACharacterFromFile()
{
	char	cACharacter;

	//���ļ��ж�ȡ��һ���ַ�
	if (fscanf(fpSourceFile, "%c", &cACharacter) == EOF)
		cACharacter = EOF;

	if (cACharacter == RETURN)//����ǻس���
		g_nLineNo++;//Դ����������һ

	return cACharacter;//���ش��ļ��ж�ȡ��һ���ַ�
}

int GetAWord()//�ʷ�����,��ȡһ������
{
	static		char	cACharacter = SPACE;//��ǰ�ַ�
	int			nAWordIndex;//���������±�,��ʾ���ʵڼ����ַ�
	int			nDigitNumber;//�����ʵ��ַ�����
	int			i;
	char		szAWord[MAX_LENGTH_OF_A_WORD + 1];
	int			nNumberValue;	//����ֵ

	//���Կո񡢻��к�TAB
	while ((cACharacter == SPACE || cACharacter == RETURN || cACharacter == TABLE) && cACharacter != EOF)
		cACharacter = GetACharacterFromFile();//���ļ��ж�ȡһ���ַ�Ȼ�󷵻�

	if (cACharacter != EOF)//��������ļ�ĩβ
	{
		if (cACharacter >= 'a' && cACharacter <= 'z')//�����ǰ�ַ���Сд����ĸ
		{//��ʶ����������a..z��ͷ
			nAWordIndex = 0;//���ʵĵ�һ���ַ�
			do {
				if (nAWordIndex < MAX_LENGTH_OF_A_WORD)
					szAWord[nAWordIndex++] = cACharacter;//�ѵ�ǰ�ַ��Ž�����������,���������±��һ

				cACharacter = GetACharacterFromFile();//���ļ����ٶ�ȡһ���ַ�
			} while ((cACharacter >= 'a' && cACharacter <= 'z' || cACharacter >= '0' && cACharacter <= '9' || cACharacter == '_') && cACharacter != EOF);
			//ֻҪ���������a..z��0..9,��Ҫ������������ַ� ss ��ʶ�����Ժ���'_'

			//��ǰ�ַ�������a..z��0..9
			if (cACharacter != EOF)//��������ļ�ĩβ
			{
				szAWord[nAWordIndex] = 0;//��0��β,�ڵ������������һ�������ַ���

				for (i = 0; i < NUMBER_OF_RESERVED_WORDS; i++)//�ӱ����ֱ��в�ѯ��ǰ�����ַ����Ƿ�Ϊĳһ�������ַ���
					if (strcmp(szAWord, ReservedWordNameVsTypeTable[i].szName) == 0)
					{
						//����Ǳ�����,�򸳵�ǰ��������Ϊ��Ӧ�����ֵĵ�������ö��ֵ
						g_Words[g_nWordsIndex].eType = ReservedWordNameVsTypeTable[i].eType;
						strcpy(g_Words[g_nWordsIndex].nReserveValue, szAWord);//�������ֵ�ֵ���� ss
						break;
					}
				if (i >= NUMBER_OF_RESERVED_WORDS) {
					//����ڱ����ֱ��в�ѯ����,��ǰ�����Ǳ�ʶ��,��������Ϊ��������ö��ֵIDENTIFIER
					g_Words[g_nWordsIndex].eType = IDENTIFIER;
					strcpy(g_Words[g_nWordsIndex].nIdentifierValue, szAWord);//����ʶ����ֵ���� ss
				}
				strcpy(g_Words[g_nWordsIndex].szName, szAWord);//ʶ����ĵ��ʷŽ����ʶ���g_Words��
				g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����

				g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

				return OK;
			}
			else

				return ERROR;
		}
		else
			if (cACharacter >= '0' && cACharacter <= '9')
			{//��������0..9��ͷ
				nDigitNumber = 0;//�����ʵ��ַ�����
				nNumberValue = 0;//�����ʵ�ֵ
				do {
					szAWord[nDigitNumber++] = cACharacter;
					nNumberValue = 10 * nNumberValue + cACharacter - '0';//���������ʵ�ֵ

					cACharacter = GetACharacterFromFile();//��ȡ��һ���ַ�
				} while (cACharacter >= '0' && cACharacter <= '9' && cACharacter != EOF);//�����0..9,�����ƴװ������

				//�������0..9�������ʽ���
				if (cACharacter != EOF)
				{
					szAWord[nDigitNumber] = 0;//��0��β�ڵ������������һ���������ַ���

					g_Words[g_nWordsIndex].eType = NUMBER;//��������Ϊ��������ö��ֵNUMBER
					strcpy(g_Words[g_nWordsIndex].szName, szAWord);//ʶ����ĵ��ʷŽ����ʶ���g_Words��
					g_Words[g_nWordsIndex].nNumberValue = nNumberValue;//ͬʱ������ֵ�������ʵ�nNumberValue
					g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����

					g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

					return OK;
				}
				else
					return ERROR;
			}
			else
				if (cACharacter == ':')//��ǰ������':'��
				{
					cACharacter = GetACharacterFromFile();//�����ǰ������':',��Ҫ�ٿ���һ������
					if (cACharacter == '=')//�����һ��������'='
					{//�򵥴��Ǹ�ֵ����":="
						g_Words[g_nWordsIndex].eType = ASSIGN;//��������Ϊ��������ö��ֵASSIGN
						strcpy(g_Words[g_nWordsIndex].szName, ":=");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
						g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����
						strcpy(g_Words[g_nWordsIndex].nCharValue, ":="); //ss
						g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

						cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��

						return OK;
					}
					else {
						g_Words[g_nWordsIndex].eType = COLON;//��������Ϊ��������ö��ֵASSIGN
						strcpy(g_Words[g_nWordsIndex].szName, ":");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
						g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����
						strcpy(g_Words[g_nWordsIndex].nCharValue, ":"); //ss
						g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

						cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��

						return OK;
					}
				}
				else
					if (cACharacter == '<')//�����"<"����"<="����?
					{
						cACharacter = GetACharacterFromFile();//��ȡһ������
						if (cACharacter == '=')
						{//������"<="
							g_Words[g_nWordsIndex].eType = LEQ;//��������Ϊ��������ö��ֵLEQ
							strcpy(g_Words[g_nWordsIndex].szName, "<=");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����
							strcpy(g_Words[g_nWordsIndex].nCharValue, "<="); //ss
							g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

							cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��

							return OK;
						}
						else
						{//���ʽ���"<"
							g_Words[g_nWordsIndex].eType = LES;//��������Ϊ��������ö��ֵLES
							strcpy(g_Words[g_nWordsIndex].szName, "<");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;//��Դ�����ļ��е������ڵ�����
							strcpy(g_Words[g_nWordsIndex].nCharValue, "<"); //ss
							g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

							return OK;
						}
					}
					else
						if (cACharacter == '>')//�����">"����">="����?
						{
							cACharacter = GetACharacterFromFile();//��ȡһ������
							if (cACharacter == '=')
							{//������">="
								g_Words[g_nWordsIndex].eType = GEQ;//��������Ϊ��������ö��ֵGEQ
								strcpy(g_Words[g_nWordsIndex].szName, ">=");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, ">="); //ss
								g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

								cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��
							}
							else
							{//���ʽ���">"
								g_Words[g_nWordsIndex].eType = GTR;//��������Ϊ��������ö��ֵGTR
								strcpy(g_Words[g_nWordsIndex].szName, ">");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, ">"); //ss
								g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ
							}
							return OK;
						}
						else if (cACharacter == '/') {// ����ǡ�/�����ǡ�/**/�� ss
							cACharacter = GetACharacterFromFile();
							if (cACharacter == '*') {//��ע��
								while (true) {
									cACharacter = GetACharacterFromFile();
									if (cACharacter == '*') {
										cACharacter = GetACharacterFromFile();
										if (cACharacter == '/') {
											break;
										}
									}
								}
								cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��
								return PASS_PRINTINLEXIS;
							}
							else { //�ǳ���
								g_Words[g_nWordsIndex].eType = DIVIDE;//��������Ϊ��������ö��ֵGTR
								strcpy(g_Words[g_nWordsIndex].szName, "/");//ʶ����ĵ��ʷŽ����ʶ���g_Words��
								g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
								strcpy(g_Words[g_nWordsIndex].nCharValue, "/");
								g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ
							}
							return OK;
						}
						else
						{
							//����������������ʱ,���ǵ��ַ�
							//ͨ�����Ѱ�ҵ��ַ��ĵ�������ö��ֵ
							g_Words[g_nWordsIndex].eType = SingleCharacterWordTypeTable[cACharacter];
							g_Words[g_nWordsIndex].szName[0] = cACharacter;//���ʷŽ����ʶ���g_Words��
							g_Words[g_nWordsIndex].szName[1] = 0;//��0��β���һ���ַ���
							g_Words[g_nWordsIndex].nLineNo = g_nLineNo;
							g_Words[g_nWordsIndex].nCharValue[0] = cACharacter; //ss
							g_nWordsIndex++;//ʶ����ĵ��ʸ�����һ

							cACharacter = GetACharacterFromFile();//��ȡһ������,Ϊ��һ��׼��

							return OK;
						}
	}
	return ERROR;
}

void PrintInLexis(int nIndex)//��ӡ���ʶ����е�һ������
{
	char	szWordName[100];
	char	szWordType[100];

	strcpy(szWordName, g_Words[nIndex].szName);
	WordTypeToString(szWordType, g_Words[nIndex].eType);//���������͵�ö��ֵת�����ַ���

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
//���������͵�ö��ֵת�����ַ���
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
