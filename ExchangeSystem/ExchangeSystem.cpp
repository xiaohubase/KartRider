
#include "../struct.h"
#include "../resource.h"
#include "../helper/DbgPrint.h"
#include "../KartRider.hpp"
#include "../helper/detoursHlper.hpp"
#include "../helper/aobscan/aobscan.hpp"



#define DbgPrint2
// #define DbgPrint2 MyDbgPrintFun

extern HWND g_GamehWnd;
extern HMODULE g_hModule;
extern HMODULE g_hModule;
extern DWORD g_BaseAddress;
extern DWORD g_BaseAddress1;
extern LPCDLGTEMPLATEW pDlgtmp_Dialog_Exchange;

namespace ExchangeSystem
{
	HWND hDlg;
	HWND hGameWnd;
	HWND hWnd_combox_carnum;
	HWND hWnd_combox_lucky;
	HWND hWnd_edit_carnum;
	HWND hWnd_edit_lockpage;
	HWND hWnd_edit_delay;
	HWND hWnd_edit_addcar_name;
	HWND hWnd_edit_addcar_code;

	HWND hWnd_list_filter;
	HWND hWnd_check_begin;
	HWND hWnd_check_lucky;
	HWND hWnd_check_cchresult;
	HWND hWnd_check_showcarnum;
	HWND hWnd_check_lockpage;
	HWND hWnd_check_filter;
	HWND hWnd_check_filter_show;
	HWND hWnd_check_filter_Engine_V1;
	HWND hWnd_check_filter_Engine_X;
	HWND hWnd_check_filter_Engine_9;
	HWND hWnd_check_filter_Engine_HTNew;
	HWND hWnd_check_filter_Engine_HT;
	HWND hWnd_check_filter_Engine_Z7;
	HWND hWnd_check_filter_Engine_SR;
	HWND hWnd_check_filter_Engine_PRO;
	HWND hWnd_check_filter_attribute_red;
	HWND hWnd_check_filter_attribute_gold;
	HWND hWnd_check_filter_attribute_purple;
	HWND hWnd_check_filter_attribute_white;
	HWND hWnd_check_filter_upcar;

	HWND hWnd_check_kartup;
	HWND hWnd_check_kartup_showcarnum;
	HWND hWnd_check_kartup_lucky;
	HWND hWnd_check_kartup_lockpage;
	HWND hWnd_combox_kartup_lucky;
	HWND hWnd_edit_kartup_carnum;
	HWND hWnd_edit_kartup_lockpage;
	HWND hWnd_edit_kartup_delay;
	HWND hWnd_check_disasse_kart;
	HWND hWnd_check_disasse_lockpage;
	HWND hWnd_check_disasse_shownum;
	HWND hWnd_edit_disasse_carnum;
	HWND hWnd_edit_disasse_page;
	HWND hWnd_edit_disasse_delay;

	HWND hWnd_edit_log;

	DWORD ExchangeSystemDialog;
	DWORD DisassembleStage;
	DWORD PrKartLevelUp;
	DWORD oldPrKartLevelUp;
	DWORD KartLevelUpDialog;
	DWORD KartLevelUpResultDialog;
	DWORD KartLevelUpResultDialog_animati;

	DWORD 合成计数 = 0;
	DWORD 取车子引擎参数;
	DWORD 取车子引擎函数;
	DWORD 取物品名call;

	DWORD 合成结果地址;
	DWORD 合成结果地址1;

	DWORD 合成过滤车辆;
	DWORD 合成过滤车辆跳转;
	DWORD 合成过滤车辆完毕;
	DWORD 合成过滤车辆完毕1;

	DWORD 合成过滤车辆1;
	DWORD 合成过滤车辆跳转1;

	DWORD 取车子升级信息;
	DWORD 取车子升级信息参数;

	DWORD 合成_过掉确认框;
	DWORD64 合成_过掉确认框原始数据;
	DWORD 合成_翻页CALL;
	DWORD 合成_判断车子;
	DWORD 合成_判断车子跳转;
	DWORD 合成_开始合成;
	DWORD 合成_开始合成跳转;
	DWORD 合成_车子偏移;
	DWORD 合成_页数偏移;
	DWORD 合成_锁定合成车子数;
	DWORD 合成_动画速度;

	DWORD 升级_禁止删除待升级车辆;
	DWORD 升级_过滤车子;
	DWORD 升级_过滤车子跳转;
	DWORD 升级_过滤车子完毕;
	DWORD 升级_提高幸运点;

	DWORD 改装_过滤车辆;
	DWORD 改装_过滤车辆跳转;
	DWORD 加点_过滤车辆;
	DWORD 加点_过滤车辆跳转;
	DWORD 部件_过滤车辆;
	DWORD 部件_过滤车辆跳转;

	DWORD 分解_过滤车辆;
	DWORD 分解_过滤车辆跳转;
	DWORD 分解_过滤车辆完毕;
	DWORD 分解_禁用撤销;
	DWORD 分解_跳过分解动画;
	DWORD 分解_剩余计数偏移;
	DWORD 分解_跳过信息提示;
	DWORD 分解_跳过信息提示_手续费;

	struct kartupslot
	{
		DWORD garagecard;
		DWORD itemkart;

	} kartupslotinfo;

	struct Filtercar
	{
		BOOL Filter_upcar;
		BOOL Filter_V1;
		BOOL Filter_X;
		BOOL Filter_9;
		BOOL Filter_HTNew;
		BOOL Filter_HT;
		BOOL Filter_Z7;
		BOOL Filter_SR;
		BOOL Filter_PRO;
		BOOL Filter_ultimate;
		BOOL Filter_rare;
		BOOL Filter_advanced;
		BOOL Filter_general;
		BOOL Filter_listhide;
		BOOL Filter_listshow;
	} g_Filtercar = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

	HANDLE hThread_Exchange;
	HANDLE hThread_kartlevelup;
	HANDLE hThread_disasse;

	typedef struct _ExchangeData
	{
		int 每次合成车子数量;
		int 幸运值;
		bool 判断合成结果;
		int 列表车子数量;
		int 锁定页数;
		int 延时;
		BOOL 开始启动;
		int 车子计数;
		int 枚举数量;
	} ExchangeData;
	typedef struct _合成坐标信息
	{
		POINT 车辆显示坐标;
		POINT 开始合成坐标;
		POINT 通用是否坐标;
		POINT 合成确定坐标1;
		POINT 合成确定坐标2;
		POINT 升级开始坐标;
		POINT 升级确定坐标;
		POINT 分解开始坐标;

	} 合成坐标信息;

	合成坐标信息 合成坐标;
	ExchangeData ed;
	std::vector<POINT> 坐标数组;
	std::vector<POINT> 分解坐标数组;

	std::vector<DWORD> 过滤车子列表;

	void 开始停止合成(BOOL status);
	void 开始停止车辆升级(BOOL status);

	HWND WINAPI GetGameHwnd()
	{
		HWND result = 0;
		WCHAR classNam[MAX_PATH];
		memset(classNam, 0, sizeof(classNam));
		DWORD ProcessId = 0;
		for (HWND hWnd = FindWindowEx(0, 0, nullptr, nullptr);
			 hWnd != 0;
			 hWnd = FindWindowEx(0, hWnd, nullptr, nullptr))
		{
			GetWindowThreadProcessId(hWnd, &ProcessId);
			if (ProcessId == GetCurrentProcessId())
			{
				GetClassName(hWnd, classNam, MAX_PATH * sizeof(WCHAR));
				if (lstrcmpi(classNam, L"PopKart Client") == 0 ||
					lstrcmpi(classNam, L"KartRider Client") == 0 ||
					lstrcmpi(classNam, L" Client") == 0)
					result = hWnd;
			}
		}
		return result;
	}

	DWORD __stdcall getgamestage(DWORD base)
	{
		DWORD result = 0;
		try
		{
			result = *reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(base));
		}
		catch (...)
		{
		}
		return result;
	}

	SHORT __stdcall getupkartcode(DWORD base, DWORD garagecard, DWORD itemkart, DWORD type)
	{
		SHORT result = 0;
		try
		{
			auto gc = *reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(base) + garagecard + (type == 1 ? 0 : 4));
			if (gc != 0)
			{
				auto ik = *reinterpret_cast<DWORD *>(gc + itemkart);
				if (ik != 0)
				{
					result = *reinterpret_cast<SHORT *>(ik + 0xE);
				}
			}
		}
		catch (...)
		{
		}
		return result;
	}

	SHORT __stdcall getExckartcode(DWORD base, DWORD kartof, DWORD s)
	{

		SHORT result = 0;
		try
		{
			result = *reinterpret_cast<SHORT *>(*reinterpret_cast<DWORD *>(base) + kartof + (s * 4));
		}
		catch (...)
		{
		}
		return result;
	}

	BOOL __stdcall getexckartstate(DWORD n)
	{
		DWORD res = n;
		for (size_t i = 0; i < n; i++)
		{
			if (getExckartcode(g_BaseAddress1, 合成_车子偏移, i) != 0)
			{
				res--;
			}
		}
		return res <= 0;
	}

	DWORD __stdcall getdisassecount(DWORD base, DWORD dwCount)
	{
		return *reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(base) + dwCount);
	}

	DWORD __stdcall fn_Thread_disasse(LPVOID param)
	{
		while (hThread_disasse == 0)
			Sleep(10);

		while (hThread_disasse > 0)
		{
			if (getgamestage(g_BaseAddress1) == DisassembleStage)
			{
				if (getdisassecount(g_BaseAddress1, 分解_剩余计数偏移) == 0)
				{
					for (size_t i = 0; i < 16; i++)
					{
						mouse_msg(hGameWnd, 分解坐标数组[i]);
						Sleep(100);
					}
				}
				while (getdisassecount(g_BaseAddress1, 分解_剩余计数偏移) == 0)
				{
					mouse_msg(hGameWnd, 合成坐标.分解开始坐标);
					Sleep(100);
				}
				while (getdisassecount(g_BaseAddress1, 分解_剩余计数偏移) > 0)
					Sleep(10);
			}
			Sleep(ed.延时);
		}

		return 0;
	}

	DWORD __stdcall fn_Thread_kartlevelup(LPVOID param)
	{
		auto n = 1u;
		while (hThread_kartlevelup == 0)
			Sleep(10);

		while (hThread_kartlevelup > 0)
		{
			if (getgamestage(g_BaseAddress1) == KartLevelUpDialog)
			{
				//取升级车辆代码
				if (getupkartcode(g_BaseAddress1, kartupslotinfo.garagecard, kartupslotinfo.itemkart, 1) == 0)
				{
					MessageBox(hDlg, L"请选择要升级的车辆", L"Msg", MB_ICONERROR);
					开始停止车辆升级(FALSE);
					break;
				}

				//取材料车辆代码
				if (getupkartcode(g_BaseAddress1, kartupslotinfo.garagecard, kartupslotinfo.itemkart, 2) == 0)
				{
					n++;
					mouse_msg(hGameWnd, 坐标数组[n]);
					Sleep(50);
				}

				// while (getupkartcode(g_BaseAddress1, kartupslotinfo.garagecard, kartupslotinfo.itemkart, 2) == 0)
				//	Sleep(10);

				mouse_msg(hGameWnd, 合成坐标.升级开始坐标);
				Sleep(100);
			}
			else if (getgamestage(g_BaseAddress1) == KartLevelUpResultDialog)
			{
				mouse_msg(hGameWnd, 合成坐标.升级确定坐标);
				Sleep(100);
			}
			if (n >= 坐标数组.size())
				n = 1;

			Sleep(ed.延时);
		}

		return 0;
	}

	DWORD __stdcall fn_Thread_Exchange(LPVOID param)
	{
		DWORD n = 1;
		while (hThread_Exchange == 0)
			Sleep(10);

		while (hThread_Exchange > 0)
		{
			if (getgamestage(g_BaseAddress1) == ExchangeSystemDialog)
			{
				for (size_t i = 0; i < 4; i++)
				{
					if (getExckartcode(g_BaseAddress1, 合成_车子偏移, i) == 0)
					{
						mouse_msg(hGameWnd, 坐标数组[n]);
						Sleep(100);
					}
					n++;
				}

				while (true)
				{
					//没车子.跳出循环
					if (!getexckartstate(ed.每次合成车子数量))
						break;
					//开始合成
					mouse_msg(hGameWnd, 合成坐标.开始合成坐标);
					Sleep(50);
					mouse_msg(hGameWnd, 合成坐标.合成确定坐标1);
					Sleep(50);
					mouse_msg(hGameWnd, 合成坐标.合成确定坐标2);
					Sleep(50);
				}
			}
			n = 1;
			Sleep(ed.延时);
		}
		return 0;
	}

	BOOL WINAPI 检查分辨率(RECT rc)
	{
		BOOL result = FALSE;
		if (rc.right == 800 || rc.bottom == 600 ||
			rc.right == 1024 || rc.bottom == 768)
			result = TRUE;
		return result;
	}

	BOOL 比较过滤车子代码(DWORD 车子代码, DWORD &索引)
	{
		BOOL result = FALSE;
		for (size_t i = 0; i < 过滤车子列表.size(); i++)
		{
			if (过滤车子列表[i] == 车子代码)
			{
				索引 = i;
				result = TRUE;
				break;
			}
		}
		return result;
	}
	WCHAR *__stdcall 取物品名称(DWORD 物品参数)
	{
		WCHAR *result = nullptr;
		__asm {
			lea ecx, dword ptr[ebp - 0x4];
			push ecx;
			mov ecx, dword ptr[物品参数];
			call dword ptr[取物品名call];
			test eax, eax;
			je label_err;
			mov eax, dword ptr[eax];
			mov dword ptr[result], eax;
		label_err:;
		}
		return result;
	}

	DWORD 取车子属性信息(DWORD code, DWORD thisParam, DWORD fncall)
	{
		DWORD 属性ID = 0, 引擎ID = 0;
		__asm {
			mov eax, dword ptr[code];
			push eax; //代码
			push 0x3; //类型
			mov ecx, dword ptr[thisParam];
			call dword ptr[fncall];
			test eax, eax;
			je label_end;
			xor ecx, ecx;
			movzx ecx, word ptr[eax + 0x19];
			mov dword ptr[属性ID], ecx;
			xor ecx, ecx;
			movzx ecx, word ptr[eax + 0x44];
			mov dword ptr[引擎ID], ecx;
		label_end:;
		}
		return MAKELONG(属性ID, 引擎ID);
	}

	void Edit_AddString(HWND hWnd, LPTSTR lpszText)
	{
		static auto showcount = 0;
		if (showcount > 512)
		{
			showcount = 0;
			SendMessage(hWnd, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(L"\0\0"));
		}
		SendMessage(hWnd, EM_SETSEL, -2, -1);
		SendMessage(hWnd, EM_REPLACESEL, 1, reinterpret_cast<LPARAM>(lpszText));
		showcount++;
	}

	void __stdcall 处理合成结果2(DWORD *指针, DWORD 车子代码, DWORD 物品参数)
	{
		if (*指针 == ExchangeSystemDialog)
		{
			if (ed.判断合成结果)
			{
				DWORD 索引 = -1;
				if (比较过滤车子代码(车子代码, 索引))
				{
					开始停止合成(FALSE);
					MessageBox(hGameWnd, L"合成结果出现过滤车子,停止合成", L"Msg", MB_OK);
				}
			}
			合成计数++;
			WCHAR StringResult[MAX_PATH] = {0};
			swprintf(StringResult, L"第[%d]次合成结果:[%ws]\n", 合成计数, 取物品名称(物品参数));
			Edit_AddString(hWnd_edit_log, const_cast<LPTSTR>(StringResult));
		}
	}

	//处理合成结果
	void __declspec(naked) 处理合成结果1()
	{
		__asm {
			pushad;
			cmp dword ptr[ed.开始启动], 0x1;
			jne label_out;
			push dword ptr ss : [ebp - 0x8];
			movzx ecx, ax;
			push ecx;
			push dword ptr[ebp - 0x4];
			call 处理合成结果2;
		label_out:;
			popad;
			jmp dword ptr[合成结果地址1];
		}
	}
	//处理合成结果
	void __declspec(naked) 处理合成结果()
	{
		__asm {
			pushad;
			cmp dword ptr[ed.开始启动], 0x1;
			jne label_out;
			push dword ptr ss : [ebp - 0x18];
			movzx eax, dx;
			push eax;
			push dword ptr[ebp - 0x4];
			call 处理合成结果2;
		label_out:;
			popad;
			jmp dword ptr[合成结果地址];
		}
	}

	DWORD __stdcall GetKartLevel(DWORD id, DWORD code, DWORD thisParam, DWORD thiscall)
	{
		DWORD result = 0;

		__asm {
			push dword ptr[id];
			push dword ptr[code];
			mov ecx, dword ptr[thisParam];
			call dword ptr[thiscall];
			test eax, eax;
			je label_e;
			mov eax, dword ptr[eax + 0xC];
			mov dword ptr[result], eax;
		label_e:;
		}
		return result;
	}

	void 修正车子计数()
	{

		if (ed.列表车子数量 > 0)
		{
			ed.车子计数--;
		}
	}

	BOOL checkengine(DWORD type, DWORD egid)
	{
		BOOL result = FALSE;
		switch (type)
		{
		case 0: //合成
			result = egid <= 8;
			break;
		case 1: //合成
			result = egid <= 8;
			break;
		case 2: //升级
			result = egid <= 7;
			break;
		case 5: //分解 低于9代车的
			result = egid >= 6;
			break;

		default:
			break;
		}
		return result;
	}

	//返回TRUE 表示不显示 返回FALSE 表示放行显示
	BOOL __stdcall ExChange_Filter(DWORD ecx, DWORD type)
	{
		auto code = *reinterpret_cast<SHORT *>(ecx + 2);
		auto id = *reinterpret_cast<SHORT *>(ecx + 4);
		if (g_Filtercar.Filter_listhide)
		{
			DWORD index = 0;
			if (比较过滤车子代码(code, index))
				return TRUE;
		}
		if (g_Filtercar.Filter_listshow)
		{
			DWORD index = 0;
			if (比较过滤车子代码(code, index))
				return FALSE;
			else
				return TRUE;
		}

		DWORD 属性信息 = 取车子属性信息(code, 取车子引擎参数, 取车子引擎函数);
		DWORD 引擎ID = HIWORD(属性信息), 属性ID = LOWORD(属性信息);

		//[改装][部件]正常情况下不会用到这个
		DWORD engine = type <= 1 ? 8 : type == 2 ? 7
								   : type == 5	 ? 6
												 : 9;

		/*判断V1引擎与期限道具*/
		/*还需要处理[锁定车辆]-[已装备车辆]判断*/
		if (type == 5)
		{
			if (
				引擎ID < engine ||
				*reinterpret_cast<WORD *>(ecx + 0xC) != 0xFFFF)
				return FALSE;
		}
		else
		{
			//过滤掉[部件][改装]的引擎判断
			if (type != 3 || type != 4)
				if (引擎ID >= engine)
					return FALSE;
			//只有[升级][合成][分解]才需要判断道具期限
			if (type <= 2 && *reinterpret_cast<WORD *>(ecx + 0xC) != 0xFFFF)
				return FALSE;
		}

		if (type <= 2 || type == 5)
		{
			if (ed.锁定页数 > 0)
			{
				if (ed.枚举数量 < ed.锁定页数 * 20)
				{
					if (checkengine(type, 引擎ID))
					{
						++ed.枚举数量;
						return TRUE;
					}
				}
			}
		}

		if (ed.列表车子数量 > 0)
		{
			if (ed.车子计数 >= ed.列表车子数量)
			{
				return TRUE;
			}
			else
			{
				if (checkengine(type, 引擎ID))
				{
					ed.车子计数++;
				}
			}
		}

		if (g_Filtercar.Filter_upcar)
		{
			auto KartLevel = GetKartLevel(id, code, 取车子升级信息参数, 取车子升级信息);
			if (KartLevel > 0)
			{
				修正车子计数();
				return TRUE;
			}
		}
		if (引擎ID == 0 || 属性ID == 0)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_V1 && 引擎ID == 8)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_X && 引擎ID == 7)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_9 && 引擎ID == 6)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_HTNew && 引擎ID == 5)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_HT && 引擎ID == 4)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_Z7 && 引擎ID == 3)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_SR && 引擎ID == 2)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_PRO && 引擎ID == 1)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_ultimate && 属性ID == 1)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_rare && 属性ID == 2)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_advanced && 属性ID == 3)
		{
			修正车子计数();
			return TRUE;
		}
		if (g_Filtercar.Filter_general && 属性ID == 4)
		{
			修正车子计数();
			return TRUE;
		}

		return FALSE;
	}

	//跳过升级的车子
	void __declspec(naked) 处理车辆过滤()
	{
		__asm {
			pushad;
			// cmp dword ptr[ed.开始合成], 0x1;
			// jne label_out;
			push 0x1;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[合成过滤车辆跳转];
		label_out:;
			popad;
			jmp dword ptr[合成过滤车辆];
		}
	}
	//跳过升级的车子
	void __declspec(naked) 处理车辆过滤1()
	{
		__asm {
			pushad;
			// cmp dword ptr[ed.开始合成], 0x1;
			// jne label_out;
			push 0x0;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[合成过滤车辆跳转1];
		label_out:;
			popad;
			jmp dword ptr[合成过滤车辆1];
		}
	}

	void __declspec(naked) 合成_车辆读取完毕()
	{
		__asm {
			xor ecx, ecx;
			mov dword ptr[ed.车子计数], ecx;
			mov dword ptr[ed.枚举数量], ecx;
			jmp dword ptr[合成过滤车辆完毕];
		}
	}

	void __declspec(naked) 合成_车辆读取完毕1()
	{
		__asm {
			xor ecx, ecx;
			mov dword ptr[ed.车子计数], ecx;
			mov dword ptr[ed.枚举数量], ecx;
			jmp dword ptr[合成过滤车辆完毕1];
		}
	}
	//开始合成->过滤车子列表
	BOOL __stdcall 处理判断车子1(DWORD 车子代码)
	{

		DWORD 索引 = -1;
		BOOL result = 比较过滤车子代码(车子代码, 索引);
		if (result)
		{
			开始停止合成(FALSE);
			MessageBox(hGameWnd, L"合成出现过滤车辆,停止合成", L"Msg", MB_OK);
		}
		return result;
	}
	//开始合成时判断过滤列表
	void __declspec(naked) 处理判断车子()
	{
		__asm {
			pushad;
			test eax, eax;
			je label_out;
			cmp dword ptr[ed.开始启动], 0x1;
			jne label_out;
			push eax;
			call 处理判断车子1;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[合成_判断车子跳转];
		label_out:;
			popad;
			jmp dword ptr[合成_判断车子];
		}
	}

	//开始合成时修改幸运值
	void __declspec(naked) 处理开始合成()
	{
		__asm {
			pushad;
			cmp dword ptr[ed.开始启动], 0x1;
			jne label_out;
			mov eax, dword ptr[ebp - 0x8];
			mov edx, dword ptr[ed.幸运值];
			mov dword ptr[eax + 0x30], edx;
			mov dword ptr[eax + 0x14], edx;
		label_out:;
			popad;
			jmp dword ptr[合成_开始合成];
		}
	}

	void __declspec(naked) 加点_车辆读取处理()
	{
		__asm {
			pushad;
			push 0x2;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[加点_过滤车辆跳转];
		label_out:;
			popad;
			jmp dword ptr[加点_过滤车辆];
		}
	}

	void __declspec(naked) 改装_车辆读取处理()
	{
		__asm {
			pushad;
			push 0x3;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[改装_过滤车辆跳转];
		label_out:;
			popad;
			jmp dword ptr[改装_过滤车辆];
		}
	}

	void __declspec(naked) 部件_车辆读取处理()
	{
		__asm {
			pushad;
			push 0x4;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[部件_过滤车辆跳转];
		label_out:;
			popad;
			jmp dword ptr[部件_过滤车辆];
		}
	}

	void __declspec(naked) 分解_车辆读取处理()
	{
		__asm {
			pushad;
			// cmp dword ptr[ed.开始启动], 0x1;
			// jne label_out;
			push 0x5;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[分解_过滤车辆跳转];
		label_out:;
			popad;
			jmp dword ptr[分解_过滤车辆];
		}
	}

	void __declspec(naked) 分解_车辆读取完毕()
	{
		__asm {
			xor ecx, ecx;
			mov dword ptr[ed.车子计数], ecx;
			mov dword ptr[ed.枚举数量], ecx;
			jmp dword ptr[分解_过滤车辆完毕];
		}
	}

	void __declspec(naked) 升级_车辆读取处理()
	{
		__asm {
			pushad;
			push 0x2;
			push eax;
			call ExChange_Filter;
			test eax, eax;
			je label_out;
			popad;
			jmp dword ptr[升级_过滤车子跳转];
		label_out:;
			popad;
			jmp dword ptr[升级_过滤车子];
		}
	}

	void __declspec(naked) 升级_车辆读取完毕()
	{
		__asm {
			xor ecx, ecx;
			mov dword ptr[ed.车子计数], ecx;
			mov dword ptr[ed.枚举数量], ecx;
			jmp dword ptr[升级_过滤车子完毕];
		}
	}

	BOOL __stdcall 开始升级过滤(DWORD packet)
	{
		auto carcode = *reinterpret_cast<WORD *>(packet + 0x14);
		DWORD 索引 = -1;
		BOOL result = 比较过滤车子代码(carcode, 索引);
		if (result)
		{
			开始停止车辆升级(FALSE);
			MessageBox(hGameWnd, L"升级材料出现过滤车辆,停止升级", L"Msg", MB_OK);
		}
		return result;
	}

	void __declspec(naked) 升级_提高幸运点处理()
	{
		__asm {
			pushad;
			cmp dword ptr[ed.开始启动], 0x1;
			jne label_out;
			mov eax, dword ptr[esp + 0x20];
			mov edx, dword ptr[ed.幸运值];
			mov dword ptr[eax + 0x18], edx;
			push eax;
			call 开始升级过滤;
			test eax, eax;
			je label_out;
			popad;
			add esp, 0x4;
			mov eax, dword ptr[升级_提高幸运点];
			add eax, 0x5;
			jmp eax;
		label_out:;
			popad;
			jmp dword ptr[升级_提高幸运点];
		}
	}

	void __stdcall PrKartLevelUp_Filter(DWORD id, DWORD data, DWORD length)
	{

		struct MyStruct
		{
			DWORD t;
			DWORD result;	// 1=升级成功 2=升级失败 5=满级
			DWORD usestuff; //材料车辆
			WORD level;
			WORD point;
			DWORD un1;
			DWORD un2;
			DWORD un3;
			DWORD kartid;
		} *KartUpgrade = reinterpret_cast<MyStruct *>(data);
		// KartUpgrade->level = 1;
		// KartUpgrade->result = 2;

		if (KartUpgrade->result == 5 || KartUpgrade->level == 5)
		{
			开始停止车辆升级(FALSE);
		}

		// DbgPrint2(L"result:[%d],level:[%d],point:[%d]\n", KartUpgrade->result, KartUpgrade->level, KartUpgrade->point);
		// KartUpgrade->level = 1;
		// KartUpgrade->result = 2;
	}

	void __declspec(naked) PrKartLevelUp_Revc()
	{
		__asm {
			pushad;
			mov ecx, dword ptr[esp + 0x2C];
			mov eax, dword ptr[esp + 0x24];
			mov eax, dword ptr[eax + 0x08];
			mov edx, dword ptr[eax + 0x0c];
			sub edx, dword ptr[eax + 0x08];
			push edx; //封包长度
			mov eax, dword ptr[eax + 0x04];
			push eax; //封包内容
			push ecx; //封包标识
			call PrKartLevelUp_Filter;
			popad;
			jmp dword ptr[oldPrKartLevelUp];
		}
	}

	void __declspec(naked) fnKartLevelUpResultDialog_animati()
	{
		__asm {		
			cmp dword ptr[ed.开始启动],0x1;
			jne label_skip;
			sub eax, 0x1F40;
		label_skip:;	
			jmp dword ptr[KartLevelUpResultDialog_animati];
		}
	}

	void 鼠标移动(HWND hwnd, int x, int y)
	{
		POINT poi;
		memset(&poi, 0, sizeof(POINT));
		ClientToScreen(hwnd, &poi);
		SetCursorPos(poi.x + x, poi.y + y);
	}

	void 初始化数据()
	{
		try
		{
			DWORD hModule = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
			PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
			IMAGE_NT_HEADERS *nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(hModule + dos_header->e_lfanew);
			DWORD stopaddr = hModule + nt_header->OptionalHeader.SizeOfImage;
			DWORD dwBaseOfData, dwSizeOfData;
			if ((dwBaseOfData = nt_header->OptionalHeader.BaseOfData) == 0 ||
				(dwSizeOfData = nt_header->OptionalHeader.SizeOfInitializedData) == 0)
			{
				MessageBox(hGameWnd, L"获取数据地址失败", L"Msg", MB_ICONERROR);
				return;
			}
			DWORD pSearchBegin = hModule + dwBaseOfData;
			ExchangeSystemDialog = FindPacketHeader("ExchangeSystemDialog", pSearchBegin, pSearchBegin + dwSizeOfData) + 4;
			DbgPrint2(L"ExchangeSystemDialog:[%08X]\n", ExchangeSystemDialog);
			DisassembleStage = FindPacketHeader("DisassembleStage", pSearchBegin, pSearchBegin + dwSizeOfData) + 4;
			DbgPrint2(L"DisassembleStage:[%08X]\n", DisassembleStage);

			PrKartLevelUp = FindPacketHeader("PrKartLevelUp", pSearchBegin, pSearchBegin + dwSizeOfData) + 0x10;
			oldPrKartLevelUp = *reinterpret_cast<DWORD *>(PrKartLevelUp);

			*reinterpret_cast<DWORD *>(PrKartLevelUp) = reinterpret_cast<DWORD>(PrKartLevelUp_Revc);
			DbgPrint2(L"PrKartLevelUp:[%08X],oldPrKartLevelUp:[%08X]\n", PrKartLevelUp, oldPrKartLevelUp);
			KartLevelUpDialog = FindPacketHeader("KartLevelUpDialog", pSearchBegin, pSearchBegin + dwSizeOfData) + 0x4;

			KartLevelUpResultDialog = FindPacketHeader("KartLevelUpResultDialog", pSearchBegin, pSearchBegin + dwSizeOfData) + 4;

			KartLevelUpResultDialog_animati = *reinterpret_cast<DWORD *>(KartLevelUpResultDialog + 0x24) + 0x25;

			DbgPrint2(L"KartLevelUpResultDialog:[%08X],KartLevelUpResultDialog_animati:[%08X]\n", KartLevelUpResultDialog, KartLevelUpResultDialog_animati);

			std::vector<DWORD> foundAddress;

			auto temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "0F 84 3D 01 00 00 51 54 B9", foundAddress);
			g_BaseAddress = *reinterpret_cast<DWORD *>(GetCallAddress(temp + 13) + 8) + 52;
			;
			DbgPrint2(L"g_BaseAddress:[%08X]\n", g_BaseAddress);
			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "8B 82 ** ** 00 00 50 6A 01 B9", foundAddress);
			g_BaseAddress1 = *reinterpret_cast<DWORD *>(GetCallAddress(temp + 14) + 8) + 4;
			;
			DbgPrint2(L"g_BaseAddress1:[%08X]\n", g_BaseAddress1);

			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "7E 43 0F B7 4D 08 51", foundAddress);
			取车子引擎参数 = *reinterpret_cast<PDWORD>(GetCallAddress(temp + 0x10) + 0x8);
			取车子引擎函数 = GetCallAddress(GetCallAddress(temp + 0x17) + 0x13);
			DbgPrint2(L"取车子引擎参数:[%08X]\n", 取车子引擎参数);
			DbgPrint2(L"取车子引擎函数:[%08X]\n", 取车子引擎函数);
			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "74 61 8D 45 D0", foundAddress);
			取物品名call = GetCallAddress(temp + 0x9);
			DbgPrint2(L"取物品名call:[%08X]\n", 取物品名call);
			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "66 8B 48 04 0F B7 D1 52 51 8B CC", foundAddress);
			合成结果地址 = temp - 0x11;
			DbgPrint2(L"合成结果地址:[%08X]\n", 合成结果地址);
			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "75 79 68 FA 00 00 00 68 C8 00 00 00 6A 00", foundAddress);
			合成结果地址1 = temp + 0x1D;
			DbgPrint2(L"合成结果地址1:[%08X]\n", 合成结果地址1);

			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "0F 82 AC 01 00 00 8B 45 E8 3B 45 B8", foundAddress);
			合成过滤车辆 = temp + 0x1A;
			合成过滤车辆跳转 = 合成过滤车辆 - 0x48;
			合成过滤车辆完毕 = 合成过滤车辆 + 0x1AD;

			DbgPrint2(L"合成过滤车辆:[%08X]\n", 合成过滤车辆);
			DbgPrint2(L"合成过滤车辆跳转:[%08X]\n", 合成过滤车辆跳转);
			DbgPrint2(L"合成过滤车辆完毕:[%08X]\n", 合成过滤车辆完毕);
			temp = GetCallAddress(aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "75 14 8B 55 FC 8B 45 E4", foundAddress) - 0x49);

			for (size_t i = 0; i < 0x100; i++)
			{
				if (memcmp(reinterpret_cast<void *>(temp + i), "\x89\x45\xFC", 3) == 0)
				{
					temp += i;
					break;
				}
			}
			合成过滤车辆1 = temp;
			合成过滤车辆跳转1 = 合成过滤车辆1 - 0x27;
			合成过滤车辆完毕1 = 合成过滤车辆1 + 0x25B;

			DbgPrint2(L"合成过滤车辆1:[%08X]\n", 合成过滤车辆1);
			DbgPrint2(L"合成过滤车辆跳转1:[%08X]\n", 合成过滤车辆跳转1);
			DbgPrint2(L"合成过滤车辆完毕1:[%08X]\n", 合成过滤车辆完毕1);

			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "83 7D 14 00 74 19 8B 45 1C 50 8B 4D 14 51", foundAddress);
			取车子升级信息参数 = GetCallAddress(temp + 0x13);
			取车子升级信息参数 = *reinterpret_cast<DWORD *>(取车子升级信息参数 + 0x8);
			DbgPrint2(L"取车子升级信息参数:[%08X]\n", 取车子升级信息参数);

			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "55 8B EC 83 EC 18 89 4D FC 0F B7 45 0C 25 FF FF 00 00 0F B7 C8 0F B7 55 08 81 E2 FF FF 00 00 0F", foundAddress);
			取车子升级信息 = temp;
			DbgPrint2(L"取车子升级信息:[%08X]\n", 取车子升级信息);
			合成_过掉确认框 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "E9 B6 00 00 00 8D 4D D0", foundAddress) - 0x49;
			合成_过掉确认框原始数据 = *reinterpret_cast<DWORD64 *>(合成_过掉确认框);

			DbgPrint2(L"合成_过掉确认框:[%08X]\n", 合成_过掉确认框);
			合成_翻页CALL = GetCallAddress(合成_过掉确认框 - 0xBD);
			合成_判断车子 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "83 7D F0 04 73 36", foundAddress);
			合成_开始合成 = GetJumpAddress(合成_判断车子 + 0x4);
			合成_判断车子 += 0x14;
			合成_判断车子跳转 = 合成_判断车子 + 0x7A;
			DbgPrint2(L"合成_合成_翻页CALL:[%08X]\n", 合成_翻页CALL);
			DbgPrint2(L"合成_判断车子:[%08X]\n", 合成_判断车子);
			DbgPrint2(L"合成_判断车子跳转:[%08X]\n", 合成_判断车子跳转);
			DbgPrint2(L"合成_开始合成:[%08X]\n", 合成_开始合成);
			合成_车子偏移 = *reinterpret_cast<DWORD *>(合成_判断车子 - 0x4);
			合成_页数偏移 = 合成_车子偏移 + 0x64;
			合成_锁定合成车子数 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "83 7D B0 04 75 0F", foundAddress) + 0x3;
			DbgPrint2(L"合成_车子偏移:[%08X]\n", 合成_车子偏移);
			DbgPrint2(L"合成_页数偏移:[%08X]\n", 合成_页数偏移);
			DbgPrint2(L"合成_锁定合成车子数:[%08X]\n", 合成_锁定合成车子数);

			升级_禁止删除待升级车辆 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "74 19 51 8B CC 6A 01 6A 00", foundAddress);
			DbgPrint2(L"升级_禁止删除待升级车辆:[%08X]\n", 升级_禁止删除待升级车辆);

			升级_过滤车子 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "66 8B 40 44 66 89 45 E4", foundAddress) - 0x27;

			升级_过滤车子跳转 = 升级_过滤车子 - 0x30;
			升级_过滤车子完毕 = 升级_过滤车子 + 0x10A;
			DbgPrint2(L"升级_过滤车子完毕:[%08X]\n", 升级_过滤车子完毕);
			DbgPrint2(L"升级_过滤车子:[%08X]\n", 升级_过滤车子);

			DbgPrint2(L"升级_过滤车子跳转:[%08X]\n", 升级_过滤车子跳转);
			升级_提高幸运点 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "89 48 18 51 8B CC 0F B6 55 FC", foundAddress) + 0x27;

			DbgPrint2(L"升级_提高幸运点:[%08X]\n", 升级_提高幸运点);

			temp = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "74 07 32 C0 E9 9A 02 00 00", foundAddress);
			kartupslotinfo.garagecard = *reinterpret_cast<DWORD *>(temp + 0x10);
			temp = GetCallAddress(temp + 0x1B);
			kartupslotinfo.itemkart = *reinterpret_cast<DWORD *>(temp + 0xE) + 0x1C;

			DbgPrint2(L"kartupslotinfo.garagecard:[%08X],kartupslotinfo.itemkart[%08X]\n", kartupslotinfo.garagecard, kartupslotinfo.itemkart);
			合成_动画速度 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "3D 02 08 00 00 76 31", foundAddress) + 0x1;

			改装_过滤车辆 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "74 02 EB B3 8B 4D FC 0F B7 51 02", foundAddress) - 0x1F;
			改装_过滤车辆跳转 = 改装_过滤车辆 - 0x2A;
			DbgPrint2(L"改装_过滤车辆:[%08X]\n", 改装_过滤车辆);
			DbgPrint2(L"改装_过滤车辆跳转:[%08X]\n", 改装_过滤车辆跳转);
			加点_过滤车辆 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "89 45 F4 8B 4D F4 0F B7 51 02", foundAddress);
			加点_过滤车辆跳转 = 加点_过滤车辆 - 0x30;
			DbgPrint2(L"加点_过滤车辆:[%08X]\n", 加点_过滤车辆);
			DbgPrint2(L"加点_过滤车辆跳转:[%08X]\n", 加点_过滤车辆跳转);

			部件_过滤车辆 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "8B 4D EC 0F B7 71 02 6A 03", foundAddress) - 0x23;
			部件_过滤车辆跳转 = 部件_过滤车辆 - 0x30;
			DbgPrint2(L"部件_过滤车辆:[%08X]\n", 部件_过滤车辆);
			DbgPrint2(L"部件_过滤车辆跳转:[%08X]\n", 部件_过滤车辆跳转);

			分解_过滤车辆 = aobscan(GetCurrentProcess(), 2, hModule, stopaddr, "89 45 F8 8B 45 F8 0F B7 48 02 51 8D 55", foundAddress);
			分解_过滤车辆跳转 = 分解_过滤车辆 - 0x30;
			分解_过滤车辆完毕 = 分解_过滤车辆 - 0xE;
			分解_过滤车辆完毕 = 分解_过滤车辆完毕 + *reinterpret_cast<DWORD *>(分解_过滤车辆完毕 + 2) + 6;
			DbgPrint2(L"分解_过滤车辆:[%08X]\n", 分解_过滤车辆);
			DbgPrint2(L"分解_过滤车辆跳转:[%08X]\n", 分解_过滤车辆跳转);
			DbgPrint2(L"分解_过滤车辆完毕:[%08X]\n", 分解_过滤车辆完毕);

			分解_禁用撤销 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "74 22 6A 00 6A 00 51 8B CC 6A 01", foundAddress) - 0x28;
			DbgPrint2(L"分解_禁用撤销:[%08X]\n", 分解_禁用撤销);
			分解_跳过分解动画 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "69 D0 F4 01 00 00 39 55", foundAddress) + 0x9;
			分解_剩余计数偏移 = *reinterpret_cast<DWORD *>(分解_跳过分解动画 - 0x19) + 0x20;

			DbgPrint2(L"分解_跳过分解动画:[%08X]\n", 分解_跳过分解动画);
			DbgPrint2(L"分解_剩余计数偏移:[%08X]\n", 分解_剩余计数偏移);

			分解_跳过信息提示 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "0F B6 ** FA ** ** 0F 84 C3 00 00 00 51", foundAddress) + 0x6;

			分解_跳过信息提示_手续费 = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "0F 82 DC 01 00 00", foundAddress) + 0xED;

			DbgPrint2(L"分解_跳过信息提示:[%08X]\n", 分解_跳过信息提示);
			DbgPrint2(L"分解_跳过信息提示_手续费:[%08X]\n", 分解_跳过信息提示_手续费);

			detours::redirect(true, &(PVOID &)合成结果地址, 处理合成结果);
			detours::redirect(true, &(PVOID &)合成结果地址1, 处理合成结果1);
			detours::redirect(true, &(PVOID &)合成过滤车辆, 处理车辆过滤);
			detours::redirect(true, &(PVOID &)合成过滤车辆1, 处理车辆过滤1);
			detours::redirect(true, &(PVOID &)合成_判断车子, 处理判断车子);
			detours::redirect(true, &(PVOID &)合成_开始合成, 处理开始合成);
			detours::redirect(true, &(PVOID &)合成过滤车辆完毕, 合成_车辆读取完毕);
			detours::redirect(true, &(PVOID &)合成过滤车辆完毕1, 合成_车辆读取完毕1);
			detours::redirect(true, &(PVOID &)升级_过滤车子, 升级_车辆读取处理);
			detours::redirect(true, &(PVOID &)升级_过滤车子完毕, 升级_车辆读取完毕);
			detours::redirect(true, &(PVOID &)升级_提高幸运点, 升级_提高幸运点处理);
			detours::redirect(true, &(PVOID &)KartLevelUpResultDialog_animati, fnKartLevelUpResultDialog_animati);

			detours::redirect(true, &(PVOID &)改装_过滤车辆, 改装_车辆读取处理);
			detours::redirect(true, &(PVOID &)加点_过滤车辆, 加点_车辆读取处理);
			detours::redirect(true, &(PVOID &)部件_过滤车辆, 部件_车辆读取处理);
			detours::redirect(true, &(PVOID &)分解_过滤车辆, 分解_车辆读取处理);
			detours::redirect(true, &(PVOID &)分解_过滤车辆完毕, 分解_车辆读取完毕);
		}
		catch (...)
		{
			MessageBox(hGameWnd, L"数据初始化失败,[ECKUPDSEFE].请反馈给作者修复", L"Msg", MB_ICONERROR);
			ExitProcess(0);
			return;
		}
		// hGameWnd = FindWindow(L"PopKart Client", nullptr);
		RECT rc;
		GetClientRect(hGameWnd, &rc);
		// rc.right = 1024;
		// rc.bottom = 768;
		if (!检查分辨率(rc))
		{
			MessageBox(hGameWnd, L"不支持在此分辨率下运行。请更换分辨率 800*600 或 1024*768", L"Msg", MB_ICONERROR);
			ExitProcess(0);
		}

		合成坐标.车辆显示坐标.x = rc.right / 2 - 32;
		合成坐标.车辆显示坐标.y = rc.bottom / 2 - 155;

		合成坐标.开始合成坐标.x = rc.right / 2 - 280;
		合成坐标.开始合成坐标.y = rc.bottom / 2 + 35;
		合成坐标.通用是否坐标.x = rc.right / 2 - 55;
		合成坐标.通用是否坐标.y = rc.bottom / 2 + 45;
		合成坐标.合成确定坐标1.x = rc.right / 2 + 252;
		合成坐标.合成确定坐标1.y = rc.bottom / 2 - 115;
		合成坐标.合成确定坐标2.x = rc.right / 2;
		合成坐标.合成确定坐标2.y = rc.bottom / 1.41f;
		合成坐标.升级开始坐标.x = rc.right / 2 - 290;
		合成坐标.升级开始坐标.y = rc.bottom / 2 + 200;
		合成坐标.升级确定坐标.x = rc.right / 2;
		合成坐标.升级确定坐标.y = rc.bottom / 2 + 160;

		合成坐标.分解开始坐标.x = rc.right / 2 - 200;
		合成坐标.分解开始坐标.y = rc.bottom / 2 + 290;

		POINT kartpos = {rc.right / 2 - 480, rc.bottom / 2 - 165};

		//鼠标移动(hGameWnd, kartpos.x, kartpos.y);

		for (int y = kartpos.y;
			 y < kartpos.y + 90 * 4;
			 y += 90)
		{
			for (int x = kartpos.x;
				 x < kartpos.x + 90 * 4;
				 x += 90)
			{
				DbgPrint2(L"x:[%d],y:[%d]\n", x + 55, y + 55);
				//鼠标移动(hGameWnd, x + 55, y + 55);
				// Sleep(200);
				分解坐标数组.push_back({x + 55, y + 55});
			}
		}

		//鼠标移动(hGameWnd, 合成坐标.合成确定坐标2.x, 合成坐标.合成确定坐标2.y);
		for (int y = 合成坐标.车辆显示坐标.y;
			 y < 合成坐标.车辆显示坐标.y + 81 * 4;
			 y += 81)
		{
			for (int x = 合成坐标.车辆显示坐标.x;
				 x < 合成坐标.车辆显示坐标.x + 81 * 5;
				 x += 81)
			{
				DbgPrint2(L"x:[%d],y:[%d]\n", x + 30, y + 30);
				//鼠标移动(hGameWnd,x+30,y+30);
				// Sleep(500);
				坐标数组.push_back({x + 30, y + 30});
			}
		}
	}

	void 开始停止合成(BOOL status)
	{

		for (size_t i = IDC_COMBO_carnum; i < IDC_EDIT4_log; i++)
			EnableWindow(GetDlgItem(hDlg, i), !status);

		Button_SetCheck(hWnd_check_begin, status);

		if (status)
		{

			// for (int i = 0; i < ListView_GetItemCount(hWnd_list_filter); i++)
			//{
			//	WCHAR buffer[MAX_PATH] = { 0 };
			//	ListView_GetItemText(hWnd_list_filter, i, 0, buffer, MAX_PATH * sizeof(WCHAR));
			//	DbgPrint2(L"车子名[%ws]\n", buffer);
			//	ListView_GetItemText(hWnd_list_filter, i, 1, buffer, MAX_PATH * sizeof(WCHAR));
			//	DbgPrint2(L"车子代码[%ws]\n", buffer);
			//	auto code = _wtoi(buffer);
			//	过滤车子列表.push_back(code);
			// }
			// DbgPrint2(L"size[%d]\n", 过滤车子列表.size());

			BOOL lpTranslated = FALSE;
			ed.延时 = GetDlgItemInt(hDlg, IDC_EDIT3_delay, &lpTranslated, FALSE);
			ed.列表车子数量 = Button_GetCheck(hWnd_check_showcarnum) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT1_carnum, &lpTranslated, FALSE);
			ed.每次合成车子数量 = ComboBox_GetCurSel(hWnd_combox_carnum) + 2;
			ed.判断合成结果 = Button_GetCheck(hWnd_check_cchresult) == 1;
			ed.锁定页数 = Button_GetCheck(hWnd_check_lockpage) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT_lockpage, &lpTranslated, FALSE);
			ed.锁定页数 = ed.锁定页数 - 1 <= 0 ? 0 : ed.锁定页数 - 1;

			ed.幸运值 = Button_GetCheck(hWnd_check_lucky) == 0 ? 0 : ComboBox_GetCurSel(hWnd_combox_carnum) + 1;
			ed.开始启动 = status;
			*reinterpret_cast<BYTE *>(合成_锁定合成车子数) = ed.每次合成车子数量;

			*reinterpret_cast<DWORD64 *>(合成_过掉确认框) = 0x90909000000001B8;
			*reinterpret_cast<DWORD *>(合成_动画速度) = 0x0;

			DbgPrint2(L"合成延时:[%d],合成列表车子数量:[%d],每次合成车子数量:[%d],判断合成结果:[%d],锁定页数:[%d],幸运值:[%d] \n",
					  ed.延时,
					  ed.列表车子数量,
					  ed.每次合成车子数量,
					  ed.判断合成结果,
					  ed.锁定页数,
					  ed.幸运值);
			hThread_Exchange = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fn_Thread_Exchange), nullptr, 0, nullptr);
		}
		else
		{
			if (hThread_Exchange > 0)
			{
				TerminateThread(hThread_Exchange, 0);
				CloseHandle(hThread_Exchange);
				hThread_Exchange = 0;
			}

			*reinterpret_cast<DWORD *>(合成_动画速度) = 2050;
			*reinterpret_cast<BYTE *>(合成_锁定合成车子数) = 4;
			*reinterpret_cast<DWORD64 *>(合成_过掉确认框) = 合成_过掉确认框原始数据;
			memset(&ed, 0, sizeof(ExchangeData));
		}
	}

	void 开始停止车辆升级(BOOL status)
	{
		for (size_t i = IDC_CHECK_kartup_lucky; i < IDC_EDIT_kartup_delay + 1; i++)
			EnableWindow(GetDlgItem(hDlg, i), !status);
		Button_SetCheck(hWnd_check_kartup, status);

		if (status)
		{
			BOOL lpTranslated = FALSE;
			ed.延时 = GetDlgItemInt(hDlg, IDC_EDIT_kartup_delay, &lpTranslated, FALSE);
			ed.列表车子数量 = Button_GetCheck(hWnd_check_kartup_showcarnum) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT1_carnum2, &lpTranslated, FALSE);
			ed.锁定页数 = Button_GetCheck(hWnd_check_kartup_lockpage) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT_kartup_lockpage, &lpTranslated, FALSE);
			ed.锁定页数 = ed.锁定页数 - 1 <= 0 ? 0 : ed.锁定页数 - 1;
			ed.幸运值 = Button_GetCheck(hWnd_check_kartup_lucky) == 0 ? 0 : ComboBox_GetCurSel(hWnd_combox_kartup_lucky) + 1;
			ed.幸运值 = ed.幸运值 == 1 ? 5 : ed.幸运值 == 2 ? 10
															: 0;

			ed.开始启动 = status;

			*reinterpret_cast<BYTE *>(升级_禁止删除待升级车辆) = 0xeb;

			DbgPrint2(L"延时:[%d],列表车子数量:[%d],锁定页数:[%d],幸运值:[%d] \n",
					  ed.延时,
					  ed.列表车子数量,
					  ed.锁定页数,
					  ed.幸运值);

			hThread_kartlevelup = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fn_Thread_kartlevelup), nullptr, 0, nullptr);
		}
		else
		{
			if (hThread_kartlevelup > 0)
			{
				TerminateThread(hThread_kartlevelup, 0);
				CloseHandle(hThread_kartlevelup);
				hThread_kartlevelup = 0;
			}
			*reinterpret_cast<BYTE *>(升级_禁止删除待升级车辆) = 0x74;
			memset(&ed, 0, sizeof(ExchangeData));
		}
	}

	void 开始停止分解车辆(BOOL status)
	{
		for (size_t i = IDC_CHECK_disasse_lockpage; i < IDC_CHECK_filter_show; i++)
			EnableWindow(GetDlgItem(hDlg, i), !status);
		Button_SetCheck(hWnd_check_disasse_kart, status);

		if (status)
		{
			BOOL lpTranslated = FALSE;
			ed.延时 = GetDlgItemInt(hDlg, IDC_EDIT_disasse_delay, &lpTranslated, FALSE);
			ed.列表车子数量 = Button_GetCheck(hWnd_check_disasse_shownum) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT_disasse_carnum, &lpTranslated, FALSE);
			ed.锁定页数 = Button_GetCheck(hWnd_check_disasse_lockpage) == 0 ? 0 : GetDlgItemInt(hDlg, IDC_EDIT_disasse_page, &lpTranslated, FALSE);
			ed.锁定页数 = ed.锁定页数 - 1 <= 0 ? 0 : ed.锁定页数 - 1;

			ed.开始启动 = status;

			*reinterpret_cast<DWORD *>(分解_跳过分解动画 - 7) = 0x0;
			*reinterpret_cast<BYTE *>(分解_禁用撤销 + 1) = 0x0;

			*reinterpret_cast<DWORD *>(分解_跳过信息提示) = 0x0000C4E9;
			*reinterpret_cast<DWORD64 *>(分解_跳过信息提示_手续费) = 0x04110F000000AFE9;

			DbgPrint2(L"延时:[%d],列表车子数量:[%d],锁定页数:[%d],幸运值:[%d] \n",
					  ed.延时,
					  ed.列表车子数量,
					  ed.锁定页数,
					  ed.幸运值);

			hThread_disasse = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(fn_Thread_disasse), nullptr, 0, nullptr);
		}
		else
		{
			if (hThread_disasse > 0)
			{
				TerminateThread(hThread_disasse, 0);
				CloseHandle(hThread_disasse);
				hThread_disasse = 0;
			}
			*reinterpret_cast<DWORD *>(分解_跳过分解动画 - 7) = 0x1F4;
			*reinterpret_cast<BYTE *>(分解_禁用撤销 + 1) = 0x54;
			*reinterpret_cast<DWORD *>(分解_跳过信息提示) = 0x00C3840F;
			*reinterpret_cast<DWORD64 *>(分解_跳过信息提示_手续费) = 0x04110FF3C0570F51;
			memset(&ed, 0, sizeof(ExchangeData));
		}
	}

	void ExChange_JumpPage(DWORD base, DWORD fncall, DWORD pageof, DWORD index)
	{

		__asm {
			mov eax, dword ptr[base];
			mov eax, dword ptr[eax];
			add eax, dword ptr[pageof];
			mov edx, dword ptr[index];
			add edx, 0x1;
			mov dword ptr [eax], edx;
			push 0x0;
			sub eax, dword ptr[pageof];
			mov ecx, eax;
			call dword ptr[fncall];
		}
	}

	void ExChange_JumpPage(DWORD index)
	{
		if (getgamestage(g_BaseAddress1) == ExchangeSystemDialog)
		{
			ExChange_JumpPage(g_BaseAddress1, 合成_翻页CALL, 合成_页数偏移, index);
		}
	}

	BOOL listview_addordel(DWORD code, BOOL type)
	{
		DWORD pos = -1;
		for (auto it = 过滤车子列表.begin(); it != 过滤车子列表.end(); it++)
		{
			if (*it == code)
			{
				if (type == 1) //添加车子代码
				{
					pos = 1;
					MessageBox(hDlg, L"车子代码重复,请勿重复添加", L"Msg", MB_ICONERROR);
					break;
				}
				else
				{
					//删除车子代码
					过滤车子列表.erase(it);
					break;
				}
			}
		}
		if (type == 1 && pos != 1)
		{
			过滤车子列表.push_back(code);
		}

		DbgPrint2(L"size[%d]\n", 过滤车子列表.size());

		return pos == -1;
	}

	INT_PTR CALLBACK WndProc_wx(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_INITDIALOG:
		{
			hDlg = hWnd;
			hWnd_list_filter = GetDlgItem(hWnd, IDC_LIST_filter);
			hWnd_combox_carnum = GetDlgItem(hWnd, IDC_COMBO_carnum);
			hWnd_combox_lucky = GetDlgItem(hWnd, IDC_COMBO2_lucky);
			hWnd_edit_carnum = GetDlgItem(hWnd, IDC_EDIT1_carnum);
			hWnd_edit_lockpage = GetDlgItem(hWnd, IDC_EDIT_lockpage);
			hWnd_edit_delay = GetDlgItem(hWnd, IDC_EDIT3_delay);
			hWnd_edit_addcar_name = GetDlgItem(hWnd, IDC_EDIT1_addcar_name);
			hWnd_edit_addcar_code = GetDlgItem(hWnd, IDC_EDIT1_addcar_code);

			hWnd_check_begin = GetDlgItem(hWnd, IDC_CHECK_begin);
			hWnd_check_lucky = GetDlgItem(hWnd, IDC_CHECK2_lucky);
			hWnd_check_cchresult = GetDlgItem(hWnd, IDC_CHECK3_cchresult);
			hWnd_check_showcarnum = GetDlgItem(hWnd, IDC_CHECK4_showcarnum);
			hWnd_check_lockpage = GetDlgItem(hWnd, IDC_CHECK5_lockpage);
			hWnd_check_filter = GetDlgItem(hWnd, IDC_CHECK_filter);

			hWnd_check_filter_show = GetDlgItem(hWnd, IDC_CHECK_filter_show);

			hWnd_check_filter_Engine_V1 = GetDlgItem(hWnd, IDC_CHECK_Engine_V1);
			hWnd_check_filter_Engine_X = GetDlgItem(hWnd, IDC_CHECK_Engine_X);
			hWnd_check_filter_Engine_9 = GetDlgItem(hWnd, IDC_CHECK_Engine_9);
			hWnd_check_filter_Engine_HTNew = GetDlgItem(hWnd, IDC_CHECK_Engine_HTNew);
			hWnd_check_filter_Engine_HT = GetDlgItem(hWnd, IDC_CHECK_Engine_HT);
			hWnd_check_filter_Engine_Z7 = GetDlgItem(hWnd, IDC_CHECK_Engine_Z7);
			hWnd_check_filter_Engine_SR = GetDlgItem(hWnd, IDC_CHECK_Engine_SR);
			hWnd_check_filter_Engine_PRO = GetDlgItem(hWnd, IDC_CHECK_Engine_PRO);
			hWnd_check_filter_attribute_red = GetDlgItem(hWnd, IDC_CHECK_attribute_red);
			hWnd_check_filter_attribute_gold = GetDlgItem(hWnd, IDC_CHECK_attribute_gold);
			hWnd_check_filter_attribute_purple = GetDlgItem(hWnd, IDC_CHECK_attribute_purple);
			hWnd_check_filter_attribute_white = GetDlgItem(hWnd, IDC_CHECK_attribute_white);
			hWnd_check_filter_upcar = GetDlgItem(hWnd, IDC_CHECK_filter_upcar);

			hWnd_check_kartup = GetDlgItem(hWnd, IDC_CHECK_kartup);
			hWnd_check_kartup_showcarnum = GetDlgItem(hWnd, IDC_CHECK4_showcarnum2);
			hWnd_check_kartup_lucky = GetDlgItem(hWnd, IDC_CHECK_kartup_lucky);
			hWnd_check_kartup_lockpage = GetDlgItem(hWnd, IDC_CHECK_kartup_lockpage);
			hWnd_combox_kartup_lucky = GetDlgItem(hWnd, IDC_COMBO_kartup_lucky);
			hWnd_edit_kartup_carnum = GetDlgItem(hWnd, IDC_EDIT1_carnum2);
			hWnd_edit_kartup_lockpage = GetDlgItem(hWnd, IDC_EDIT_kartup_lockpage);
			hWnd_edit_kartup_delay = GetDlgItem(hWnd, IDC_EDIT_kartup_delay);

			hWnd_check_disasse_kart = GetDlgItem(hWnd, IDC_CHECK_disasse_kart);
			hWnd_check_disasse_lockpage = GetDlgItem(hWnd, IDC_CHECK_disasse_lockpage);
			hWnd_check_disasse_shownum = GetDlgItem(hWnd, IDC_CHECK_disasse_shownum);
			hWnd_edit_disasse_carnum = GetDlgItem(hWnd, IDC_EDIT_disasse_carnum);
			hWnd_edit_disasse_page = GetDlgItem(hWnd, IDC_EDIT_disasse_page);
			hWnd_edit_disasse_delay = GetDlgItem(hWnd, IDC_EDIT_disasse_delay);

			hWnd_edit_log = GetDlgItem(hWnd, IDC_EDIT4_log);
			SendMessage(hWnd_edit_log, EM_SETLIMITTEXT, -1, 0);
			SetDlgItemInt(hWnd, IDC_EDIT1_carnum, 10, false);
			SetDlgItemInt(hWnd, IDC_EDIT_lockpage, 2, false);
			SetDlgItemInt(hWnd, IDC_EDIT3_delay, 500, false);
			SetDlgItemInt(hWnd, IDC_EDIT_kartup_lockpage, 1, false);
			SetDlgItemInt(hWnd, IDC_EDIT1_carnum2, 20, false);
			SetDlgItemInt(hWnd, IDC_EDIT_kartup_delay, 500, false);

			SetDlgItemInt(hWnd, IDC_EDIT_disasse_page, 1, false);
			SetDlgItemInt(hWnd, IDC_EDIT_disasse_carnum, 10, false);
			SetDlgItemInt(hWnd, IDC_EDIT_disasse_delay, 500, false);

			ComboBox_InsertString(hWnd_combox_carnum, 0, L"合成2辆");
			ComboBox_InsertString(hWnd_combox_carnum, 1, L"合成3辆");
			ComboBox_InsertString(hWnd_combox_carnum, 2, L"合成4辆");
			ComboBox_InsertString(hWnd_combox_lucky, 0, L"一般");
			ComboBox_InsertString(hWnd_combox_lucky, 1, L"高");
			ComboBox_InsertString(hWnd_combox_lucky, 2, L"极高");
			ComboBox_InsertString(hWnd_combox_kartup_lucky, 0, L"中级");
			ComboBox_InsertString(hWnd_combox_kartup_lucky, 1, L"上级");
			ComboBox_SetCurSel(hWnd_combox_carnum, 0);
			ComboBox_SetCurSel(hWnd_combox_lucky, 0);
			ComboBox_SetCurSel(hWnd_combox_kartup_lucky, 0);

			ListView_SetExtendedListViewStyle(hWnd_list_filter, LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT);
			LVCOLUMN lvc;
			memset(&lvc, 0, sizeof(LVCOLUMN));
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
			lvc.fmt = LVCFMT_CENTER;
			lvc.cx = 100;
			lvc.iSubItem = 0;
			lvc.pszText = L"车子名称";
			lvc.cchTextMax = wcslen(lvc.pszText) * sizeof(wchar_t);
			ListView_InsertColumn(hWnd_list_filter, lvc.iSubItem, &lvc);
			lvc.iSubItem = 1;
			lvc.pszText = L"车子代码";
			lvc.cchTextMax = wcslen(lvc.pszText) * sizeof(wchar_t);
			ListView_InsertColumn(hWnd_list_filter, lvc.iSubItem, &lvc);
			// LVITEM vitem;
			// memset(&vitem, 0, sizeof(LVITEM));
			// vitem.mask = LVIF_TEXT;
			// vitem.iSubItem = 0;
			// ListView_InsertItem(hWnd_list_filter, &vitem);
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 0, L"金刚龙 9");
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 1, L"887");
			// vitem.iItem = ListView_GetItemCount(hWnd_list_filter);
			// ListView_InsertItem(hWnd_list_filter, &vitem);
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 0, L"烈焰马拉松1");
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 1, L"232");
			// vitem.iItem = ListView_GetItemCount(hWnd_list_filter);
			// ListView_InsertItem(hWnd_list_filter, &vitem);
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 0, L"烈焰马拉松12");
			// ListView_SetItemText(hWnd_list_filter, vitem.iItem, 1, L"2323");

			memset(&ed, 0, sizeof(ExchangeData));
			memset(&合成坐标, 0, sizeof(合成坐标信息));
			hGameWnd = GetGameHwnd();
			初始化数据();
		}
		break;

		case WM_NOTIFY:
		{
			if (lParam)
			{
				LPNMHDR pNMhdr = reinterpret_cast<LPNMHDR>(lParam);
				switch (pNMhdr->idFrom)
				{
				case IDC_LIST_filter:
				{
					if (pNMhdr->code == LVN_KEYDOWN)
					{
						LPNMLVKEYDOWN pNMLVKey = reinterpret_cast<LPNMLVKEYDOWN>(lParam);
						if (pNMLVKey->wVKey == VK_DELETE)
						{
							auto id = ListView_GetSelectionMark(hWnd_list_filter);
							if (id != -1)
							{
								WCHAR buffer[MAX_PATH] = {0};
								ListView_GetItemText(hWnd_list_filter, id, 1, buffer, MAX_PATH * sizeof(WCHAR));
								// DbgPrint2(L"buffer:[%ws]\n",buffer);
								auto code = _wtoi(buffer);
								// DbgPrint2(L"code:[%d]\n", code);
								listview_addordel(code, 2);
								ListView_DeleteItem(hWnd_list_filter, id);
							}

							// DbgPrint2(L"id[%d]\n", id);
						}
						// DbgPrint2(L"wVKey[%d],flags[%d]\n", pNMLVKey->wVKey, pNMLVKey->flags);
					}
					if (pNMhdr->code == LVN_DELETEITEM)
					{
						// LPNMLISTVIEW pNMLVListView = reinterpret_cast<LPNMLISTVIEW>(lParam);
					}
					if (pNMhdr->code == LVN_INSERTITEM)
					{
					}

					// DbgPrint2(L"通知,code[%d],idFrom[%d],hwndFrom[%d]\n", pNMhdr->code, pNMhdr->idFrom, pNMhdr->hwndFrom);
				}
				break;
				default:
					break;
				}
			}
		}
		break;
		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);
			WORD wNF = HIWORD(wParam);

			// DbgPrint2(L"wID[%d],wNF[%d]\n", wID, wNF);

			switch (wID)
			{
			case IDC_CHECK_begin:
			{
				if (wNF == BN_CLICKED)
				{
					auto status = Button_GetCheck(hWnd_check_begin) == 1;
					开始停止合成(status);
				}
			}
			break;
			case IDC_CHECK_kartup:
			{
				if (wNF == BN_CLICKED)
				{
					auto status = Button_GetCheck(hWnd_check_kartup) == 1;
					开始停止车辆升级(status);
				}
			}
			break;
			case IDC_CHECK_disasse_kart:
			{
				if (wNF == BN_CLICKED)
				{
					auto status = Button_GetCheck(hWnd_check_disasse_kart) == 1;
					开始停止分解车辆(status);
				}
			}
			break;

			case IDC_BUTTON1_jumppage:
			{
				BOOL lpTranslated = FALSE;
				auto id = GetDlgItemInt(hDlg, IDC_EDIT_lockpage, &lpTranslated, TRUE);
				PostMessage(hGameWnd, WM_PrivateList + 20, id, 1772);
			}
			break;
			case IDC_BUTTON1_addcar:
			{

				BOOL lpTranslated = FALSE;
				auto id = GetDlgItemInt(hDlg, IDC_EDIT1_addcar_code, &lpTranslated, TRUE);
				if (lpTranslated)
				{

					if (listview_addordel(id, 1))
					{
						WCHAR wcstr[MAX_PATH];
						memset(wcstr, 0, sizeof(wcstr));
						LVITEM vitem;
						memset(&vitem, 0, sizeof(LVITEM));
						vitem.mask = LVIF_TEXT;
						vitem.iSubItem = 0;
						vitem.iItem = ListView_GetItemCount(hWnd_list_filter);
						auto index = ListView_InsertItem(hWnd_list_filter, &vitem);
						swprintf_s(wcstr, L"%d", id);
						ListView_SetItemText(hWnd_list_filter, index, 1, wcstr);
						memset(wcstr, 0, sizeof(wcstr));
						GetDlgItemText(hDlg, IDC_EDIT1_addcar_name, wcstr, MAX_PATH * sizeof(WCHAR));
						ListView_SetItemText(hWnd_list_filter, index, 0, wcstr);
					}

					//过滤车子列表.push_back(id);
				}
			}
			case IDC_CHECK_Engine_V1:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_V1 = Button_GetCheck(hWnd_check_filter_Engine_V1) == 1;
				break;
			case IDC_CHECK_Engine_X:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_X = Button_GetCheck(hWnd_check_filter_Engine_X) == 1;
				break;
			case IDC_CHECK_Engine_9:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_9 = Button_GetCheck(hWnd_check_filter_Engine_9) == 1;
				break;
			case IDC_CHECK_Engine_HTNew:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_HTNew = Button_GetCheck(hWnd_check_filter_Engine_HTNew) == 1;
				break;
			case IDC_CHECK_Engine_HT:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_HT = Button_GetCheck(hWnd_check_filter_Engine_HT) == 1;
				break;
			case IDC_CHECK_Engine_Z7:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_Z7 = Button_GetCheck(hWnd_check_filter_Engine_Z7) == 1;
				break;
			case IDC_CHECK_Engine_SR:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_SR = Button_GetCheck(hWnd_check_filter_Engine_SR) == 1;
				break;
			case IDC_CHECK_Engine_PRO:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_PRO = Button_GetCheck(hWnd_check_filter_Engine_PRO) == 1;
				break;
			case IDC_CHECK_attribute_red:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_ultimate = Button_GetCheck(hWnd_check_filter_attribute_red) == 1;
				break;
			case IDC_CHECK_attribute_gold:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_rare = Button_GetCheck(hWnd_check_filter_attribute_gold) == 1;
				break;
			case IDC_CHECK_attribute_purple:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_advanced = Button_GetCheck(hWnd_check_filter_attribute_purple) == 1;
				break;
			case IDC_CHECK_attribute_white:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_general = Button_GetCheck(hWnd_check_filter_attribute_white) == 1;
				break;
			case IDC_CHECK_filter_upcar:
				if (wNF == BN_CLICKED)
					g_Filtercar.Filter_upcar = Button_GetCheck(hWnd_check_filter_upcar) == 1;
				break;
			case IDC_CHECK_filter:
				if (wNF == BN_CLICKED)
				{
					g_Filtercar.Filter_listhide = Button_GetCheck(hWnd_check_filter) == 1;
					g_Filtercar.Filter_listshow = FALSE;
					Button_SetCheck(hWnd_check_filter_show, 0);
				}
				break;
			case IDC_CHECK_filter_show:
				if (wNF == BN_CLICKED)
				{
					g_Filtercar.Filter_listhide = FALSE;
					Button_SetCheck(hWnd_check_filter, 0);
					g_Filtercar.Filter_listshow = Button_GetCheck(hWnd_check_filter_show) == 1;
				}
				break;

				break;

			default:
				break;
			}
		}
		break;
		case WM_CLOSE:
			开始停止合成(FALSE);
			开始停止车辆升级(FALSE);
			开始停止分解车辆(FALSE);
			ShowWindow(hDlg, SW_HIDE);
			break;
		case WM_DESTROY:
			EndDialog(hWnd, 0);
			break;
		default:
			break;
		}
		return FALSE;
	}

	void initialization(HMODULE hModule)
	{
		// Init2();
		while (g_GamehWnd == 0)
			Sleep(1000);
		Sleep(1000);

#ifdef MemPlug
		DialogBoxIndirectParam(0, pDlgtmp_Dialog_Exchange, 0, WndProc_wx, NULL);
#else
		DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG_Exchange), 0, WndProc_wx);
#endif // !MemPlug
	}

	void __stdcall LoadDialog(HMODULE hModule)
	{
		if (hDlg)
		{
			ShowWindow(hDlg, SW_SHOW);
		}
		else
		{

			CloseHandle(MyCreateThread(reinterpret_cast<LPTHREAD_START_ROUTINE>(initialization), hModule));
		}
	}

}