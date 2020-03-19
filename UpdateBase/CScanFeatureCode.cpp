#include "CScanFeatureCode.h"

/********************************************************************************************/
/* Description	: 字节集 转 十六进制字符串
/* Return		: 无返回值
/* Time			: 2020/01/24 11:46
/* Remark		: szpHexBuf为OUT参数
/********************************************************************************************/
void BytesToHexStr(BYTE* nbData, DWORD nbDataSize, char* szpHexBuf) {
	for (DWORD i = 0; i < nbDataSize; i++)
	{
		ByteToHex(&szpHexBuf[2*i],nbData[i]);
	}
}

/********************************************************************************************/
/* Description	: 把一个字节变成十六进制字符串
/* Return		: 无返回值
/* Time			: 2020/01/26 16:13
/* Remark		: 
/********************************************************************************************/
void ByteToHex(char * szpBuffHex, BYTE c)
{
	int high = c / 16;		//取字节高位:0-15
	int low = c % 16;		//取字节低位:0-15
	//高位处理
	if (high > 9)	szpBuffHex[0] = 'A' + high - 10;
	else    szpBuffHex[0] = '0' + high;
	//低位处理
	if (low > 9)	szpBuffHex[1] = 'A' + low - 10;
	else    szpBuffHex[1] = '0' + low;
}

/********************************************************************************************/
/* Description	: 把一个字符变成大写
/* Return		: 一个字符如果是小写字母，就返回大写字母。其它则直接返回它自身
/* Time			: 2020/01/24 16:46
/* Remark		: 
/********************************************************************************************/
char CharToUp(char c)
{
	if (c >= 'a' && c <= 'z')
	{
		return c - 0x20;
	}
	return c;
}

/********************************************************************************************/
/* Description	: 把字符串中的字符全部转化为大写字母 && 非十六进制的通配符替换为‘X’
/* Return		: 无返回值
/* Time			: 2020/01/24 17:03
/* Remark		: 
/********************************************************************************************/
void StrToUp(char* szpHexStr)
{
	DWORD ndStrLen = strlen(szpHexStr);
	for (DWORD i = 0; i <ndStrLen; i++)
	{
		szpHexStr[i] = CharToUp(szpHexStr[i]);	//先转换为大写
		if (IsHexChar(szpHexStr[i])==FALSE)
		{
			szpHexStr[i] = 'X';	//如果该字符不是十六进制，则替换为‘X’
		}
	}
}

/********************************************************************************************/
/* Description	: 比较十六位字符串是否相同
/* Return		: 两字符串相同 返回真，否则返回假
/* Time			: 2020/01/24 16:03
/* Remark		: 
/********************************************************************************************/
BOOL HexStrCmp(const char * HexStr1, const char * HexStr2)
{
	DWORD ndHexLen1 = strlen(HexStr1);
	DWORD ndHexLen2 = strlen(HexStr2);
	if (ndHexLen1 > ndHexLen2)	ndHexLen1 = ndHexLen2;
	for (DWORD i = 0; i < ndHexLen1; i++)
	{
		if (HexStr1[i]=='X' || HexStr2[i]=='X')	continue;	//是通配符则跳过
		if (HexStr1[i] == HexStr2[i])	continue;
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************************/
/* Description	: 扫描指定进程的特征码
/* Return		: 成功找到返回特征码的起始VA，失败返回NULL
/* Time			: 2020/01/25 12:03
/* Remark		: 
/********************************************************************************************/
#define PAGESIZE 4096	//分页读取，每次读取4096个字节
DWORD ScanFeatureCode(HANDLE hProcess, char* szpFeatureCode,
	DWORD ndStartAddress, DWORD ndEndAddress)
{
	//先把字串中的字符全部转为大写，并替换通配符为‘X’
	StrToUp(szpFeatureCode);
	//获取特征码字串的next数组
	int* next = new int[strlen(szpFeatureCode)];
	GetNext(szpFeatureCode, next);
	//获取十六进制特征码的字节数
	DWORD ndLenHexCode = strlen(szpFeatureCode) / 2;
	//申请一个存放内存字节数据的缓冲区，大小为 页面长度+字节数+NULL
	BYTE* nbDataBuf = new BYTE[PAGESIZE + ndLenHexCode + 2];
	//实际读取的缓冲区大小
	DWORD ndByReadSize = 0;
	//逐页搜索特征码
	for (DWORD ndCurAddr = ndStartAddress; ndCurAddr < ndEndAddress - ndLenHexCode; ndCurAddr += PAGESIZE)
	{
		//写入 游戏进程的内存页字节 到 nbDataBuf
		ReadProcessMemory(hProcess, (LPCVOID)ndCurAddr, nbDataBuf, PAGESIZE + ndLenHexCode + 2, &ndByReadSize);

		//写入 nbDataBuf 到 szpTempHex,特征码字节数不能超过256字节
		char szpTempHex[(PAGESIZE + 256 + 2) * 2] = "";
		BytesToHexStr(nbDataBuf, ndByReadSize, szpTempHex);	// PAGESIZE + ndLenHexCode + 2

		//把char*类型的C风格字符串转换为string类型的变量
		string FeatureStr = szpFeatureCode;
		string MemberStr = szpTempHex;

		//获取字串szpTempHex与FeatureStr匹配成功时的起始下标
		int iIndex = GetIndexByKMP(MemberStr, FeatureStr, next);
		if (iIndex != -1)
		{
			return ndCurAddr + iIndex / 2;
		}

	}
	return NULL;
}

/********************************************************************************************/
/* Description	: 获得子串T的next数组
/* Return		: 返回next数组的指针
/* Time			: 2020/01/26 17:54
/* Remark		: 
/********************************************************************************************/
int* GetNext(string T,int next[]) {
	UINT len = T.length();
	next[0] = 0;
	//构建部分匹配串
	for (UINT i=1,j=0; i< len; i++)
	{
		//先判断是否有通配符，有的话就比后面的字符，进入下一轮循环
		if (T.at(i) == 'X' || T.at(j) == 'X')
		{
			j++;	next[i] = j;
			continue;
		}
		while (j>0 && T.at(i)!=T.at(j))
		{
			j = next[j - 1];	
		}
		if (T.at(i) == T.at(j))
		{
			j++;
		}
		next[i] = j;
	}
	//构建next数组,即将部分匹配串数组右移,再令next[0]=-1
	for (UINT j = len-1; j > 0; j--)
	{
		next[j] = next[j - 1];
	}
	next[0] = -1;
	return next;
}

/********************************************************************************************/
/* Description	: 利用KMP算法,获取下次开始匹配的子串下标
/* Return		: 成功返回下次开始匹配的子串下标，失败返回-1
/* Time			: 2020/01/26 23:22
/* Remark		: S为母串，T为子串。
/********************************************************************************************/
int GetIndexByKMP(string S, string T, int next[]) {
	int len1 = S.length();
	int len2 = T.length();
	int i=0,j=0;
	while ( i<len1 && j<len2 )
	{
		if (j==-1 || S.at(i)==T.at(j) || T.at(j)=='X')	//通配符适配
		{
			i++;	j++;
		}
		else
			j = next[j];
	}
	//匹配结束
	if (j >= len2)	//匹配成功
		return i - len2;
	else
		return -1;
}

/********************************************************************************************/
/* Description	: 判断一个字符是否是十六进制字符
/* Return		: 是则返回真，否则返回假
/* Time			: 2020/01/27 17:13
/* Remark		: 
/********************************************************************************************/
BOOL IsHexChar(char c)
{
	if (c>='0' && c<='9')
	{
		return TRUE;
	}
	if (c>='A' && c<='F')
	{
		return TRUE;
	}
	return FALSE;
}

/********************************************************************************************/
/* Description	: 根据特征码地址，读出基址
/* Return		: 则返回真，否则返回假
/* Time			: 2020/01/28 13:28
/* Remark		: 
/********************************************************************************************/
DWORD ReadBaseAddr(HANDLE hProcess, DWORD ndAddr)
{
	DWORD ndBaseAddr = NULL;
	BOOL bRet = ReadProcessMemory(hProcess, (LPCVOID)ndAddr, &ndBaseAddr, 4,NULL);
	return ndBaseAddr;
}

/********************************************************************************************/
/* Description	: 一键更新基址
/* Return		: 
/* Time			: 2020/02/09 00:14
/* Remark		: 
/********************************************************************************************/
void OneKeyUpdateBase(HANDLE hProcess)
{
	DWORD ndLocateAddr = NULL;
	DWORD ndBaseAddr = NULL;
	char szpFeatureCode[256] = "";

	//Base_GameWndHandle
	strcpy_s(szpFeatureCode, "8B086A1652508B4134");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_GameWndHandle 0x%08X	//游戏主窗口句柄\n\n", ndBaseAddr);

	//Base_AllObjList
	strcpy_s(szpFeatureCode, "83C404A308C0C0008B018B50045757");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_AllObjList 0x%08X		//所有对象列表基址\n", ndBaseAddr);
	printf("										//特定对象基址：[所有对象列表基址+4*ID]\n\n");

	//Base_RoleProperty
	strcpy_s(szpFeatureCode, "558BEC83EC085356B8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 9);		//读取基址
	printf("#define Base_RoleProperty 0x%08X		//人物属性基址\n\n", ndBaseAddr);

	//Base_EquipList
	strcpy_s(szpFeatureCode, "BF3C0400008D9B00000000833C070074**8B0C07");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_EquipList 0x%08X			//装备列表基址\n", ndBaseAddr);
	//Base_ShopList
	strcpy_s(szpFeatureCode, "6A0050E8********8B4F0883C40C51B9");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 16);		//读取基址
	printf("#define Base_ShopList 0x%08X			//商店列表基址\n", ndBaseAddr);
	//Base_DepotList
	strcpy_s(szpFeatureCode, "C78134160000080000008B152C69E402899A6C020000");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_DepotList 0x%08X			//仓库列表基址\n", ndBaseAddr);
	//Base_BackPackList
	strcpy_s(szpFeatureCode, "8B848A3C0400008BB0E40C00008BB8E80C00008BC60BC7");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_BackPackList 0x%08X		//背包列表基址\n", ndBaseAddr);
	//Call_UseObjForIndex
	strcpy_s(szpFeatureCode, "8B87601C00005651508BCFE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 12);		//读取基址
	printf("#define Call_UseObjForIndex 0x%08X	//背包物品使用CALL\n", ndBaseAddr + ndLocateAddr + 16);
	//Call_SendData
	strcpy_s(szpFeatureCode, "6689B5FED7FFFF66899500D8FFFF66898502D8FFFFE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 22);		//读取基址
	printf("#define Call_SendData 0x%08X		//发送数据CALL\n", ndBaseAddr + ndLocateAddr + 26);
	//Call_SendData_Ecx
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Call_SendData_Ecx 0x%08X	//发送数据CALL的Ecx\n\n", ndBaseAddr);

	//Base_NearObjList
	strcpy_s(szpFeatureCode, "8B118B420453536A02FFD003F7");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//读取基址
	printf("#define Base_NearObjList 0x%08X		//周围对象列表基址\n", ndBaseAddr);
	//Base_PlayerObj
	strcpy_s(szpFeatureCode, "85C074**83B8880100000074**8D8D30FCFFFF");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//读取基址
	printf("#define Base_PlayerObj 0x%08X		//玩家对象基址\n\n", ndBaseAddr);

	//Base_Unknown
	strcpy_s(szpFeatureCode, "6A09E8********5F5E5B8BE55DC2");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//读取基址
	printf("#define Base_Unknown 0x%08X			//未知对象基址\n", ndBaseAddr);
	//Base_Unknown2
	strcpy_s(szpFeatureCode, "8B15********3BC274**8B083BCB");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_Unknown2 0x%08X		//未知对象基址2\n\n", ndBaseAddr);

	//Base_ActionList
	strcpy_s(szpFeatureCode, "833C0600743C8B04068B50548B7858");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr -4);		//读取基址
	printf("#define Base_ActionList 0x%08X			//动作列表基址\n", ndBaseAddr);
	//Call_ActionUse
	strcpy_s(szpFeatureCode, "83BF341600003675**8B84B73C04000085C0");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Call_ActionUse 0x%08X		//动作使用CALL\n\n", ndBaseAddr+ ndLocateAddr);

	//Base_SkillList 
	strcpy_s(szpFeatureCode, "33C0A3********8B96C00B000083C2288950388B86C00B00008B0D");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +3);		//读取基址
	printf("#define Base_SkillList  0x%08X			//技能列表基址\n", ndBaseAddr);
	//Call_LearnSkill
	strcpy_s(szpFeatureCode, "8B94B78C0300008B0D********8B425451508D8DF4D7FFFF518B0D");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +32);		//读取基址
	printf("#define Call_LearnSkill 0x%08X		//修炼技能CALL\n", ndBaseAddr+ ndLocateAddr+36);
	//Call_LearnSkill_Ecx
	strcpy_s(szpFeatureCode, "5068********8D4DBC6A4051E8********8B8E2C0F0000");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Call_LearnSkill_Ecx 0x%08X	//修炼技能CALL的ECX\n\n", ndBaseAddr);

	//Base_ShortcutBar
	strcpy_s(szpFeatureCode, "33C08D8E3C040000EB**8D490083390074");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_ShortcutBar 0x%08X			//快捷栏基址\n", ndBaseAddr);
	//Call_F1F10Use
	strcpy_s(szpFeatureCode, "83BC8A3C040000000F84********A16C452501518B888C020000E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr +27);		//读取基址
	printf("#define Call_F1F10Use 0x%08X		//快捷栏使用CALL\n\n", ndBaseAddr + ndLocateAddr + 31);

	//Base_MouseSelObj
	strcpy_s(szpFeatureCode, "85D274**833C070074**8B0C078B59588B49548B400C");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr - 4);		//读取基址
	printf("#define Base_MouseSelObj 0x%08X			//鼠标拖起的对象基址\n", ndBaseAddr);
	printf("											//鼠标拖起的对象:[[鼠标拖起的对象基址]+0x230]\n");
	//Call_MoveObjToList
	strcpy_s(szpFeatureCode, "8B86900200008B0D********8B9134160000508B81601C00005250E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 28);		//读取基址
	printf("#define Call_MoveObjToList 0x%08X	//移动对象到列表CALL\n\n", ndBaseAddr + ndLocateAddr + 32);

	//Call_ChatWithNPC
	strcpy_s(szpFeatureCode, "33C05DC20C008B5510528B550C5250E8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 16);		//读取基址
	printf("#define Call_ChatWithNPC 0x%08X			//与NPC对话CALL\n", ndBaseAddr + ndLocateAddr + 20);
	//Call_ClickChatMenuOption
	strcpy_s(szpFeatureCode, "8B9C9EA801000081FB3C0100000F8D********538BCEE8");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 23);		//读取基址
	printf("#define Call_ClickChatMenuOption 0x%08X	//点击对话菜单选项CALL\n\n", ndBaseAddr + ndLocateAddr + 27);

	//Base_ViewRange
	strcpy_s(szpFeatureCode, "6A016A006A006A006A0068********525056E8********D905");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	ndBaseAddr = ReadBaseAddr(hProcess, ndLocateAddr + 25);		//读取基址
	printf("#define Base_ViewRange 0x%08X			//玩家可视范围基址\n\n", ndBaseAddr);

	//Base_ScreenRefreshHook
	strcpy_s(szpFeatureCode, "8B86940100008B088B91A800000050FFD28BCE");
	ndLocateAddr = ScanFeatureCode(hProcess, szpFeatureCode);	//定位特征码
	printf("#define Base_ScreenRefreshHook 0x%08X		//画面刷新Hook基址\n\n", ndLocateAddr + 14);
}
