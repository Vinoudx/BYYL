#define _CRT_SECURE_NO_WARNINGS
#include <fstream>
#include "stdafx.h"
#include "Error.h"



#define		MAX_LENGTH_OF_A_WORD				10		//一个单词的最多字符个数
#define		MAX_NUMBER_OF_WORDS					1000	//可识别的最多单词个数
#define		MAX_NUMBER_INSTRUCTS					200		//最多的虚拟机指令数
#define		MAX_NUMBER_NAME_TABLE				100		//名字表中的长度
#define		MAX_CASES							100     //一个switch中case的最大个数

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


struct	WORD_STRUCT {//一个单词的数据结构
	char											szName[MAX_LENGTH_OF_A_WORD];//单词名字的字符串
	enum WORD_TYPE_ENUM								eType;//单词类型枚举值
	char											nIdentifierValue[MAX_LENGTH_OF_A_WORD];//标识符单词的值 ss
	char											nReserveValue[MAX_LENGTH_OF_A_WORD];//保留字标识符单词的值 ss
	char											nCharValue[5];//运算符单词的值 ss
	int												nNumberValue;//数单词的值
	int												nLineNo;//在源代码文件中单词所在的行数
};


enum	TYPE_ENUM {
	CONSTANT_TYPE,
	VARIABLE_TYPE,
	PROCEDURE_TYPE,
};

//名字表结构
struct TABLE_STRUCT
{
	char											szName[MAX_LENGTH_OF_A_WORD];	//名字字符串
	enum TYPE_ENUM				eType;					//类型,const, var, array or procedure
	int											nVal;					//单词的数值
	int											nLevel;					//所处层,仅const不使用
	int											nAddress;			//地址,仅const不使用
	int											nSize;					//需要分配的数据区空间,仅procedure使用
};
TABLE_STRUCT				g_NameTable[MAX_NUMBER_NAME_TABLE];//名字表
int										g_nNameTableIndex;//名字表下标

//虚拟机指令操作码
enum	INSTRUCT_ENUM {
	JMP_INSTRUCT,				//直接跳转到地址为操作数的指令
	JPC_INSTRUCT,				//条件跳转.如果栈顶数据等于零,则跳转执行地址为操作数的指令,否则执行下一条指令
	ALLOCATE_INSTRUCT,	//分配内存,栈顶指针增加操作数
	INSTANT_INSTRUCT,		//直接将操作数的值压入栈顶
	LOAD_INSTRUCT,			//取某层过程的相对地址为操作数的内存的值到栈顶
	STORE_INSTRUCT,			//把栈顶数据存到某层过程的相对地址为操作数的内存中
	CALL_INSTRUCT,				//调用过程,返回地址压栈
	ENDCALL_INSTRUCT,		//结束过程,退栈,返回到原调用过程处
	NEGATIVE_INSTRUCT,	//取负操作,将栈顶数据取负后仍放在栈顶
	ADD_INSTRUCT,				//加法指令,取出栈顶数据和次栈顶数据的和再放到栈顶
	SUB_INSTRUCT,				//减法指令,取出栈顶数据和次栈顶数据的差再放到栈顶
	MULTI_INSTRUCT,			//相乘指令,取出栈顶数据和次栈顶数据的积再放到栈顶
	DIVID_INSTRUCT,			//相除指令,取出栈顶数据和次栈顶数据的商再放到栈顶
	ODD_INSTRUCT,				//判断栈顶数据是否为奇数.如果是,则栈顶数据为1,否则为0
	EQL_INSTRUCT,				//取出并判断栈顶数据和次栈顶数据是否相同.如果相同,则栈顶数据为1,否则为0
	NEQ_INSTRUCT,				//取出并判断栈顶数据和次栈顶数据是否不相同.如果相同,则栈顶数据为0,否则为1
	LES_INSTRUCT,				//取出并判断次栈顶数据是否小于栈顶数据.如果是,则栈顶数据为1,否则为0
	LEQ_INSTRUCT,				//取出并判断次栈顶数据是否小于等于栈顶数据.如果是,则栈顶数据为1,否则为0
	GTR_INSTRUCT,				//取出并判断次栈顶数据是否大于栈顶数据.如果是,则栈顶数据为1,否则为0
	GEQ_INSTRUCT,				//取出并判断次栈顶数据是否大于等于栈顶数据.如果是,则栈顶数据为1,否则为0个
	OUT_INSTRUCT,				//生成输出指令,输出栈顶数据
	IN_INSTRUCT,					//生成输人指令,读取数据到栈顶
};
//虚拟机指令结构
struct INSTRUCT_STRUCT
{
	enum INSTRUCT_ENUM		eInstruct;		//操作码
	int											nLevel;			//引用层与声明层的层次差
	int											nOperand;	//操作数
};
INSTRUCT_STRUCT			g_Instructs[MAX_NUMBER_INSTRUCTS];//生成后的代码队列
int										g_nInstructsIndex;//虚拟机指令指针,取值范围[0,MAX_NUMBER_INSTRUCTS-1]



struct SWITCH_STRUCT 
{	
	bool has_default = false;
	int default_pos;
	int num_case = 0;
	int case_list[MAX_CASES];
	int case_condition[MAX_CASES];
	int break_list[MAX_CASES] = { 0 };
};


void InitializeInstructs();
void InitializeNameTable();
int BlockGenerate(int nLevel, int nIndentNum);
int OneConstantGenerate(int nLevel, int* pAddressInCurrentLevel, int nIndentNum);
int OneVariableGenerate(int nLevel, int* pAddressInCurrentLevel, int nIndentNum);
int StatementGenerate(int nLevel, int nIndentNum);
int ConditionGenerate(int nLevel, int nIndentNum);
int ExpressionGenerate(int nLevel, int nIndentNum);
int TermGenerate(int nLevel, int nIndentNum);
int FactorGenerate(int nLevel, int nIndentNum);
void PrintInGenerate(int nWordsIndex, int nIndentNum, const char* pString);
int InstructFromEnumToString(enum INSTRUCT_ENUM eInstruct, char* szString);
int GenerateOneInstruction(INSTRUCT_ENUM eInstruct, int nLevel, int nAddress);
int GenerateOneInstruction(INSTRUCT_ENUM eInstruct, int nLevel, int nAddress, int pos);
void PrintAllInstructions();
void PrintAVariable(int nNameTableIndex);
void RegisterInNameTable(enum TYPE_ENUM eType, char* szName, int nNumberValue, int* pAddressInThisLevel, int nLevel);

extern	WORD_STRUCT			g_Words[MAX_NUMBER_OF_WORDS];
extern	int									g_nWordsIndex;
extern	int									g_nInstructsIndex;

int GenerateAnalysis()
{
	InitializeInstructs();
	InitializeNameTable();

	printf("\n----------------Generate Begin!--------------\n");
	printf("No  Line--(Word)-----\t\t\t        Index Instructs  Level Address\n");

	g_nWordsIndex = 0;
	g_nNameTableIndex = 0;
	g_nInstructsIndex = 0;//第一条指令放在aInstructs[0]中

	PrintInGenerate(g_nWordsIndex, 0, "Block");//打印调试信息
	BlockGenerate(0, 1);//程序由分程序Block开始

	if (g_Words[g_nWordsIndex].eType == PERIOD)//后面跟'.'结束
	{
		PrintInGenerate(g_nWordsIndex, 0, "PERIOD");//打印调试信息
		printf("\n----------------Generate Successfull Finished!--------------\n");

		PrintAllInstructions();//最后打印所有生成的指令

		return OK;
	}
	else
		return ERROR;
}

void InitializeInstructs()//初始化代码队列
{
	int i;

	for (i = 0; i < MAX_NUMBER_INSTRUCTS; i++)
	{
		g_Instructs[i].eInstruct = (INSTRUCT_ENUM)-1;
		g_Instructs[i].nLevel = -1;
		g_Instructs[i].nOperand = -1;
	}
}

void InitializeNameTable()//初始化名字表
{
	int	i;

	for (i = 0; i < MAX_NUMBER_NAME_TABLE; i++)
	{
		g_NameTable[i].eType = (TYPE_ENUM)-1;
		g_NameTable[i].nAddress = -1;
		g_NameTable[i].nLevel = -1;
		g_NameTable[i].nSize = -1;
		g_NameTable[i].nVal = -1;
		g_NameTable[i].szName[0] = '\0';
	}
}
//"分程序"Block代码生成
int BlockGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	int		nCurrentLevelAddress;
	int		nCurrentLevelFirstInstructAddress;//名字分配到的相对地址
	int		nCurrentLevelNameTableFirstIndex;//保存本层名字表中的初始位置

	nCurrentLevelAddress = 3;//本层指令偏移地址,每层"分程序"(Block)的本层指令偏移地址从3开始
	nCurrentLevelNameTableFirstIndex = g_nNameTableIndex;//保存名字表中本层名字的初始位置
	g_NameTable[g_nNameTableIndex].nAddress = g_nInstructsIndex;//在名字表中记录本层指令的初始位置

	GenerateOneInstruction(JMP_INSTRUCT, 0, 0);//产生JMP跳转到本层第1条指令

	if (g_Words[g_nWordsIndex].eType == CONST)//如果当前单词是"const"
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "CONST");//打印调试信息

		g_nWordsIndex++;//取下一个单词
		PrintInGenerate(g_nWordsIndex, nIndentNum, "OneConstant");//打印调试信息
		OneConstantGenerate(nLevel, &nCurrentLevelAddress, nIndentNum + 1);//后面至少有一个"常量声明"

		while (g_Words[g_nWordsIndex].eType == COMMA)//如果后面有逗号','
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "COMMA");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			PrintInGenerate(g_nWordsIndex, nIndentNum, "OneConstant");//打印调试信息
			OneConstantGenerate(nLevel, &nCurrentLevelAddress, nIndentNum + 1);//必须再处理一个常量声明
		}

		if (g_Words[g_nWordsIndex].eType == SEMICOLON)//常量声明最后以分号';'结束
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ";");//打印调试信息
			g_nWordsIndex++;//取下一个单词
		}
		else
			return ERROR;//错误,漏掉了逗号或者分号
	}
	if (g_Words[g_nWordsIndex].eType == VAR)//如果当前单词是"var"
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "VAR");//打印调试信息

		g_nWordsIndex++;//取下一个单词
		PrintInGenerate(g_nWordsIndex, nIndentNum, "OneVariable");//打印调试信息
		OneVariableGenerate(nLevel, &nCurrentLevelAddress, nIndentNum + 1);//后面至少有一个"变量声明"

		while (g_Words[g_nWordsIndex].eType == COMMA)//如果后面有逗号','
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "COMMA");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			PrintInGenerate(g_nWordsIndex, nIndentNum, "OneVariable");//打印调试信息
			OneVariableGenerate(nLevel, &nCurrentLevelAddress, nIndentNum + 1);//必须再处理一个"变量声明"
		}

		if (g_Words[g_nWordsIndex].eType == SEMICOLON)//变量声明最后以分号';'结束
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ";");//打印调试信息
			g_nWordsIndex++;//取下一个单词
		}
		else
			return ERROR;
	}
	while (g_Words[g_nWordsIndex].eType == PROCEDURE)//如果当前单词是"procedure",则进入"函数定义"代码生成
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "PROCEDURE");//打印调试信息

		g_nWordsIndex++;//取下一个单词
		if (g_Words[g_nWordsIndex].eType == IDENTIFIER)//"procedure"后应为标识符
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

			RegisterInNameTable(PROCEDURE_TYPE, g_Words[g_nWordsIndex].szName, -1, (int*)-1, nLevel);//登记过程中的名字
			g_nWordsIndex++;//取下一个单词
		}
		else
			return ERROR;

		if (g_Words[g_nWordsIndex].eType == SEMICOLON)//标识符后应为分号';'
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ";");//打印调试信息
			g_nWordsIndex++;//取下一个单词
		}
		else
			return ERROR;//漏掉了分号

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Block");//打印调试信息
		BlockGenerate(nLevel + 1, nIndentNum + 1);//在"函数定义"中,分号后面是"分程序"Block

		if (g_Words[g_nWordsIndex].eType == SEMICOLON)//"分程序"Block后面应是分号';'
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ";");//打印调试信息
			g_nWordsIndex++;//取下一个单词
		}
		else
			return ERROR;//漏掉了分号
	}

	//开始生成当前过程指令
	//取出本层第1条指令的地址,即JMP_INSTRUCT 0 0指令的地址
	nCurrentLevelFirstInstructAddress = g_NameTable[nCurrentLevelNameTableFirstIndex].nAddress;
	//将本层第1条指令,即JMP_INSTRUCT 0 0指令修改为JMP_INSTRUCT 0 g_nInstructsIndex,跳转到当前新指令
	g_Instructs[nCurrentLevelFirstInstructAddress].nOperand = g_nInstructsIndex;

	//登记当前过程第一条指令地址
	g_NameTable[nCurrentLevelNameTableFirstIndex].nAddress = g_nInstructsIndex;
	//前面变量声明部分中每增加一条变量声明都会给nCurrentLevelAddress增加l,
	//所以现在nCurrentLevelAddress就是当前过程数据的nSize
	g_NameTable[nCurrentLevelNameTableFirstIndex].nSize = nCurrentLevelAddress + 5;

	GenerateOneInstruction(ALLOCATE_INSTRUCT, 0, nCurrentLevelAddress + 5);//生成分配内存指令

	PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
	StatementGenerate(nLevel, nIndentNum + 1);//"分程序"Block中必须有"语句"

	GenerateOneInstruction(ENDCALL_INSTRUCT, 0, 0);//每个过程出口都要使用的释放数据段指令

	return OK;
}
//在名字表中登记单词
void RegisterInNameTable(enum TYPE_ENUM eType, char* szName, int nNumberValue, int* pAddressInThisLevel, int nLevel)
{
	g_nNameTableIndex++;//名字表中下一个空白处

	for (int i = 0; i < g_nNameTableIndex; i++) {
		if (strcmp(g_NameTable[i].szName, szName) == 0) {
			ehandler(REPEAT_IDENTIFIER, 0, szName);
		}
	}


	strcpy(g_NameTable[g_nNameTableIndex].szName, szName);//登记单词名字
	g_NameTable[g_nNameTableIndex].eType = eType;//登记单词类型

	switch (eType)//如果单词的类型
	{
	case CONSTANT_TYPE://单词的类型是常量
		g_NameTable[g_nNameTableIndex].nVal = nNumberValue;//登记常量数值
		break;

	case VARIABLE_TYPE://单词的类型是变量
		g_NameTable[g_nNameTableIndex].nLevel = nLevel;//登记变量所在的层数
		g_NameTable[g_nNameTableIndex].nAddress = (*pAddressInThisLevel);//登记变量被分配的内存空间地址

		PrintAVariable(g_nNameTableIndex);//打印这个变量

		(*pAddressInThisLevel)++;//内存空间中下一个变量的地址加一
		break;

	case PROCEDURE_TYPE://如果单词的类型是过程
		g_NameTable[g_nNameTableIndex].nLevel = nLevel;//登记过程所在的层数
		break;
	}
}
int LookUpNameTable(char* pIdentifier)//在名字表中查找单词(常量,变量或过程名)
{
	int		i;

	for (i = 0; i <= g_nNameTableIndex; i++)
		if (strcmp(g_NameTable[i].szName, pIdentifier) == 0)
			return i;

	ehandler(UNKOWN_IDENTIFIER, 0, pIdentifier);

	return 0;
}

//"一个常量声明"代码生成
int OneConstantGenerate(int nLevel, int* pAddressInCurrentLevel, int nIndentNum)//nLevel是分程序所在的层次,pAddressInCurrentLevel是本层指令偏移地址(指针),nIndentNum是打印时要缩进的空格数
{
	char	szConstantName[100];

	if (g_Words[g_nWordsIndex].eType == IDENTIFIER)//"一个常量声明"以标识符开头
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

		strcpy(szConstantName, g_Words[g_nWordsIndex].szName);

		g_nWordsIndex++;//取下一个单词
		if (g_Words[g_nWordsIndex].eType == EQL)//标识符后面应是'='
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "EQL");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			if (g_Words[g_nWordsIndex].eType == NUMBER)//'='后面的单词应是数
			{
				PrintInGenerate(g_nWordsIndex, nIndentNum, "NUMBER");//打印调试信息

				//在名字表中登记常量
				RegisterInNameTable(CONSTANT_TYPE, szConstantName, g_Words[g_nWordsIndex].nNumberValue, pAddressInCurrentLevel, nLevel);

				g_nWordsIndex++;//取下一个单词
				return OK;
			}
			else
				return ERROR;
		}
		else
			return ERROR;
	}
	else
		return ERROR;

	return ERROR;
}
//"一个变量声明"代码生成
int OneVariableGenerate(int nLevel, int* pAddressInCurrentLevel, int nIndentNum)//nLevel是分程序所在的层次,pAddressInCurrentLevel是本层指令偏移地址(指针),nIndentNum是打印时要缩进的空格数
{
	if (g_Words[g_nWordsIndex].eType == IDENTIFIER)//一个变量声明就是标识符
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

		//在名字表中登记变量
		RegisterInNameTable(VARIABLE_TYPE, g_Words[g_nWordsIndex].szName, g_Words[g_nWordsIndex].nNumberValue, pAddressInCurrentLevel, nLevel);

		g_nWordsIndex++;//取下一个单词

		return OK;
	}
	else
		return ERROR;

	return ERROR;
}

//一条"语句"代码生成
int StatementGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	int nNameTableIndex, nInstructIndexCopy1, nInstructIndexWhenFalse, nNameTableIndexOfAssignedVariable;
	int nInstructIndex_IfPos, nInstructIndex_elsePos;// 保存if跳转指令和else跳转指令的地址 ss
	int nInstructIndex_doPos, nInstructIndex_WhilePos;// 保存do while的开始地址, 保存do while的结束地址

	int switch_identifier_index, switch_begin, switch_end, num_case_jump;
	SWITCH_STRUCT this_switch;

	switch (g_Words[g_nWordsIndex].eType)//当前单词类型是
	{
	case IDENTIFIER://当前单词是标识符(被赋值变量),应按照"赋值语句"处理
		//查找被赋值变量的名字在名字表中的地址
		nNameTableIndexOfAssignedVariable = LookUpNameTable(g_Words[g_nWordsIndex].szName);
		if (nNameTableIndexOfAssignedVariable != 0)
		{//如果查找到,即被赋值变量事先声明过
			PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			if (g_Words[g_nWordsIndex].eType == ASSIGN)//标识符后面应是赋值号单词':='
			{
				PrintInGenerate(g_nWordsIndex, nIndentNum, "ASSIGN");//打印调试信息

				g_nWordsIndex++;//取下一个单词
				PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");//打印调试信息
				ExpressionGenerate(nLevel, nIndentNum + 1);//':='后面应是表达式

				//产生STORE_INSTRUCT指令,将栈顶中表达式的值存到被赋值变量的内存中去
				GenerateOneInstruction(STORE_INSTRUCT, nLevel - g_NameTable[nNameTableIndexOfAssignedVariable].nLevel, g_NameTable[nNameTableIndexOfAssignedVariable].nAddress);

				return OK;
			}
		}
		return ERROR;//没有检测到':='符号
		break;

	case SWITCH:
		PrintInGenerate(g_nWordsIndex, nIndentNum, "SWITCH");//打印调试信息
		g_nWordsIndex++;

		nNameTableIndexOfAssignedVariable = LookUpNameTable(g_Words[g_nWordsIndex].szName);
		if (nNameTableIndexOfAssignedVariable != 0){
			//如果查找到,即被赋值变量事先声明过
			PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息
			g_nWordsIndex++;//取下一个单词
		}

		switch_identifier_index = nNameTableIndexOfAssignedVariable;
		switch_begin = g_nInstructsIndex;

		g_nWordsIndex++;//分号
		
		while (g_Words[g_nWordsIndex].eType == CASE) {
			PrintInGenerate(g_nWordsIndex, nIndentNum, "CASE");//打印调试信息
			g_nWordsIndex++;

			this_switch.case_list[this_switch.num_case] = g_nInstructsIndex;
			this_switch.case_condition[this_switch.num_case] = g_Words[g_nWordsIndex].nNumberValue;
			for (int i = 0; i < this_switch.num_case; i++) {
				if (this_switch.case_condition[i] == g_Words[g_nWordsIndex].nNumberValue) {
					ehandler(REPEAT_CONDITION_IN_SWITCH, g_Words[g_nWordsIndex].nLineNo);
				}
			}
			PrintInGenerate(g_nWordsIndex, nIndentNum, "NUMBER");//打印调试信息
			g_nWordsIndex++;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "COLON");//打印调试信息
			g_nWordsIndex++;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "StatementParsing");//打印调试信息
			StatementGenerate(nLevel, nIndentNum + 1);
			g_nWordsIndex++;//end后的分号
			if (g_Words[g_nWordsIndex].eType == BREAK) {
				this_switch.break_list[this_switch.num_case] = g_nInstructsIndex;
				GenerateOneInstruction(JMP_INSTRUCT, 0, 0);
				PrintInGenerate(g_nWordsIndex, nIndentNum, "BREAK");//打印调试信息
				g_nWordsIndex++;
				g_nWordsIndex++;//分号
			}
			this_switch.num_case++;
		}

		if (g_Words[g_nWordsIndex].eType == DEFAULT) {
			this_switch.has_default = true;
			this_switch.default_pos = g_nInstructsIndex;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "DEFAULT");//打印调试信息
			g_nWordsIndex++;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "COLON");//打印调试信息
			g_nWordsIndex++;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "StatementParsing");//打印调试信息
			StatementGenerate(nLevel, nIndentNum + 1);
			g_nWordsIndex++;//end后的分号
			if (g_Words[g_nWordsIndex].eType == BREAK) {
				PrintInGenerate(g_nWordsIndex, nIndentNum, "BREAK");//打印调试信息
				g_nWordsIndex++;
				g_nWordsIndex++;//分号
			}
		}
		
		switch_end = g_nInstructsIndex;

		//计算处理case跳转需要几条指令
		num_case_jump = this_switch.num_case * 4 + this_switch.has_default;
		//对所有指令序列进行位置调整
		g_nInstructsIndex += num_case_jump;
		for (int i = switch_end; i >= switch_begin; i--) {
			g_Instructs[i + num_case_jump] = g_Instructs[i];
		}
		for (int i = 0; i < this_switch.num_case; i++) {
			this_switch.case_list[i] += num_case_jump;
			this_switch.break_list[i] = this_switch.break_list[i] != 0 ? this_switch.break_list[i] + num_case_jump : 0;
			
		}
		this_switch.default_pos = this_switch.default_pos + num_case_jump;
		switch_end += num_case_jump;
		//添加case跳转指令
		for (int i = 0; i < this_switch.num_case; i++) {
			GenerateOneInstruction(LOAD_INSTRUCT, nLevel - g_NameTable[switch_identifier_index].nLevel, g_NameTable[switch_identifier_index].nAddress, switch_begin);
			switch_begin++;
			GenerateOneInstruction(INSTANT_INSTRUCT, 0, this_switch.case_condition[i], switch_begin);
			switch_begin++;
			GenerateOneInstruction(NEQ_INSTRUCT, 0, 0, switch_begin);
			switch_begin++;
			GenerateOneInstruction(JPC_INSTRUCT, 0, this_switch.case_list[i], switch_begin);
			switch_begin++;
		}
		if (this_switch.has_default) {
			GenerateOneInstruction(JMP_INSTRUCT, 0, this_switch.default_pos, switch_begin);
		}
		else {
			GenerateOneInstruction(JMP_INSTRUCT, 0, switch_end, switch_begin);
		}
		//处理break
		for (int i = 0; i < this_switch.num_case; i++) {
			if (this_switch.break_list[i] != 0) {
				g_Instructs[this_switch.break_list[i]].nOperand = switch_end;
			}
		}

		g_nWordsIndex++;//endswitch

		return OK;
		break;





	case IF://如果当前单词是"if",按"条件语句"处理   ss

		/*
			如果if后条件满足则不跳转，不满足则跳转到else
			if块执行完后跳转到else块后

			条件语句处理后的指令，在条件不满足时栈顶为1跳转
		*/
		PrintInGenerate(g_nWordsIndex, nIndentNum, "IF");//打印调试信息

		g_nWordsIndex++;//取下一个单词 ss 这个单词应该是'（'
		PrintInGenerate(g_nWordsIndex, nIndentNum, "（");
		//g_nWordsIndex++;//ss 这个才是条件的开头
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Condition");//打印调试信息
		ConditionGenerate(nLevel, nIndentNum + 1);//"if"后是"条件"Condition

		PrintInGenerate(g_nWordsIndex, nIndentNum, "）");//打印调试信息
		//g_nWordsIndex++;//取下一个单词 ss 这句话很奇怪，一会要一会不要

		//保存当前指令地址,即if块的假出口地址,等回填时用
		nInstructIndex_IfPos = g_nInstructsIndex;
		//生成条件跳转指令JPC_INSTRUCT,即"THEN"的假出口,跳转地址等待回填
		GenerateOneInstruction(JPC_INSTRUCT, 0, 0);

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
		StatementGenerate(nLevel, nIndentNum + 1);//")"后应是"语句"

		g_Instructs[nInstructIndex_IfPos].nOperand = g_nInstructsIndex;
		g_nWordsIndex++;

		//处理else
		if (g_Words[g_nWordsIndex].eType == ELSE) {
			nInstructIndex_elsePos = g_nInstructsIndex;// 保存下面那条跳转指令地址
			GenerateOneInstruction(JMP_INSTRUCT, 0, 0);//这条指令是给在有else的情况下执行完if块后跳转到else块结束位置

			g_Instructs[nInstructIndex_IfPos].nOperand = g_nInstructsIndex;//if 条件不满足跳转到else块

			//nInstructIndex_IfJumpTarget = g_nInstructsIndex;//这里保存的是else块开始地址

			PrintInGenerate(g_nWordsIndex, nIndentNum, "ELSE");//打印调试信息
			g_nWordsIndex++;
			PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
			StatementGenerate(nLevel, nIndentNum + 1);
			g_Instructs[nInstructIndex_elsePos].nOperand = g_nInstructsIndex;
		}

		g_nWordsIndex++;

		return OK;
		break;

	case DO://do while语句 ss
		PrintInGenerate(g_nWordsIndex, nIndentNum, "DO");//打印调试信息
		nInstructIndex_doPos = g_nInstructsIndex;//保存do while 的开始地址

		g_nWordsIndex++;//取下一个单词
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
		StatementGenerate(nLevel, nIndentNum + 1);

		g_nWordsIndex++;//这个是while
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Condition");//打印调试信息
		ConditionGenerate(nLevel, nIndentNum + 1);//while后是"条件"Condition
		nInstructIndex_WhilePos = g_nInstructsIndex;
		GenerateOneInstruction(JPC_INSTRUCT, 0, 0);

		//满足条件时while后条件语句不跳转，则无条件跳转回while开头
		GenerateOneInstruction(JMP_INSTRUCT, 0, 0);
		g_Instructs[g_nInstructsIndex - 1].nOperand = nInstructIndex_doPos;
		g_Instructs[nInstructIndex_WhilePos].nOperand = g_nInstructsIndex;


		return OK;
		break;


	case WHILE://如果当前单词是"while",准备按照"循环语句"处理
		PrintInGenerate(g_nWordsIndex, nIndentNum, "WHILE");//打印调试信息

		//保存"while"语句的初始地址,为while语句最后跳转回来JMP_INSTRUCT语句作准备
		nInstructIndexCopy1 = g_nInstructsIndex;

		g_nWordsIndex++;//取下一个单词
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Condition");//打印调试信息
		ConditionGenerate(nLevel, nIndentNum + 1);//"while"后面应是"条件"

		//当"条件"不满足时应跳出去,但跳出去的假出口地址现在不知道,需要回填
		//所以要保存当前指令地址,即"while"的假出口地址,等待回填
		nInstructIndexWhenFalse = g_nInstructsIndex;//保存回填的地址
		//生成条件跳转指令JPC_INSTRUCT,即当"条件"不满足时应跳出去
		GenerateOneInstruction(JPC_INSTRUCT, 0, 0);

		if (g_Words[g_nWordsIndex].eType == DO)//"条件"后面应是"do"
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "DO");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
			StatementGenerate(nLevel, nIndentNum + 1);//"do"后面应是"一条语句"

			//while语句最后应安排JMP_INSTRUCT跳回到while语句的初始地址
			GenerateOneInstruction(JMP_INSTRUCT, 0, nInstructIndexCopy1);

			//用下一条指令地址反填"while"的假出口地址
			//即上面"while"条件为假的JPC_INSTRUCT指令的地址
			g_Instructs[nInstructIndexWhenFalse].nOperand = g_nInstructsIndex;

			return OK;
		}
		else
			return ERROR;//缺少do

		break;

	case BEGIN://如果当前单词是"begin",准备按照"复合语句"处理
		PrintInGenerate(g_nWordsIndex, nIndentNum, "BEGIN");//打印调试信息

		g_nWordsIndex++;//取下一个单词
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
		StatementGenerate(nLevel, nIndentNum + 1);//"begin"后面应是一条语句

		while (g_Words[g_nWordsIndex].eType == SEMICOLON)//如果语句后面还有';'
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ";");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			PrintInGenerate(g_nWordsIndex, nIndentNum, "Statement");//打印调试信息
			StatementGenerate(nLevel, nIndentNum + 1);//则";"后面应是"一条语句"
		}

		if (g_Words[g_nWordsIndex].eType == END)//复合语句最后必须是"end"
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "END");//打印调试信息

			g_nWordsIndex++;//取下一个单词
			return OK;
		}
		else
			return ERROR;//缺少end或分号

		break;

	case READ://如果当前单词是"read",准备按照"读语句"处理
		PrintInGenerate(g_nWordsIndex, nIndentNum, "READ");//打印调试信息

		g_nWordsIndex++;//取下一个单词
		if (g_Words[g_nWordsIndex].eType == LEFT_PARENTHESIS)//"read"后面应是左括号'('
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "LEFT_PARENTHESIS");//打印调试信息

			do {//一条"读语句"应至少有一个变量标识符
				g_nWordsIndex++;//取下一个单词

				if (g_Words[g_nWordsIndex].eType == IDENTIFIER)//后面应是一个标识符
				{
					PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

					nNameTableIndex = LookUpNameTable(g_Words[g_nWordsIndex].szName);//查找要读的变量是否已经在名字表中
					if (nNameTableIndex > 0)
					{//如果该变量是声明过的变量名
						GenerateOneInstruction(IN_INSTRUCT, 0, 0);//生成输人指令,读取变量值到栈顶
						//产生STORE_INSTRUCT指令,将栈顶数据存到某层过程的相对地址为操作数的内存中
						GenerateOneInstruction(STORE_INSTRUCT, nLevel - g_NameTable[nNameTableIndex].nLevel, g_NameTable[nNameTableIndex].nAddress);
					}
					else
					{
						return ERROR;
					}

					g_nWordsIndex++;//取下一个单词
				}
				else
					return ERROR;//格式错误,

				if (g_Words[g_nWordsIndex].eType == COMMA)//打印调试信息
					PrintInGenerate(g_nWordsIndex, nIndentNum, "COMMA");
			} while (g_Words[g_nWordsIndex].eType == COMMA);//如果标识符后面有一个逗号','

			if (g_Words[g_nWordsIndex].eType == RIGHT_PARENTHESIS)//最后应是右括号')'
			{
				PrintInGenerate(g_nWordsIndex, nIndentNum, ")");//打印调试信息

				g_nWordsIndex++;//取下一个单词
				return OK;
			}
			else
				return ERROR;//格式错误,应是右括号
		}
		else
			return ERROR;//格式错误,应是左括号

		break;

	case WRITE://如果当前单词是"write",准备按照"写语句"处理
		PrintInGenerate(g_nWordsIndex, nIndentNum, "WRITE");

		g_nWordsIndex++;//取下一个单词
		if (g_Words[g_nWordsIndex].eType == LEFT_PARENTHESIS)//"write"后面应是左括号'('
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "(");

			do {//一条write语句可写多个变量
				g_nWordsIndex++;//取下一个单词

				PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");//打印调试信息
				ExpressionGenerate(nLevel, nIndentNum + 1);//生成"表达式"指令

				//生成输出指令OUT_INSTRUCT,输出栈顶数据
				GenerateOneInstruction(OUT_INSTRUCT, 0, 0);

				if (g_Words[g_nWordsIndex].eType == COMMA)
					PrintInGenerate(g_nWordsIndex, nIndentNum, "COMMA");
			} while (g_Words[g_nWordsIndex].eType == COMMA);//如果标识符后面有一个逗号','

			if (g_Words[g_nWordsIndex].eType == RIGHT_PARENTHESIS)//最后应是右括号')'
			{
				PrintInGenerate(g_nWordsIndex, nIndentNum, ")");

				g_nWordsIndex++;//取下一个单词
				return OK;
			}
			else
				return ERROR;//格式错误,应是右括号
		}
		else
			return ERROR;//格式错误,应是左括号

		break;

	case CALL://如果当前单词是"call",准备按照"调用函数语句"处理
		PrintInGenerate(g_nWordsIndex, nIndentNum, "CALL");

		g_nWordsIndex++;//取下一个单词
		if (g_Words[g_nWordsIndex].eType == IDENTIFIER)//"call"后面应是一个标识符,过程名
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");

			nNameTableIndex = LookUpNameTable(g_Words[g_nWordsIndex].szName);//在名字表中查找该过程名
			if (nNameTableIndex > 0)
			{
				if (g_NameTable[nNameTableIndex].eType == PROCEDURE_TYPE)//如果该过程名是声明过的过程名
					//生成调用过程指令CALL_INSTRUCT,返回地址压栈
					GenerateOneInstruction(CALL_INSTRUCT, nLevel - g_NameTable[nNameTableIndex].nLevel, g_NameTable[nNameTableIndex].nAddress);
				else
					return ERROR;
			}
			else
				return ERROR;

			g_nWordsIndex++;//取下一个单词
			return OK;
		}
		else
			return ERROR;//call后应为标识符

		break;

	default:	return OK;
	}

	return ERROR;
}
//"条件"语法分析代码生成
int ConditionGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	enum	WORD_TYPE_ENUM	ePreRelationOperator;
	g_nWordsIndex++;//这是‘（’ ss
	if (g_Words[g_nWordsIndex].eType == ODD)//如果有"odd"单词
	{
		PrintInGenerate(g_nWordsIndex, nIndentNum, "ODD");//打印调试信息

		g_nWordsIndex++;//取下一个单词

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");//打印调试信息
		ExpressionGenerate(nLevel, nIndentNum + 1);//"odd"后面有一个"表达式"

		//生成判断奇数指令ODD_INSTRUCT,判断栈顶数据是否为奇数.如果是,则栈顶数据为1,否则为0
		GenerateOneInstruction(ODD_INSTRUCT, 0, 0);
	}
	else
	{//如果没有"odd"单词,则"条件"是一个"表达式"
		PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");//打印调试信息
		ExpressionGenerate(nLevel, nIndentNum + 1);//"条件"是一个"表达式"

		if (g_Words[g_nWordsIndex].eType == EQL		//如果后面有逻辑运算符
			|| g_Words[g_nWordsIndex].eType == NEQ
			|| g_Words[g_nWordsIndex].eType == LES
			|| g_Words[g_nWordsIndex].eType == LEQ
			|| g_Words[g_nWordsIndex].eType == GTR
			|| g_Words[g_nWordsIndex].eType == GEQ)
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "LOGICAL");//打印调试信息

			ePreRelationOperator = g_Words[g_nWordsIndex].eType;//保存当前的逻辑运算符

			g_nWordsIndex++;//取下一个单词

			PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");//打印调试信息
			ExpressionGenerate(nLevel, nIndentNum + 1);//继续处理后面的第二个"表达式"

			switch (ePreRelationOperator)//根据前面保存的逻辑运算符,产生相应的指令
			{
			case EQL://取出并判断栈顶数据和次栈顶数据是否相同.如果相同,则栈顶数据为1,否则为0
				GenerateOneInstruction(EQL_INSTRUCT, 0, 0);
				break;
			case NEQ://取出并判断栈顶数据和次栈顶数据是否不相同.如果相同,则栈顶数据为0,否则为1
				GenerateOneInstruction(NEQ_INSTRUCT, 0, 0);
				break;
			case LES://取出并判断次栈顶数据是否小于栈顶数据.如果是,则栈顶数据为1,否则为0
				GenerateOneInstruction(LES_INSTRUCT, 0, 0);
				break;
			case GEQ://取出并判断次栈顶数据是否大于等于栈顶数据.如果是,则栈顶数据为1,否则为0个
				GenerateOneInstruction(GEQ_INSTRUCT, 0, 0);
				break;
			case GTR://取出并判断次栈顶数据是否大于栈顶数据.如果是,则栈顶数据为1,否则为0
				GenerateOneInstruction(GTR_INSTRUCT, 0, 0);
				break;
			case LEQ://取出并判断次栈顶数据是否小于等于栈顶数据.如果是,则栈顶数据为1,否则为0
				GenerateOneInstruction(LEQ_INSTRUCT, 0, 0);
				break;
			}
		}
		else
			return ERROR;//缺少右括号


	}
	g_nWordsIndex++;//这是‘）’ ss
	return OK;
}
//"表达式"语法分析代码生成
int ExpressionGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	enum	WORD_TYPE_ENUM	eMathOperator = PLUS;

	if (g_Words[g_nWordsIndex].eType == PLUS || g_Words[g_nWordsIndex].eType == MINUS)
	{//如果表达式开头有正负号,则此时表达式应被看作一个正的或负的项
		if (g_Words[g_nWordsIndex].eType == PLUS)//如果开头符号是正号,则忽略
			PrintInGenerate(g_nWordsIndex, nIndentNum, "PLUS");//打印调试信息
		if (g_Words[g_nWordsIndex].eType == MINUS)//如果开头符号是负号
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, "MINUS");//打印调试信息
			eMathOperator = MINUS;//保存当前的负号
		}

		g_nWordsIndex++;//取下一个单词
	}

	PrintInGenerate(g_nWordsIndex, nIndentNum, "Term");//打印调试信息
	TermGenerate(nLevel, nIndentNum + 1);//表达式可以是一个"项"

	if (eMathOperator == MINUS)//如果开头符号是负号,则生成取负指令NEGATIVE_INSTRUCT
		GenerateOneInstruction(NEGATIVE_INSTRUCT, 0, 0);//取负操作,将栈顶数据取负后仍放在栈顶

	while (g_Words[g_nWordsIndex].eType == PLUS || g_Words[g_nWordsIndex].eType == MINUS)
	{//如果"项"后面还有'+'或'-'
		if (g_Words[g_nWordsIndex].eType == PLUS)
		{//"项"后面是'+'
			PrintInGenerate(g_nWordsIndex, nIndentNum, "PLUS");//打印调试信息
			eMathOperator = PLUS;//保存运算符号
		}
		if (g_Words[g_nWordsIndex].eType == MINUS)
		{//"项"后面是'-'
			PrintInGenerate(g_nWordsIndex, nIndentNum, "MINUS");//打印调试信息
			eMathOperator = MINUS;//保存运算符号
		}

		g_nWordsIndex++;//取下一个单词

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Term");//打印调试信息
		TermGenerate(nLevel, nIndentNum + 1);//'+'或'-'后面还有"项"

		if (eMathOperator == PLUS)//如果前面保存的运算符号是'+'
			GenerateOneInstruction(ADD_INSTRUCT, 0, 0);//生成加法指令ADD_INSTRUCT,取出栈顶数据和次栈顶数据的和再放到栈顶
		if (eMathOperator == MINUS)//如果前面保存的运算符号是'-'
			GenerateOneInstruction(SUB_INSTRUCT, 0, 0);//生成减法指令SUB_INSTRUCT,取出栈顶数据和次栈顶数据的差再放到栈顶
	}
	return OK;
}
//"项"语法分析代码生成
int TermGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	enum	WORD_TYPE_ENUM	eMathOperator = PLUS;

	PrintInGenerate(g_nWordsIndex, nIndentNum, "Factor");//打印调试信息
	FactorGenerate(nLevel, nIndentNum + 1);//"项"可以是一个"因子"

	while (g_Words[g_nWordsIndex].eType == MULTIPLY || g_Words[g_nWordsIndex].eType == DIVIDE)
	{//如果"因子"后面还有'*'或'/'
		PrintInGenerate(g_nWordsIndex, nIndentNum, "*/");//打印调试信息

		eMathOperator = g_Words[g_nWordsIndex].eType;//先保存运算符号

		g_nWordsIndex++;//取下一个单词

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Factor");//打印调试信息
		FactorGenerate(nLevel, nIndentNum + 1);//'*'或'/'后面还应有"因子"

		if (eMathOperator == MULTIPLY)//如果前面保存的运算符号是'*'
			GenerateOneInstruction(MULTI_INSTRUCT, 0, 0);//生成乘法指令MULTI_INSTRUCT,取出栈顶数据和次栈顶数据的积再放到栈顶
		if (eMathOperator == DIVIDE)//如果前面保存的运算符号是'/'
			GenerateOneInstruction(DIVID_INSTRUCT, 0, 0);//生成除法指令DIVID_INSTRUCT,取出栈顶数据和次栈顶数据的商再放到栈顶
	}
	return OK;
}
//"因子"语法分析代码生成
int FactorGenerate(int nLevel, int nIndentNum)//nLevel是分程序所在的层次,nIndentNum是打印时要缩进的空格数
{
	int		nNameTableIndex;

	switch (g_Words[g_nWordsIndex].eType)
	{
	case IDENTIFIER://因子可以是一个常量或变量
		PrintInGenerate(g_nWordsIndex, nIndentNum, "IDENTIFIER");//打印调试信息

		nNameTableIndex = LookUpNameTable(g_Words[g_nWordsIndex].szName);//在名字表中查找该常量或变量名
		if (nNameTableIndex > 0)
		{
			switch (g_NameTable[nNameTableIndex].eType)
			{
			case CONSTANT_TYPE://名字为常量
				//生成INSTANT_INSTRUCT指令,直接将名字表中的操作数的值压入栈顶
				GenerateOneInstruction(INSTANT_INSTRUCT, 0, g_NameTable[nNameTableIndex].nVal);
				break;
			case VARIABLE_TYPE://名字为变量
				//生成LOAD_INSTRUCT指令,找到变量地址并将其值压入栈顶
				GenerateOneInstruction(LOAD_INSTRUCT, nLevel - g_NameTable[nNameTableIndex].nLevel, g_NameTable[nNameTableIndex].nAddress);
				break;
			default: return ERROR;
			}
		}
		else
			return ERROR;//标识符未事先声明

		g_nWordsIndex++;//取下一个单词
		return OK;

	case NUMBER:	//因子可以是一个数
		PrintInGenerate(g_nWordsIndex, nIndentNum, "NUMBER");//打印调试信息
		//生成INSTANT_INSTRUCT指令,直接把数的值压入栈
		GenerateOneInstruction(INSTANT_INSTRUCT, 0, g_Words[g_nWordsIndex].nNumberValue);

		g_nWordsIndex++;//取下一个单词
		return OK;

	case LEFT_PARENTHESIS:	//如果看到左括号'('
		PrintInGenerate(g_nWordsIndex, nIndentNum, "(");

		g_nWordsIndex++;//取下一个单词

		PrintInGenerate(g_nWordsIndex, nIndentNum, "Expression");
		ExpressionGenerate(nLevel, nIndentNum + 1);//左括号'('后面应是"表达式"

		if (g_Words[g_nWordsIndex].eType == RIGHT_PARENTHESIS)//"表达式"后面应是右括号')'
		{
			PrintInGenerate(g_nWordsIndex, nIndentNum, ")");

			g_nWordsIndex++;//取下一个单词
			return OK;
		}
		else
			return ERROR;//缺少右括号
		break;

	default:		return ERROR;
	}
	return OK;
}
//将枚举类型的指令转换成字符串
int InstructFromEnumToString(enum INSTRUCT_ENUM eInstruct, char* szString)
{
	switch (eInstruct)
	{
	case INSTANT_INSTRUCT:
		strcpy(szString, "INSTANT");
		break;
	case LOAD_INSTRUCT:
		strcpy(szString, "LOAD");
		break;
	case STORE_INSTRUCT:
		strcpy(szString, "STORE");
		break;
	case CALL_INSTRUCT:
		strcpy(szString, "CALL");
		break;
	case ENDCALL_INSTRUCT:
		strcpy(szString, "ENDCALL");
		break;
	case ALLOCATE_INSTRUCT:
		strcpy(szString, "ALLOCATE");
		break;
	case JMP_INSTRUCT:
		strcpy(szString, "JMP");
		break;
	case JPC_INSTRUCT:
		strcpy(szString, "JPC");
		break;
	case NEGATIVE_INSTRUCT:
		strcpy(szString, "NEG");
		break;
	case ADD_INSTRUCT:
		strcpy(szString, "ADD");
		break;
	case SUB_INSTRUCT:
		strcpy(szString, "SUB");
		break;
	case MULTI_INSTRUCT:
		strcpy(szString, "MULTI");
		break;
	case DIVID_INSTRUCT:
		strcpy(szString, "DIVID");
		break;
	case ODD_INSTRUCT:
		strcpy(szString, "ODD");
		break;
	case EQL_INSTRUCT:
		strcpy(szString, "EQL");
		break;
	case NEQ_INSTRUCT:
		strcpy(szString, "NEQ");
		break;
	case LES_INSTRUCT:
		strcpy(szString, "LES");
		break;
	case GEQ_INSTRUCT:
		strcpy(szString, "GEQ");
		break;
	case GTR_INSTRUCT:
		strcpy(szString, "GTR");
		break;
	case LEQ_INSTRUCT:
		strcpy(szString, "LEQ");
		break;
	case OUT_INSTRUCT:
		strcpy(szString, "OUT");
		break;
	case IN_INSTRUCT:
		strcpy(szString, "IN");
		break;
	}
	return OK;
}

void PrintInGenerate(int nWordsIndex, int nIndentNum, const char* pString)//打印调试信息
{
	int		i;
	char	strText[100];

	printf("\n%-4d%-4d%-12s", nWordsIndex + 1, g_Words[nWordsIndex].nLineNo, g_Words[nWordsIndex].szName);
	printf("%-1s", "|");
	for (i = 0; i < nIndentNum; i++)
		printf(" ");
	printf("%-1s", " ");

	sprintf(strText, "(%d)%s", nIndentNum, pString);
	printf("%-15s", strText);

	for (i = 0; i < 10 - nIndentNum; i++)
		printf(" ");
	printf("|");
}
//在指令队列g_Instructs结构数组中生成一条虚拟指令
int GenerateOneInstruction(INSTRUCT_ENUM eInstruct, int nLevel, int nOperand)//eInstruct是虚拟指令的操作码,nLevel是虚拟指令操作码引用层与声明层的层次差,nOperand是虚拟指令操作数
{
	char		szText[100];

	if (g_nInstructsIndex >= MAX_NUMBER_INSTRUCTS)
	{
		printf("GenerateOneInstruction()--Program too long");//程序过长,无法继续存放
		return	-1;
	}
	g_Instructs[g_nInstructsIndex].eInstruct = eInstruct;//存放虚拟指令操作码
	g_Instructs[g_nInstructsIndex].nLevel = nLevel;//存放虚拟指令操作码引用层与声明层的层次差
	g_Instructs[g_nInstructsIndex].nOperand = nOperand;//存放虚拟指令操作数

	//打印调试信息
	InstructFromEnumToString(g_Instructs[g_nInstructsIndex].eInstruct, szText);
	printf("\n\t\t\t\t\t         %-4d%-10s%-4d%-3d", g_nInstructsIndex, szText, g_Instructs[g_nInstructsIndex].nLevel, g_Instructs[g_nInstructsIndex].nOperand);

	g_nInstructsIndex++;//下一条指令存放的位置下标加一

	return 0;
}

//在指令队列g_Instructs结构数组中生成一条虚拟指令 ss
int GenerateOneInstruction(INSTRUCT_ENUM eInstruct, int nLevel, int nOperand, int pos)//eInstruct是虚拟指令的操作码,nLevel是虚拟指令操作码引用层与声明层的层次差,nOperand是虚拟指令操作数
{
	char		szText[100];

	if (pos >= MAX_NUMBER_INSTRUCTS)
	{
		printf("GenerateOneInstruction()--Program too long");//程序过长,无法继续存放
		return	-1;
	}
	g_Instructs[pos].eInstruct = eInstruct;//存放虚拟指令操作码
	g_Instructs[pos].nLevel = nLevel;//存放虚拟指令操作码引用层与声明层的层次差
	g_Instructs[pos].nOperand = nOperand;//存放虚拟指令操作数

	//打印调试信息
	InstructFromEnumToString(g_Instructs[pos].eInstruct, szText);
	printf("\n\t\t\t\t\t         %-4d%-10s%-4d%-3d", pos, szText, g_Instructs[pos].nLevel, g_Instructs[pos].nOperand);


	return 0;
}


void PrintAllInstructions()//最后打印所有生成的指令
{
	char		szText[100];

	std::ofstream f("./pcode.txt", std::ios_base::out);

	for (int i = 0; i < g_nInstructsIndex; i++)
	{
		InstructFromEnumToString(g_Instructs[i].eInstruct, szText);
		printf("\n%-5d%-15s%-5d%-5d", i, szText, g_Instructs[i].nLevel, g_Instructs[i].nOperand);
		f << i << ' ' << szText << ' ' << g_Instructs[i].nLevel << ' ' << g_Instructs[i].nOperand << std::endl;
	}
	f.close();
}

void PrintAVariable(int nNameTableIndex)
{
	printf("\n\t\t\t\t\t       --Table[%d] %s Level=%d Addr=%d", nNameTableIndex, g_NameTable[nNameTableIndex].szName, g_NameTable[nNameTableIndex].nLevel, g_NameTable[nNameTableIndex].nAddress);
}