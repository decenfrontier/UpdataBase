// UpdateBase.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "CScanFeatureCode.h"
#include <ctime>

#define GameClassName "D3D Window"

int main()
{
	clock_t StartTime, EndTime;
	BOOL bRet = 1;
	HWND hGame = FindWindowA(GameClassName, NULL);
	if (hGame == NULL)
	{
		printf("游戏窗口未找到\n");
		bRet = 0;
	}
	DWORD dwpid = 0;
	GetWindowThreadProcessId(hGame, &dwpid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwpid);
	if (hProcess == NULL)
	{
		printf("游戏进程打开失败\n");
		bRet = 0;
	}
	if (bRet)
	{
		cout << "正在更新基址到文件，请等待1分钟..." << endl;
		FILE* pFile = NULL;
		freopen_s(&pFile, "BaseGame.h", "w", stdout);	//输出重定向到 输出目录，生成的文件为"BaseGame.h"

		printf("#pragma once\n");
		printf("//基址管理单元\n");

		StartTime = clock();
		OneKeyUpdateBase(hProcess);
		EndTime = clock();

		printf("#define Call_ActionUse_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)\n");
		printf("#define Call_F1F10Use_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)\n");

		fclose(pFile);
		freopen_s(&pFile, "conout$", "w", stdout);	//输出重定向到 控制台输出,即恢复标准输出
		cout << "更新基址到文件已完成,所用时间为：" << EndTime - StartTime << endl;
	}
	getchar();
}





