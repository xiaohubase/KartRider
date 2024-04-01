#include "../struct.h"
#include "../KartRider.hpp"
#include "../resource.h"
#include "../helper/aobscan/aobscan.hpp"
#include "../helper/detoursHlper.hpp"
#include "../helper/DbgPrint.h"
#include <thread>
#include <chrono>
#include <mutex>
#define DbgPrint2 
//#define DbgPrint2 MyDbgPrintFun


extern HWND g_GamehWnd;
extern DWORD g_BaseAddress;
extern LPCDLGTEMPLATEW pDlgtmp_Dialog_BlockCatch;

namespace BlockCatch {

	HWND hDlg = nullptr;

	HWND hWnd_edit_score = nullptr;
	HWND hWnd_check_begin = nullptr;
	HWND hWnd_button_finalTime = nullptr;
	HWND hWnd_button_dollover = nullptr;
	HANDLE hThread_doll = nullptr;
	POINT BlockCatch_BeginPos = { 0,0 };
	POINT BlockCatch_RewardPos = { 0,0 };
	DWORD BlockCatch_MoveFunc = 0;
	DWORD BlockCatch_Time = 0;
	DWORD BlockCatch_Time_offset = 0;
	DWORD Global_BaseAddress = 0;
	DWORD Packet_BlockCatchStage = 0;
	DWORD BlockCatch_score = 0;
	BOOL  dollstatus = FALSE;
	HANDLE hEvent_ThreadStatus = nullptr;
	std::thread g_ThreadDoll;

	void __declspec(naked) MyFunc_BlockCatchMove()
	{
		VMProtectBegin("MyFunc_BlockCatchMove");
		__asm {
			mov eax, dword ptr[dollstatus];
			test eax, eax;
			je label_end;
			mov eax, dword ptr[BlockCatch_Time_offset];
			mov edx, dword ptr[BlockCatch_score];
			cmp dword ptr[ecx + eax + 0x4], edx;	//判断当前分数 <= 自定义分数 跳过
			jae label_end;
			mov eax, dword ptr[esp + 0x4];
			cmp dword ptr[eax + 0x1BC], 0x4;		//判断是否为空壳子
			je label_skip_nullBox;
			mov dword ptr[eax + 0x1C4], 0x2;		//抓到盒子
			mov eax, dword ptr[BlockCatch_Time_offset];
			mov edx, eax;
			add edx, 0x3C;
			mov dword ptr[ecx + edx], 0x3C;			//进入狂热模式加速
			sub edx, 0x104;
			cmp dword ptr[ecx + edx], 0x4;			//是否为升起
			jne label_up;
			mov dword ptr[ecx + edx], 0x1;			//升起了置1
			jmp label_end;
		label_up:;
			mov dword ptr[ecx + edx], 0x4;			//未升起,开始升起获取分数
			jmp label_end;
		label_skip_nullBox:;
			retn 0x8;
		label_end:;
			jmp dword ptr[BlockCatch_MoveFunc];
		}
		VMProtectEnd();
	}

	void WINAPI MyFunc_ThreadDoll() {

		auto i = 0;
		while (true)
		{
			while (WaitForSingleObject(hEvent_ThreadStatus, 10) == WAIT_TIMEOUT)
				Sleep(10);

			if (*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(g_BaseAddress) + BlockCatch_Time_offset) == 0)
			{
				mouse_msg(g_GamehWnd, BlockCatch_BeginPos);
				Sleep(50);
				mouse_msg(g_GamehWnd, BlockCatch_RewardPos);
			}
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	INT_PTR CALLBACK WndProc_BlockCatch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{

		switch (message)
		{
		case WM_INITDIALOG:
		{
			hDlg = hWnd;
			hWnd_edit_score = GetDlgItem(hWnd, IDC_EDIT1_score);
			hWnd_check_begin = GetDlgItem(hWnd, IDC_CHECK_dollbegin);
			hWnd_button_finalTime = GetDlgItem(hWnd, IDC_BUTTON_finalTime);
			hWnd_button_dollover = GetDlgItem(hWnd, IDC_BUTTON_overdollgame);
			SetDlgItemInt(hWnd, IDC_EDIT1_score, 6000, false);
			hEvent_ThreadStatus = CreateEvent(nullptr, TRUE, FALSE, nullptr);
			auto initBlockCatch = [](VOID)->void {

				auto GetCallAddress = [](DWORD dwAddress)->DWORD {
					DWORD result = 0;
					try
					{
						result = dwAddress + *(PDWORD)(dwAddress + 1) + 5;
					}
					catch (...)
					{

					}
					return result;
				};

				//初始化开始与结束坐标
				RECT Client_rc = { 0,0,0,0 };
				GetClientRect(g_GamehWnd, &Client_rc);
				BlockCatch_BeginPos = { (Client_rc.right / 2) - 160,591 };
				BlockCatch_RewardPos = { Client_rc.right / 2,514 };
				//初始化地址偏移数据
				DWORD hModule = reinterpret_cast<DWORD>(GetModuleHandle(nullptr));
				PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
				IMAGE_NT_HEADERS* nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(hModule + dos_header->e_lfanew);
				DWORD stopaddr = hModule + nt_header->OptionalHeader.SizeOfImage;
				DWORD dwBaseOfData, dwSizeOfData;
				if ((dwBaseOfData = nt_header->OptionalHeader.BaseOfData) == 0 ||
					(dwSizeOfData = nt_header->OptionalHeader.SizeOfInitializedData) == 0) {
					MessageBox(g_GamehWnd, L"获取数据地址失败", L"Msg", MB_ICONERROR);
					return;
				}
				DWORD pSearchBegin = hModule + dwBaseOfData;
				std::vector<DWORD> foundAddress;
				Global_BaseAddress = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "8B 82 ** ** 00 00 50 6A 01 B9", foundAddress);
				Global_BaseAddress = GetCallAddress(Global_BaseAddress + 0xE) + 0x8;
				Global_BaseAddress = *reinterpret_cast<DWORD*>(Global_BaseAddress) + 0x4;
				DbgPrint2(L"Global_BaseAddress:[%08X]\n", Global_BaseAddress);
				Packet_BlockCatchStage = FindPacketHeader("BlockCatchStage", pSearchBegin, pSearchBegin + dwSizeOfData) + 4;
				DbgPrint2(L"Packet_BlockCatchStage:[%08X]\n", Packet_BlockCatchStage);
				BlockCatch_MoveFunc = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "89 4D FC 8B 45 FC 8B 4D 0C 2B 88 ** 01 00 00", foundAddress) - 0x6;
				DbgPrint2(L"BlockCatch_MoveFunc:[%08X]\n", BlockCatch_MoveFunc);
				BlockCatch_Time = aobscan(GetCurrentProcess(), 1, hModule, stopaddr, "8B 88 ** 02 00 00 83 E9 01 8B", foundAddress);
				DbgPrint2(L"BlockCatch_Time:[%08X]\n", BlockCatch_Time);
				BlockCatch_Time_offset = *reinterpret_cast<DWORD*>(BlockCatch_Time + 0x2);
				DbgPrint2(L"BlockCatch_Time_offset:[%08X]\n", BlockCatch_Time_offset);
				memcpy(reinterpret_cast<void*>(BlockCatch_Time + 0x6), "\x90\x90\x90", 3);
				detours::redirect(true, &(PVOID&)BlockCatch_MoveFunc, MyFunc_BlockCatchMove);
			};
			initBlockCatch();
		}
		break;

		case WM_COMMAND:
		{
			WORD wID = LOWORD(wParam);
			WORD wNF = HIWORD(wParam);
			switch (wID)
			{
			case IDC_CHECK_dollbegin:
			{
				if (wNF == BN_CLICKED) {
					dollstatus = Button_GetCheck(hWnd_check_begin) == 1;
					auto BeginFunc = [](BOOL status)->void {
						if (status) {
							BOOL lpTranslated = FALSE;
							BlockCatch_score = GetDlgItemInt(hDlg, IDC_EDIT1_score, &lpTranslated, TRUE);
							if (g_ThreadDoll.native_handle() == nullptr) {	
								g_ThreadDoll = std::thread(MyFunc_ThreadDoll);
							}
							memcpy(reinterpret_cast<void*>(BlockCatch_Time + 0x6), "\x83\xE9\x01", 3);
							SetEvent(hEvent_ThreadStatus);
						}
						else {
							ResetEvent(hEvent_ThreadStatus);
							BlockCatch_score = 0;
							memcpy(reinterpret_cast<void*>(BlockCatch_Time + 0x6), "\x90\x90\x90", 3);
						}

					};
					BeginFunc(dollstatus);
				}
			}
			break;
			case IDC_BUTTON_finalTime:
				*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(g_BaseAddress) + BlockCatch_Time_offset) = 0x9;
				break;
			case IDC_BUTTON_overdollgame:
				*reinterpret_cast<DWORD*>(*reinterpret_cast<DWORD*>(g_BaseAddress) + BlockCatch_Time_offset) = 0x0;
				break;
			default:
				break;
			}
		}
		break;
		case WM_CLOSE:
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

	void __stdcall initialization(HMODULE hModule)
	{
		while (g_GamehWnd == 0)
			Sleep(1000);

		Sleep(1000);


#ifdef MemPlug
		DialogBoxIndirectParam(0, pDlgtmp_Dialog_BlockCatch, 0, WndProc_BlockCatch, NULL);
#else
		DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG_BlockCatch), 0, WndProc_BlockCatch);
#endif // !MemPlug

	}

	void __stdcall LoadDialog(HMODULE hModule)
	{
		if (hDlg) {
			ShowWindow(hDlg, SW_SHOW);
		}
		else
		{
			CloseHandle(MyCreateThread(reinterpret_cast<LPTHREAD_START_ROUTINE>(initialization), hModule));
		}
	}

}