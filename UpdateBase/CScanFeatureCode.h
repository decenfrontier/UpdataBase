#pragma once
#include <Windows.h>
#include <stdio.h>
#include <string>

using namespace std;

void BytesToHexStr(BYTE* nbData, DWORD nbDataSize, char* szpHexBuf);
void ByteToHex(char* szpBuffHex, BYTE c);
char CharToUp(char c);
void StrToUp(char* szpHexStr);
BOOL HexStrCmp(const char* HexStr1, const char* HexStr2);
DWORD ScanFeatureCode(HANDLE hProcess, char* szpFeatureCode,
				DWORD ndStartAddress = 0x00401000, DWORD ndEndAddress = 0x7FFFFFFF);
int* GetNext(string T, int next[]);
int GetIndexByKMP(string S, string T, int next[]);
BOOL IsHexChar(char c);
DWORD  ReadBaseAddr(HANDLE hProcess, DWORD ndAddr);
void OneKeyUpdateBase(HANDLE hProcess);