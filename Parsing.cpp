#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "Error.h"

#define		MAX_LENGTH_OF_A_WORD				10		//һ�����ʵ�����ַ�����
#define		MAX_NUMBER_OF_WORDS					1000	//��ʶ�����൥�ʸ���

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
	ELSE//ss
};

struct	WORD_STRUCT {//һ�����ʵ����ݽṹ
	char											szName[MAX_LENGTH_OF_A_WORD];//�������ֵ��ַ���
	enum WORD_TYPE_ENUM								eType;//��������ö��ֵ
	char											nIdentifierValue[MAX_LENGTH_OF_A_WORD];//��ʶ�����ʵ�ֵ ss
	char											nReserveValue[MAX_LENGTH_OF_A_WORD];//�����ֱ�ʶ�����ʵ�ֵ ss
	char											nCharValue[5];//��������ʵ�ֵ ss
	int												nNumberValue;//�����ʵ�ֵ
	int												nLineNo;//��Դ�����ļ��е������ڵ�����
};


extern	WORD_STRUCT			g_Words[MAX_NUMBER_OF_WORDS];
extern	int									g_nWordsIndex;
extern	int									g_nInstructsIndex;

int BlockParsing(int nIndentNum);
int OneConstantParsing(int nIndentNum);
int OneVariableParsing(int nIndentNum);
int StatementParsing(int nIndentNum);
int ConditionParsing(int nIndentNum);
int ExpressionParsing(int nIndentNum);
int TermParsing(int nIndentNum);
int FactorParsing(int nIndentNum);
void PrintInParsing(int nWordsIndex,int nIndentNum,const char *pString);

int ParsingAnalysis()//�﷨����
{
	printf("\n----------------Parsing Begin!--------------\n");
	printf("No  Line--(Word)-----\n");

	g_nWordsIndex=0;//g_nWordsIndex����ʶ�𵥴ʵ����

	//�����ɷֳ���Block��ʼ
	PrintInParsing(g_nWordsIndex,0,"BlockParsing");//��ӡ"BlockParsing"
	BlockParsing(1);//"�ֳ���"Block�﷨����

	if(g_Words[g_nWordsIndex].eType==PERIOD)
	{//�ֳ���Block�����Ե�'.'����
		PrintInParsing(g_nWordsIndex,0,"PERIOD");//��ӡ"PERIOD"
		printf("----------------Parsing Successfull Finished!--------------\n");
		return OK;
	}
	else {
		ehandler(NO_PERIOD, g_Words[g_nWordsIndex].nLineNo);
		return ERROR;
	}
}
//"�ֳ���"Block�﷨����
int BlockParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	if(g_Words[g_nWordsIndex].eType==CONST)//���������"const"
	{
		PrintInParsing(g_nWordsIndex,nIndentNum,"CONST");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		PrintInParsing(g_nWordsIndex,nIndentNum,"OneConstantParsing");//��ӡ������Ϣ
		OneConstantParsing(nIndentNum+1);//һ��"��������"�﷨����

		while(g_Words[g_nWordsIndex].eType==COMMA)//��������ж���','
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,"COMMA");//��ӡ������Ϣ

			g_nWordsIndex++;//ȡ��һ������
			PrintInParsing(g_nWordsIndex,nIndentNum,"OneConstantParsing");//��ӡ������Ϣ
			OneConstantParsing(nIndentNum+1);//�����ٴ���һ��"��������"
		}

		if(g_Words[g_nWordsIndex].eType==SEMICOLON)//������������Էֺ�';'����
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,";");//��ӡ������Ϣ
			g_nWordsIndex++;//ȡ��һ������
		}
		else {
			ehandler(NO_SEMICOLON, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;//����,©���˶��Ż��߷ֺ�
		}
	}
	if(g_Words[g_nWordsIndex].eType==VAR)//���������"var"
	{
		PrintInParsing(g_nWordsIndex,nIndentNum,"VAR");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		PrintInParsing(g_nWordsIndex,nIndentNum,"OneVariableParsing");//��ӡ������Ϣ
		OneVariableParsing(nIndentNum+1);//һ��"��������"�﷨����

		while(g_Words[g_nWordsIndex].eType==COMMA)//��������ж���','
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,"COMMA");//��ӡ������Ϣ

			g_nWordsIndex++;//ȡ��һ������
			PrintInParsing(g_nWordsIndex,nIndentNum,"OneVariableParsing");//��ӡ������Ϣ
			OneVariableParsing(nIndentNum+1);//�����ٴ���һ��"��������"
		}

		if(g_Words[g_nWordsIndex].eType==SEMICOLON)//������������Էֺ�';'����
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,";");//��ӡ������Ϣ
			g_nWordsIndex++;//ȡ��һ������
		}
		else {
			ehandler(NO_SEMICOLON, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;
		}
	}
	while (g_Words[g_nWordsIndex].eType==PROCEDURE)//���������"procedure",�����"��������"�﷨����
	{
		PrintInParsing(g_nWordsIndex,nIndentNum,"PROCEDURE");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//"procedure"��ӦΪ��ʶ��
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ
			g_nWordsIndex++;//ȡ��һ������
		}
		else {
			ehandler(NO_IDENTIFIER, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;
		}

		if(g_Words[g_nWordsIndex].eType==SEMICOLON)//��ʶ����Ӧ�Ƿֺ�';'
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,";");//��ӡ������Ϣ
			g_nWordsIndex++;//ȡ��һ������
		}
		else {
			ehandler(NO_SEMICOLON, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;//©���˷ֺ�
		}

		PrintInParsing(g_nWordsIndex,nIndentNum,"BlockParsing");//��ӡ������Ϣ
		BlockParsing(nIndentNum+1);//��"��������"��,�ֺź�����"�ֳ���"Block

		if(g_Words[g_nWordsIndex].eType==SEMICOLON)//"�ֳ���"Block����Ӧ�Ƿֺ�';'
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,";");//��ӡ������Ϣ
			g_nWordsIndex++;//ȡ��һ������
		}
		else{
			ehandler(NO_SEMICOLON, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;//©���˷ֺ�
		}
	}
	
	PrintInParsing(g_nWordsIndex,nIndentNum,"StatementParsing");//��ӡ������Ϣ
	StatementParsing(nIndentNum+1);//"�ֳ���"Block�б�����"���"

	return OK;
}

//һ��"��������"�﷨����
int OneConstantParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//һ�����������Ա�ʶ����ͷ
	{
		PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		if(g_Words[g_nWordsIndex].eType==EQL)//��ʶ������Ӧ��'='
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,"EQL");//��ӡ������Ϣ

			g_nWordsIndex++;//ȡ��һ������
			if(g_Words[g_nWordsIndex].eType==NUMBER)//'='����ĵ���Ӧ����
			{
				PrintInParsing(g_nWordsIndex,nIndentNum,"NUMBER");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������
				return OK;
			}
            else
			{
				ehandler(NO_NUMBER, g_Words[g_nWordsIndex - 1].nLineNo);
				return ERROR;
			}
		}
		else
		{
			ehandler(NO_EQUAL, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;
		}
	}
	else
	{	
		ehandler(NO_IDENTIFIER_IN_STATEMENT, g_Words[g_nWordsIndex - 1].nLineNo);
		return ERROR;
	}

	return ERROR;
}
//һ��"��������"�﷨����
int OneVariableParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//һ�������������Ǳ�ʶ��
	{
		PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ
		g_nWordsIndex++;//ȡ��һ������

		return OK;
	}
	else
	{
		ehandler(NO_IDENTIFIER_IN_STATEMENT, g_Words[g_nWordsIndex - 1].nLineNo);
		return ERROR;
	}

	return ERROR;
}
//һ��"���"�﷨����
int StatementParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	int		nResult;

	switch(g_Words[g_nWordsIndex].eType)//��ǰ����������
	{
	case IDENTIFIER://�����һ�������Ǳ�ʶ��,Ӧ����"��ֵ���"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������
				if(g_Words[g_nWordsIndex].eType==ASSIGN)//��ʶ������Ӧ�Ǹ�ֵ�ŵ���":="
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"ASSIGN");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					PrintInParsing(g_nWordsIndex,nIndentNum,"ExpressionParsing");//��ӡ������Ϣ
					nResult=ExpressionParsing(nIndentNum+1);//��ֵ�ŵ���":="����Ӧ�Ǳ��ʽ

					return nResult;
				}
				else
				{
					ehandler(NO_ASSIGNMENT, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//û�м�⵽':='����
				}

				break;

	case IF://�����һ��������"if",��"�������"���� ssp
				PrintInParsing(g_nWordsIndex, nIndentNum, "IF");//��ӡ������Ϣ

				g_nWordsIndex++;
				PrintInParsing(g_nWordsIndex, nIndentNum, "ConditionParsing");//��ӡ������Ϣ
				ConditionParsing(nIndentNum + 1);//"if"����"����"Condition

				PrintInParsing(g_nWordsIndex, nIndentNum, "StatementParsing");//��ӡ������Ϣ
				nResult = StatementParsing(nIndentNum + 1);//��Ӧ��"���"
				g_nWordsIndex++;//ȡ��һ������

				//����else
				if (g_Words[g_nWordsIndex].eType == ELSE) {
					PrintInParsing(g_nWordsIndex, nIndentNum, "ELSE");//��ӡ������Ϣ
					g_nWordsIndex++;
					PrintInParsing(g_nWordsIndex, nIndentNum, "StatementParsing");//��ӡ������Ϣ
					StatementParsing(nIndentNum + 1);
				}
				g_nWordsIndex++;//ȡ��һ������
				return nResult;
				break;

	case DO:
				PrintInParsing(g_nWordsIndex, nIndentNum, "DO");//��ӡ������Ϣ
				g_nWordsIndex++;//ȡ��һ������

				PrintInParsing(g_nWordsIndex, nIndentNum, "StatementParsing");//��ӡ������Ϣ
				StatementParsing(nIndentNum + 1);
				g_nWordsIndex++; //�ֺ�

				if (g_Words[g_nWordsIndex].eType == WHILE) {
					PrintInParsing(g_nWordsIndex, nIndentNum, "WHILE");//��ӡ������Ϣ
					g_nWordsIndex++;
					PrintInParsing(g_nWordsIndex, nIndentNum, "ConditionParsing");//��ӡ������Ϣ
					ConditionParsing(nIndentNum + 1);
				}
				else {
					ehandler(NO_WHILE_AFTER_DO_STATEMENT, g_Words[g_nWordsIndex].nLineNo);
				}
				return OK;

	case WHILE://�����һ��������"while",׼������"ѭ�����"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"WHILE");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������

				PrintInParsing(g_nWordsIndex,nIndentNum,"ConditionParsing");//��ӡ������Ϣ
				ConditionParsing(nIndentNum+1);//"while"����Ӧ��"����"

				if(g_Words[g_nWordsIndex].eType==DO)//"����"����Ӧ�ǵ���"do"
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"DO");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������

					PrintInParsing(g_nWordsIndex,nIndentNum,"StatementParsing");//��ӡ������Ϣ
					nResult=StatementParsing(nIndentNum+1);//"do"����Ӧ��"һ�����"

					return nResult;
				}
				else
				{	
					ehandler(NO_DO, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//ȱ��do
				}

				break;

	case BEGIN://�����һ��������"begin",׼������"�������"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"BEGIN");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������

				PrintInParsing(g_nWordsIndex,nIndentNum,"StatementParsing");//��ӡ������Ϣ
				StatementParsing(nIndentNum+1);//"begin"����Ӧ��һ�����

				while(g_Words[g_nWordsIndex].eType==SEMICOLON)//ֻҪ�����滹��';'
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,";");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������

					PrintInParsing(g_nWordsIndex,nIndentNum,"StatementParsing");//��ӡ������Ϣ
					StatementParsing(nIndentNum+1);//";"����Ӧ��"һ�����"
                }

				if(g_Words[g_nWordsIndex].eType==END)//���������������"end"
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"END");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					return OK;
				}
				else
				{
					ehandler(NO_END, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//ȱ��end��ֺ�
				}

				break;

	case READ://�����һ��������"read",׼������"�����"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"READ");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������
				if(g_Words[g_nWordsIndex].eType==LEFT_PARENTHESIS)//"read"����Ӧ��������'('
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"LEFT_PARENTHESIS");//��ӡ������Ϣ
					do{//һ��"�����"Ӧ������һ��������ʶ��
						g_nWordsIndex++;//ȡ��һ������

						if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//����Ӧ��һ����ʶ��
						{
							PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ
							g_nWordsIndex++;//ȡ��һ������
						}
						else
						{	
							ehandler(NO_IDENTIFIER_IN_STATEMENT, g_Words[g_nWordsIndex - 1].nLineNo);
							return ERROR;//��ʽ����,
						}

						if(g_Words[g_nWordsIndex].eType==COMMA)//��ӡ������Ϣ
							PrintInParsing(g_nWordsIndex,nIndentNum,"COMMA");
					}while(g_Words[g_nWordsIndex].eType==COMMA);//�����ʶ�����滹��һ������','

					if(g_Words[g_nWordsIndex].eType==RIGHT_PARENTHESIS)//���Ӧ��������')'
					{
						PrintInParsing(g_nWordsIndex,nIndentNum,"RIGHT_PARENTHESIS");//��ӡ������Ϣ
						g_nWordsIndex++;//ȡ��һ������
						return OK;
					}
					else
					{	
						ehandler(NO_RIGHT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
						return ERROR;//��ʽ����,Ӧ��������
					}
				}
				else
				{
					ehandler(NO_LEFT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//��ʽ����,Ӧ��������
				}

				break;

	case WRITE://�����һ��������"write",׼������"д���"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"WRITE");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������
				if(g_Words[g_nWordsIndex].eType==LEFT_PARENTHESIS)//"write"����Ӧ��������'('
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"(");//��ӡ������Ϣ

					do{//һ��"д���"Ӧ������һ������������ʶ��
						g_nWordsIndex++;//ȡ��һ������
						if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//����Ӧ��һ����ʶ��
						{
							PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ
							g_nWordsIndex++;//ȡ��һ������
						}
						else
						{
							ehandler(NO_IDENTIFIER_IN_STATEMENT, g_Words[g_nWordsIndex - 1].nLineNo);
							return ERROR;//��ʽ����,
						}
						if(g_Words[g_nWordsIndex].eType==COMMA)//��ӡ������Ϣ
							PrintInParsing(g_nWordsIndex,nIndentNum,"COMMA");
					}while(g_Words[g_nWordsIndex].eType==COMMA);//�����ʶ��������һ������','

					if(g_Words[g_nWordsIndex].eType==RIGHT_PARENTHESIS)//���Ӧ��������')'
					{
						PrintInParsing(g_nWordsIndex,nIndentNum,")");//��ӡ������Ϣ

						g_nWordsIndex++;//ȡ��һ������
						return OK;
					}
					else
					{
						ehandler(NO_RIGHT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
						return ERROR;//��ʽ����,Ӧ��������
					}
				}
				else
				{
					ehandler(NO_LEFT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//��ʽ����,Ӧ��������
				}
				break;

	case CALL://�����һ��������"call",׼������"���ú������"����
				PrintInParsing(g_nWordsIndex,nIndentNum,"CALL");//��ӡ������Ϣ

				g_nWordsIndex++;//ȡ��һ������
				if(g_Words[g_nWordsIndex].eType==IDENTIFIER)//"call"����Ӧ��һ����ʶ��,������
				{
					PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					return OK;
				}
				else
				{
					ehandler(NO_IDENTIFIER_IN_STATEMENT, g_Words[g_nWordsIndex - 1].nLineNo);
					return ERROR;//��ʽ����,
				}

	default:	return ERROR;
	}

	return ERROR;
}
//"����"�﷨���� ss Ҫ���������ǰ�����������
int ConditionParsing(int nIndentNum){//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���

	if (g_Words[g_nWordsIndex].eType == LEFT_PARENTHESIS) { //�������ǰӦ��������
		PrintInParsing(g_nWordsIndex, nIndentNum, "LEFT_PARENTHESIS");//��ӡ������Ϣ
	}
	else {
		ehandler(NO_LEFT_PARENTHESIS, g_Words[g_nWordsIndex].nLineNo);
	}

	g_nWordsIndex++;


	if(g_Words[g_nWordsIndex].eType==ODD){//�����"odd"����
		PrintInParsing(g_nWordsIndex,nIndentNum,"ODD");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������

		PrintInParsing(g_nWordsIndex,nIndentNum,"ExpressionParsing");//��ӡ������Ϣ
		ExpressionParsing(nIndentNum+1);//"odd"���ʺ���Ӧ��һ��"���ʽ"
	}else{
		PrintInParsing(g_nWordsIndex,nIndentNum,"ExpressionParsing");//��ӡ������Ϣ
		ExpressionParsing(nIndentNum+1);//����������һ��"���ʽ"

		if(		g_Words[g_nWordsIndex].eType==EQL		//����������߼������
			||	g_Words[g_nWordsIndex].eType==NEQ
			||	g_Words[g_nWordsIndex].eType==LES 
			||	g_Words[g_nWordsIndex].eType==LEQ
			||	g_Words[g_nWordsIndex].eType==GTR
			||	g_Words[g_nWordsIndex].eType==GEQ)
		{
			PrintInParsing(g_nWordsIndex,nIndentNum,"LOGICAL_OP");//��ӡ������Ϣ

			g_nWordsIndex++;//ȡ��һ������
			PrintInParsing(g_nWordsIndex,nIndentNum,"ExpressionParsing");//��ӡ������Ϣ
			ExpressionParsing(nIndentNum+1);//������������ڶ���"���ʽ"
		}
		else
		{
			ehandler(NO_RIGHT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
			return ERROR;//��ʽ����,Ӧ��������
		}
	}

	if (g_Words[g_nWordsIndex].eType == RIGHT_PARENTHESIS) {  //��������Ӧ��������
		PrintInParsing(g_nWordsIndex, nIndentNum, "RIGHT_PARENTHESIS");//��ӡ������Ϣ
		
	}
	else {
		ehandler(NO_RIGHT_PARENTHESIS, g_Words[g_nWordsIndex].nLineNo);
	}
	g_nWordsIndex++;

	return OK;
}
//"���ʽ"�﷨����
int ExpressionParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	if(g_Words[g_nWordsIndex].eType==PLUS || g_Words[g_nWordsIndex].eType==MINUS)
	{//�����ͷ��������,���ʱ���ʽӦ������һ�����Ļ򸺵���
		PrintInParsing(g_nWordsIndex,nIndentNum,"PLUS/MINUS");//��ӡ������Ϣ
		g_nWordsIndex++;//ȡ��һ������
	}

	PrintInParsing(g_nWordsIndex,nIndentNum,"TermParsing");//��ӡ������Ϣ
	TermParsing(nIndentNum+1);//���ʽ������һ��"��"

	while(g_Words[g_nWordsIndex].eType==PLUS || g_Words[g_nWordsIndex].eType==MINUS)
	{//���"��"���滹��'+'��'-'
		PrintInParsing(g_nWordsIndex,nIndentNum,"PLUS/MINUS");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		PrintInParsing(g_nWordsIndex,nIndentNum,"TermParsing");//��ӡ������Ϣ
		TermParsing(nIndentNum+1);//����滹��"��"
	}
	return OK;
}
//"��"�﷨����
int TermParsing(int nIndentNum)//nIndentNum�Ǵ�ӡʱҪ�����Ŀո���
{
	PrintInParsing(g_nWordsIndex,nIndentNum,"FactorParsing");//��ӡ������Ϣ
	FactorParsing(nIndentNum+1);//"��"������һ��"����"

	while(g_Words[g_nWordsIndex].eType==MULTIPLY || g_Words[g_nWordsIndex].eType==DIVIDE)
	{//���"����"���滹�е���'*'��'/'
		PrintInParsing(g_nWordsIndex,nIndentNum,"MULTI/DIVISION");//��ӡ������Ϣ

		g_nWordsIndex++;//ȡ��һ������
		PrintInParsing(g_nWordsIndex,nIndentNum,"FactorParsing");//��ӡ������Ϣ
		FactorParsing(nIndentNum+1);//����滹��"����"
	}
	return OK;
}
//"����"�﷨����
int FactorParsing(int nIndentNum)
{
	switch(g_Words[g_nWordsIndex].eType)
	{
	case IDENTIFIER://���ӿ�����һ�����������
					PrintInParsing(g_nWordsIndex,nIndentNum,"IDENTIFIER");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					return OK;

	case NUMBER:	//���ӿ�����һ����
					PrintInParsing(g_nWordsIndex,nIndentNum,"NUMBER");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					return OK;

	case LEFT_PARENTHESIS:	//�������������'('
					PrintInParsing(g_nWordsIndex,nIndentNum,"(");//��ӡ������Ϣ

					g_nWordsIndex++;//ȡ��һ������
					PrintInParsing(g_nWordsIndex,nIndentNum,"ExpressionParsing");//��ӡ������Ϣ
					ExpressionParsing(nIndentNum+1);//������'('����Ӧ��"���ʽ"

					if(g_Words[g_nWordsIndex].eType==RIGHT_PARENTHESIS)//"���ʽ"����Ӧ��������')'
					{
						PrintInParsing(g_nWordsIndex,nIndentNum,")");//��ӡ������Ϣ

						g_nWordsIndex++;//ȡ��һ������
						return OK;
					}
					else
					{
						ehandler(NO_RIGHT_PARENTHESIS, g_Words[g_nWordsIndex - 1].nLineNo);
						return ERROR;//��ʽ����,Ӧ��������
					}
					break;

	default:		return ERROR;
	}
	return OK;
}
void PrintInParsing(int nWordsIndex,int nIndentNum,const char *pString)//��ӡ������Ϣ
{

	int		i;
	char	strText[100];

	printf("%-4d%-4d%-12s",nWordsIndex+1,g_Words[nWordsIndex].nLineNo,g_Words[nWordsIndex].szName);
	printf("%-1s","|");
	for (i=0;i<nIndentNum;i++)
		printf("  ");
	printf("%-1s"," ");

	sprintf(strText,"(%d)%s",nIndentNum,pString);
	printf("%-15s\n",strText);

}

