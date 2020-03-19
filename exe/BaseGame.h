#pragma once
//基址管理单元
#define Base_GameWndHandle 0x01196F88	//游戏主窗口句柄

#define Base_AllObjList 0x02E65A28		//所有对象列表基址
										//特定对象基址：[所有对象列表基址+4*ID]

#define Base_RoleProperty 0x02C186D8		//人物属性基址

#define Base_EquipList 0x02E3D3E0			//装备列表基址
#define Base_ShopList 0x0119B7B0			//商店列表基址
#define Base_DepotList 0x02E4692C			//仓库列表基址
#define Base_BackPackList 0x02E3D3E4		//背包列表基址
#define Call_UseObjForIndex 0x00838EA0	//背包物品使用CALL
#define Call_SendData 0x004F8740		//发送数据CALL
#define Call_SendData_Ecx 0x01197000	//发送数据CALL的Ecx

#define Base_NearObjList 0x04280BA0		//周围对象列表基址
#define Base_PlayerObj 0x02E65A24		//玩家对象基址

#define Base_Unknown 0x0125456C			//未知对象基址
#define Base_Unknown2 0x02C1EA6C		//未知对象基址2

#define Base_ActionList 0x02E3DD58			//动作列表基址
#define Call_ActionUse 0x00714220		//动作使用CALL

#define Base_SkillList  0x02E3EF64			//技能列表基址
#define Call_LearnSkill 0x00555B80		//修炼技能CALL
#define Call_LearnSkill_Ecx 0x02C18784	//修炼技能CALL的ECX

#define Base_ShortcutBar 0x02E3EF08			//快捷栏基址
#define Call_F1F10Use 0x00723420		//快捷栏使用CALL

#define Base_MouseSelObj 0x0125455C			//鼠标拖起的对象基址
											//鼠标拖起的对象:[[鼠标拖起的对象基址]+0x230]
#define Call_MoveObjToList 0x0084A180	//移动对象到列表CALL

#define Call_ChatWithNPC 0x0050D3F0			//与NPC对话CALL
#define Call_ClickChatMenuOption 0x007D5B30	//点击对话菜单选项CALL

#define Base_ViewRange 0x00AA1DBC			//玩家可视范围基址

#define Base_ScreenRefreshHook 0x00592CE2			//画面刷新Hook基址

#define Call_ActionUse_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)
#define Call_F1F10Use_Ecx *(DWORD*)(*(DWORD*)(Base_Unknown)+0x28C)
