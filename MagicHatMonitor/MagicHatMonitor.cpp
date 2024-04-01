#include "../struct.h"
#include "../helper/DbgPrint.h"
#include "../KartRider.hpp"
#include "../helper/detoursHlper.hpp"
#include "../helper/aobscan/aobscan.hpp"
#include "../helper/cJSON/cJSON.h"
#include "../helper/PacketHeaderHook/PacketHeaderHook.hpp"
#include <thread>
#include <atomic>
#include <chrono>

//#define ___DEBUGMODE

#define DbgPrint2
//#define DbgPrint2 MyDbgPrintFun

extern PGameStockItem pGameStockItem;
extern HWND g_GamehWnd;
extern HMODULE g_hModule;
extern LPCDLGTEMPLATEW pDlgtmp_Dialog_MagicHat;
extern DWORD g_BaseAddress, g_NumberAddress;
extern UINT32 角色参数;

HWND hDlg = nullptr;
HWND m_hWndGroup_magicHatBox = nullptr;
HWND m_hWndCheck_diyT = nullptr;
HWND m_hWndCheck_begin = nullptr;
HWND m_hWndCheck_goe = nullptr;
HWND m_hWndCheck_notgs = nullptr;
HWND m_hWndCheck_lastlow = nullptr;
HWND m_hWndStatic_select = nullptr;
HWND m_hWndStatic_sleep = nullptr;
HWND m_hWndStatic_t2 = nullptr;

HWND m_hWndCombo_type = nullptr;
HWND m_hWndCombo_Items = nullptr;
HWND m_hWndEdit_sleep = nullptr;
HWND m_hWndEdit_typeNum = nullptr;
HWND m_hWndEdit_notGnum = nullptr;
HWND m_hWndEdit_lowNum = nullptr;
HWND m_hWndEdit_log = nullptr;

std::vector<HWND> hwnd_magicHatBox;
std::vector<HWND> hwnd_magicHatCkBox;

DWORD MonitorItemCode = -1;

ULONG magicHatOpenReturn = 0;
ULONG magicHatOpenStatus = 0;
ULONG magicHatOverTrialCount = 0;




PacketHeaderHook PrEnterMagicHatPacket;
PacketHeaderHook PrUpdateMagicHatRewardInfoPacket;
PacketHeaderHook PrCompleteInitMagicHatPacket;
#pragma pack(push, 1)
typedef struct _StockItem
{
	LONG stockId;
	LONG dwCurCount;
	LONG dwTotalCount;
    char type;
} StockItem;

typedef struct _MagicHatItem
{
	LONG status;
	LONG length;
    StockItem si;
} MagicHatItem;
typedef struct _BoxSellOut
{
	LONG Count;
    BOOL IsOut_Box1;
    BOOL IsOut_Box2;
    BOOL IsOut_Box3;
    BOOL IsOut_Box4;
    BOOL IsOut_Box5;
    BOOL IsOut_Box6;
    BOOL IsOut_Box7;
    BOOL IsOut_Box8;
    BOOL IsOut_Box9;
    BOOL IsOut_Box10;
    BOOL IsOut_Box11;
    BOOL IsOut_Box12;
} BoxSellOut;

typedef struct _MonitorConfig
{
	LONG 监视代码;
    BOOL 抽到后停止;
	LONG 非金框售罄条件;
	LONG 后排小于数量;
    LONG 位置条件;
    BoxSellOut bso;
} MonitorConfig;

#pragma pack(pop)

namespace magicHatMonitor
{

    int Edit_SetLimitText(HWND hWnd, int nMax)
    {
        return SendMessage(hWnd, EM_SETLIMITTEXT, nMax, 0);
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

    void WINAPI showMagicHatOpenReward(wchar_t *itemName, DWORD itemCode)
    {
		auto MsgBuffer = std::make_unique<WCHAR[]>(MAX_PATH);
		auto hasItemCode = MonitorItemCode == itemCode;
		wsprintf(MsgBuffer.get(), hasItemCode ? L"恭喜你成功获得：[%ws]\n" : L"获得：[%ws]\n", itemName);
		Edit_AddString(m_hWndEdit_log, MsgBuffer.get());
    }
    void __declspec(naked) myhook_magicHatOpenReturn()
    {
        __asm pushad;
        __asm mov ecx, dword ptr[ebp + 0x8];
        __asm push dword ptr[ecx + 0x14];
        __asm push dword ptr[eax];
        __asm call showMagicHatOpenReward;
        __asm popad;
        __asm jmp dword ptr[magicHatOpenReturn];
    }

    INT_PTR OnCtlColor(
        WPARAM wParam,
        LPARAM lParam)
    {
        auto hDc = reinterpret_cast<HDC>(wParam);
        auto CtlhWnd = reinterpret_cast<HWND>(lParam);
        auto setCtrlColor = [](HDC hDc, COLORREF textColor = NULL, COLORREF bkColor = NULL) -> INT_PTR
        {
            auto Brush = GetStockObject(HOLLOW_BRUSH);
            SetTextColor(hDc, textColor == NULL ? RGB(0, 0, 0) : textColor);
            SetBkColor(hDc, bkColor == NULL ? GetSysColor(COLOR_BTNFACE) : bkColor);
            return reinterpret_cast<INT_PTR>(Brush);
        };

        if (CtlhWnd == m_hWndCheck_begin ||
            CtlhWnd == m_hWndStatic_sleep)
        {
            return setCtrlColor(hDc, RGB(0, 0, 255));
        }
        else if (CtlhWnd == m_hWndStatic_select)
        {
            return setCtrlColor(hDc, RGB(255, 20, 147));
        }
        else if (CtlhWnd == m_hWndStatic_t2)
        {
            return setCtrlColor(hDc, RGB(255, 0, 0));
        }
        else if (CtlhWnd == m_hWndGroup_magicHatBox)
        {
            return setCtrlColor(hDc, RGB(255, 69, 0));
        }
        return FALSE;
    }

    POINT pt_open, pt_confirm;

    MagicHatItem *m_magicHatItem = nullptr;
    std::atomic_flag m_LockEnterMagicHat = ATOMIC_FLAG_INIT;
    std::thread m_thread_Monitor;
    volatile long lock_EnterMagicHat, lock_thread;
    MonitorConfig monitorConfig;


    BOOL isOpenMagicHat()
    {
        // 读数据.上锁
        while (m_LockEnterMagicHat.test_and_set())
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
		auto isMonitor = FALSE;
		auto outCount = 0l;
		auto goldType_Count = 0l;
        auto ngoldTypeCount = 0l;
		auto lastCount = 0l;
        auto boxSellOut = reinterpret_cast<BoxSellOut *>(&monitorConfig.bso);
		//printf("位置条件:%d\n", monitorConfig.位置条件);

        for (auto i = 0; i < m_magicHatItem->length; i++)
        {
            auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&m_magicHatItem->si) + (i * sizeof(StockItem)));
            auto boxIsSet = *reinterpret_cast<BOOL *>(reinterpret_cast<DWORD>(&boxSellOut->IsOut_Box1) + (i * 4));
            if (stockItem->stockId == monitorConfig.监视代码 && stockItem->dwCurCount > 0)
            {
                isMonitor = TRUE;
            }
			// 是否设置了框子条件
			if (boxIsSet)
			{
				//printf("位置条件>0?:%d,dwCurCount<位置条件?:%d\n", monitorConfig.位置条件 > 0, stockItem->dwCurCount < monitorConfig.位置条件);

				if (monitorConfig.位置条件 > 0 && stockItem->dwCurCount < monitorConfig.位置条件)
				{

					outCount++;

				}
				else if (monitorConfig.位置条件 == -1 && stockItem->dwCurCount == 0)
				{
					outCount++;
				}

			}



			// 现在数量==0

            if (stockItem->dwCurCount == 0)
            {
                stockItem->type == 1 ? goldType_Count++ : ngoldTypeCount++;
            }
            if (i >= 8)
            {
                if (monitorConfig.后排小于数量 > 0 && stockItem->dwCurCount < monitorConfig.后排小于数量)
                {
                    lastCount++;
                }
            }
        }
        // 解锁
        m_LockEnterMagicHat.clear();
#ifdef ___DEBUGMODE
		isMonitor = TRUE;
#endif // !___DEBUGMODE


        if (isMonitor)
        {
            if (boxSellOut->Count == outCount)
            {
                return TRUE;
            }
            // 因为后两排只有4个框位 所以等于4即可
            if (lastCount == 4)
            {
                return TRUE;
            }
            if (monitorConfig.非金框售罄条件 != -1 && ngoldTypeCount >= monitorConfig.非金框售罄条件)
            {
                return TRUE;
            }
        }
        return FALSE;
    }

    void WINAPI fn_thread(int Delayed)
    {
        while (InterlockedExchange(&lock_thread, lock_thread) != 2)
        {
            if (InterlockedExchange(&lock_EnterMagicHat, lock_EnterMagicHat) == 0 &&
                isOpenMagicHat())
            {
				//printf("条件满足开始抽取\n");

#ifndef ___DEBUGMODE
				mouse_msg(g_GamehWnd, pt_open);
				while (*reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(g_BaseAddress) + magicHatOpenStatus) != 0)
				{
					if (InterlockedExchange(&lock_thread, lock_thread) == 2)
					{
						break;
					}
					std::this_thread::sleep_for(std::chrono::microseconds(10));
				}

				if (*reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(g_BaseAddress) + magicHatOpenStatus) == 0 &&
					*reinterpret_cast<DWORD *>(*reinterpret_cast<DWORD *>(g_BaseAddress) + (magicHatOpenStatus + 4)) == 12)
				{
					mouse_msg(g_GamehWnd, pt_confirm);
				}
				PostMessage(g_GamehWnd, WM_KEYDOWN, VK_ESCAPE, 0);
				Sleep(5);
				PostMessage(g_GamehWnd, WM_KEYUP, VK_ESCAPE, 0);
#endif // !___DEBUGMODE


            }

			Sleep(Delayed);
        }
    }

    void UpdateMagicHatPanel(MagicHatItem *magicHatItem)
    {
		auto InfoString = std::make_unique<WCHAR[]>(MAX_PATH);
        for (auto i = 0l; i < magicHatItem->length; i++)
        {
            auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&magicHatItem->si) + (i * sizeof(StockItem)));
            swprintf(InfoString.get(), MAX_PATH , L"\r\n%d\r\n%d/%d", stockItem->stockId, stockItem->dwCurCount, stockItem->dwTotalCount);
            SetWindowText(hwnd_magicHatBox[i], InfoString.get());
        }
    }

    void WINAPI showPrEnterMagicHatPacket(INT32 flag, void *buffer, size_t length)
    {
        InterlockedExchange(&lock_EnterMagicHat, 1);
        if (flag == 1474889750)
        {
            while (m_LockEnterMagicHat.test_and_set())
                std::this_thread::sleep_for(std::chrono::seconds(10));
            memcpy(m_magicHatItem, buffer, length);
            m_LockEnterMagicHat.clear();
            auto magicHatItem = reinterpret_cast<MagicHatItem *>(buffer);
            UpdateMagicHatPanel(magicHatItem);

            // for (size_t i = 0; i < magicHatItem->length; i++)
            // {
            //     auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&magicHatItem->si) + (i * sizeof(StockItem)));
            //     std::unique_ptr<WCHAR[]> InfoString(new WCHAR[MAX_PATH]);
            //     swprintf(InfoString.get(), MAX_PATH * 2, L"\r\n%d\r\n%d/%d", stockItem->stockId, stockItem->dwTotalNum, stockItem->dwNum);
            //     SetDlgItemText(hDlg, IDC_STATIC1 + i, InfoString.get());
            // }
        }
        InterlockedExchange(&lock_EnterMagicHat, 0);
    }

    void __declspec(naked) ph_PrEnterMagicHatPacket()
    {
        __asm pushad;
        __asm mov ecx, dword ptr[esp + 0x2C];
        __asm mov eax, dword ptr[esp + 0x24];
        __asm mov eax, dword ptr[eax + 0x8];
        __asm mov edx, dword ptr[eax + 0xC];
        __asm sub edx, dword ptr[eax + 0x8];
        __asm push edx;
        __asm mov eax, dword ptr[eax + 0x4];
        __asm push eax;
        __asm push ecx;
        __asm call showPrEnterMagicHatPacket;
        __asm popad;
        __asm jmp dword ptr[PrEnterMagicHatPacket.orginData];
    }
    void WINAPI showPrUpdateMagicHatRewardInfoPacket(INT32 flag, void *buffer, size_t length)
    {
        if (flag == -886895508)
        {
            struct UpdateMagicHat
            {
                LONG dwCount;
                StockItem si;
            };
            auto magicHatData = reinterpret_cast<UpdateMagicHat *>(buffer);
            for (auto i = 0l; i < magicHatData->dwCount; i++)
            {
                auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&magicHatData->si) + (i * sizeof(StockItem)));
                for (auto j = 0l; j < m_magicHatItem->length; j++)
                {
                    auto stockItem2 = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&m_magicHatItem->si) + (j * sizeof(StockItem)));
                    if (stockItem->stockId == stockItem2->stockId)
                    {
                        stockItem2->dwCurCount = stockItem->dwCurCount;
                        std::unique_ptr<WCHAR[]> InfoString(new WCHAR[MAX_PATH]);
                        swprintf(InfoString.get(), MAX_PATH * 2, L"\r\n%d\r\n%d/%d", stockItem2->stockId, stockItem2->dwCurCount, stockItem2->dwTotalCount);
                        SetWindowText(hwnd_magicHatBox[j], InfoString.get());
                    }
                }
            }
        }
    }
    void __declspec(naked) ph_PrUpdateMagicHatRewardInfoPacket()
    {
        __asm pushad;
        __asm mov ecx, dword ptr[esp + 0x2C];
        __asm mov eax, dword ptr[esp + 0x24];
        __asm mov eax, dword ptr[eax + 0x8];
        __asm mov edx, dword ptr[eax + 0xC];
        __asm sub edx, dword ptr[eax + 0x8];
        __asm push edx;
        __asm mov eax, dword ptr[eax + 0x4];
        __asm push eax;
        __asm push ecx;
        __asm call showPrUpdateMagicHatRewardInfoPacket;
        __asm popad;
        __asm jmp dword ptr[PrUpdateMagicHatRewardInfoPacket.orginData];
    }
    void WINAPI showPrCompleteInitMagicHatPacket(INT32 flag, void *buffer, size_t length)
    {
        InterlockedExchange(&lock_EnterMagicHat, 1);
        if (flag == -1652356379)
        {
            while (m_LockEnterMagicHat.test_and_set())
                std::this_thread::sleep_for(std::chrono::seconds(10));
            // memcpy(m_magicHatItem, reinterpret_cast<void *>(reinterpret_cast<DWORD>(buffer) + 4), length);
            memcpy(&m_magicHatItem->length, reinterpret_cast<void *>(buffer), length);

            m_LockEnterMagicHat.clear();
            UpdateMagicHatPanel(m_magicHatItem);

            // for (size_t i = 0; i < magicHatItem->length; i++)
            // {
            //     auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&magicHatItem->si) + (i * sizeof(StockItem)));
            //     std::unique_ptr<WCHAR[]> InfoString(new WCHAR[MAX_PATH]);
            //     swprintf(InfoString.get(), MAX_PATH * 2, L"\r\n%d\r\n%d/%d", stockItem->stockId, stockItem->dwTotalNum, stockItem->dwNum);
            //     SetDlgItemText(hDlg, IDC_STATIC1 + i, InfoString.get());
            // }
        }
        InterlockedExchange(&lock_EnterMagicHat, 0);
    }
    void __declspec(naked) ph_PrCompleteInitMagicHatPacket()
    {

        __asm pushad;
        __asm mov ecx, dword ptr[esp + 0x2C];
        __asm mov eax, dword ptr[esp + 0x24];
        __asm mov eax, dword ptr[eax + 0x8];
        __asm mov edx, dword ptr[eax + 0xC];
        __asm sub edx, dword ptr[eax + 0x8];
        __asm push edx;
        __asm mov eax, dword ptr[eax + 0x4];
        __asm push eax;
        __asm push ecx;
        __asm call showPrCompleteInitMagicHatPacket;
        __asm popad;
        __asm jmp dword ptr[PrCompleteInitMagicHatPacket.orginData];
    }
    void InstallHook()
    {
		VMProtectBeginMutation("MagicHatMonitor::InstallHook");
        m_magicHatItem = reinterpret_cast<MagicHatItem *>(new char[0x1000]);
        memset(m_magicHatItem, 0, 0x1000);
        PrEnterMagicHatPacket.setHook("PrEnterMagicHatPacket", ph_PrEnterMagicHatPacket, 0x10);
        PrUpdateMagicHatRewardInfoPacket.setHook("PrUpdateMagicHatRewardInfoPacket", ph_PrUpdateMagicHatRewardInfoPacket, 0x10);
        PrCompleteInitMagicHatPacket.setHook("PrCompleteInitMagicHatPacket", ph_PrCompleteInitMagicHatPacket, 0x10);
		VMProtectEnd();
    }
    void InitData(HWND hwnd)
    {
		VMProtectBeginMutation("MagicHatMonitor::InitData");
        hDlg = hwnd;
        m_hWndGroup_magicHatBox = GetDlgItem(hDlg, IDC_GroupBox);
        m_hWndCheck_diyT = GetDlgItem(hDlg, IDC_CHECK_diyT);
        m_hWndCheck_begin = GetDlgItem(hDlg, IDC_CHECKBOX_begin);
        m_hWndCheck_goe = GetDlgItem(hDlg, IDC_CHECK_goe);
        m_hWndCheck_notgs = GetDlgItem(hDlg, IDC_CHECK_notGs);
        m_hWndCheck_lastlow = GetDlgItem(hDlg, IDC_CHECK_lastLow);

        m_hWndStatic_select = GetDlgItem(hDlg, IDC_STATIC_t1);
        m_hWndStatic_sleep = GetDlgItem(hDlg, IDC_STATIC_sleep);
        m_hWndStatic_t2 = GetDlgItem(hDlg, IDC_STATIC_t2);

        m_hWndCombo_type = GetDlgItem(hDlg, IDC_COMBO_type);
        m_hWndCombo_Items = GetDlgItem(hDlg, IDC_COMBO_ItemList);
        m_hWndEdit_sleep = GetDlgItem(hDlg, IDC_EDIT_sleep);
        m_hWndEdit_typeNum = GetDlgItem(hDlg, IDC_EDIT_num);
        m_hWndEdit_notGnum = GetDlgItem(hDlg, IDC_EDIT_notGnum);
        m_hWndEdit_lowNum = GetDlgItem(hDlg, IDC_EDIT_lowNum);
        m_hWndEdit_log = GetDlgItem(hDlg, IDC_EDIT2);
        Edit_SetLimitText(m_hWndEdit_log, -1);
		SetDlgItemInt(hDlg, IDC_EDIT_sleep, 10, true);
		SetDlgItemInt(hDlg, IDC_EDIT_num, 2, true);
		SetDlgItemInt(hDlg, IDC_EDIT_notGnum, 6, true);
		SetDlgItemInt(hDlg, IDC_EDIT_lowNum, 100, true);



        Button_SetCheck(m_hWndCheck_diyT, TRUE);
        ComboBox_InsertString(m_hWndCombo_type, 0, L"售罄");
        ComboBox_InsertString(m_hWndCombo_type, 1, L"小于");
        ComboBox_SetCurSel(m_hWndCombo_type, 0);

        for (int i = IDC_STATIC1; i <= IDC_STATIC13; i++)
            hwnd_magicHatBox.push_back(GetDlgItem(hDlg, i));
        for (int i = IDC_CHECKBOX_box1; i <= IDC_CHECKBOX_box12; i++)
        {
            auto dlg_hWnd = GetDlgItem(hDlg, i);
            if ((i - IDC_CHECKBOX_box1) >= 4 && (i - IDC_CHECKBOX_box1) < 8)
                Button_SetCheck(dlg_hWnd, TRUE);
            hwnd_magicHatCkBox.push_back(dlg_hWnd);
        }
        char *magicHatItem = nullptr;
#ifdef ___DEBUGMODE
		auto GetResData = [](Data1 &pData1, LPCWSTR lpName, LPCWSTR lpType) -> void
		{
			HRSRC hRsrc = FindResourceW(g_hModule, lpName, lpType);
			if (hRsrc != nullptr)
			{
				auto SrcSize = SizeofResource(g_hModule, hRsrc);
				
				auto lpRsrc = LoadResource(g_hModule, hRsrc);
				if (lpRsrc != nullptr)
				{
					auto data = malloc(SrcSize);
					memcpy(data, lpRsrc, SrcSize);
					pData1.pData = data;
					pData1.dwSize = SrcSize;
					FreeResource(lpRsrc);
				}
			}
		};
		pGameStockItem = reinterpret_cast<PGameStockItem>(malloc(sizeof(GameStockItem)));
		GetResData(pGameStockItem->magicHat_cn, MAKEINTRESOURCE(IDR_CN1), L"cn");
		GetResData(pGameStockItem->magicHat_kr, MAKEINTRESOURCE(IDR_KR1), L"kr");
		GetResData(pGameStockItem->magicHat_tw, MAKEINTRESOURCE(IDR_TW1), L"tw");
		magicHatItem = reinterpret_cast<char *>(pGameStockItem->magicHat_cn.pData);


#else
		for (HWND hWndChild = FindWindowEx(0, 0, nullptr, nullptr);
			hWndChild != 0;
			hWndChild = FindWindowEx(0, hWndChild, nullptr, nullptr))
		{
			DWORD processId;
			GetWindowThreadProcessId(hWndChild, &processId);
			if (processId == GetCurrentProcessId())
			{
				auto className = std::make_unique<WCHAR[]>(MAX_PATH);
				GetClassName(hWndChild, className.get(), MAX_PATH);
				if (lstrcmpiW(className.get(), L"PopKart Client") == 0)
				{
					magicHatItem = reinterpret_cast<char *>(pGameStockItem->magicHat_cn.pData);
				}
				else if (lstrcmpiW(className.get(), L"KartRider Client") == 0)
				{
					magicHatItem = reinterpret_cast<char *>(pGameStockItem->magicHat_kr.pData);
				}
				else if (lstrcmpiW(className.get(), L" Client") == 0)
				{
					magicHatItem = reinterpret_cast<char *>(pGameStockItem->magicHat_tw.pData);
				}
			}
		}

#endif // !___DEBUGMODE







        auto obj = cJSON_Parse(magicHatItem);
        auto item = cJSON_GetObjectItem(obj, u8"data");
        for (int i = 0; i < cJSON_GetArraySize(item); i++)
        {
            auto obj2 = cJSON_GetArrayItem(item, i);
            auto stockName = cJSON_GetObjectItem(obj2, "stockName");
            auto stockId = cJSON_GetObjectItem(obj2, "stockId");
            ComboBox_InsertString(m_hWndCombo_Items, i, _A2W_CP(stockName->valuestring, CP_UTF8));
            ComboBox_SetItemData(m_hWndCombo_Items, i, stockId->valueint);
        }
        ComboBox_SetCurSel(m_hWndCombo_Items, 0);

#ifndef ___DEBUGMODE

		RECT rc = { 0, 0, 0, 0 };
		GetClientRect(g_GamehWnd, &rc);
		pt_open = { (rc.right / 2) + 64, (rc.bottom / 2) + 175 };
		pt_confirm = { rc.right / 2, (rc.bottom / 2) + 95 };
		std::vector<DWORD> foundAddress;
		magicHatOpenReturn = aobscan(GetCurrentProcess(), 1, 0x401000, 0x2000000, "8B 45 F0 83 78 10 00", foundAddress) + 52;
		DbgPrint2(L"magicHatOpenReturn:[%08X]\n", magicHatOpenReturn);
		//MessageBox(0, nullptr, nullptr, 0);
		UCHAR pattern[] = "\x83\x78\x10\x05\x0F\x85";
		PVOID pFound = NULL;
		NTSTATUS status = BBSearchPattern(pattern, 0xCC, sizeof(pattern) - 1, reinterpret_cast<void*>(magicHatOpenReturn), 0x1000, &pFound);
		if (NT_SUCCESS(status))
		{
			magicHatOverTrialCount = reinterpret_cast<ULONG>(pFound);
			auto _temp = GetJumpAddress(magicHatOverTrialCount + 4);
			WirteJump(magicHatOverTrialCount, _temp - 2);
		}
		detours::redirect(true, &(PVOID &)magicHatOpenReturn, myhook_magicHatOpenReturn);
		magicHatOpenStatus = aobscan(GetCurrentProcess(), 1, 0xA01000, 0x2000000, "83 78 10 01 0F 85 84 00 00 00", foundAddress) - 41;
		magicHatOpenStatus = GetCallAddress(magicHatOpenStatus);
		magicHatOpenStatus = *reinterpret_cast<ULONG *>(magicHatOpenStatus + 0x12);
		DbgPrint2(L"magicHatOpenStatus:[%08X]\n", magicHatOpenStatus);


		SetWindowTextW(hwnd, reinterpret_cast<WCHAR *>(*reinterpret_cast<DWORD *>(角色参数 + 0x7C)));
		InstallHook();
#else

		m_magicHatItem = reinterpret_cast<MagicHatItem *>(new char[0x1000]);
		memset(m_magicHatItem, 0, 0x1000);

		UCHAR buffer[] = { 0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x79,0x61,0x00,0x00,0x03,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x3D,0x5F,0x00,0x00,0x04,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x01,0x71,0x60,0x00,0x00,0x0B,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x3A,0x5F,0x00,0x00,0x05,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x2B,0x5C,0x00,0x00,0x24,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0xB3,0x43,0x00,0x00,0x1E,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x9B,0x66,0x00,0x00,0x1C,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x8F,0x66,0x00,0x00,0x24,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x00,0x1D,0x06,0x00,0x00,0x38,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x5E,0x4F,0x00,0x00,0x22,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x68,0x3E,0x00,0x00,0x3B,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x53,0x5D,0x00,0x00,0x2E,0x00,0x00,0x00,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

		memcpy(m_magicHatItem, buffer, 300);
		UpdateMagicHatPanel(m_magicHatItem);
		




#endif // ___DEBUGMODE



		VMProtectEnd();
    }

    INT_PTR CALLBACK WndProc_wx(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
		VMProtectBeginMutation("MagicHatMonitor::WndProc_wx");
        switch (message)
        {
        case WM_CTLCOLORSTATIC:
            return OnCtlColor(wParam, lParam);
            break;
        case WM_INITDIALOG:
        {
            InitData(hWnd);
        }
        break;
        case WM_COMMAND:
        {
            WORD wID = LOWORD(wParam);
            WORD wNF = HIWORD(wParam);

            switch (wNF)
            {
            case BN_CLICKED:
            {
                if (wID == IDC_CHECKBOX_begin)
                {
                    auto 设置变量数据 = [](BOOL status) -> void
                    {
                        EnableWindow(m_hWndCombo_Items, !status);
                        EnableWindow(m_hWndCombo_type, !status);
                        EnableWindow(m_hWndCheck_diyT, !status);
                        EnableWindow(m_hWndCheck_goe, !status);
                        EnableWindow(m_hWndCheck_notgs, !status);
                        EnableWindow(m_hWndCheck_lastlow, !status);
                        EnableWindow(m_hWndEdit_lowNum, !status);
                        EnableWindow(m_hWndEdit_notGnum, !status);
                        EnableWindow(m_hWndEdit_sleep, !status);
                        EnableWindow(m_hWndEdit_typeNum, !status);
                        EnableWindow(m_hWndEdit_lowNum, !status);
                        for (size_t i = 0; i < hwnd_magicHatCkBox.size(); i++)
                            EnableWindow(hwnd_magicHatCkBox[i], !status);

                        if (status)
                        {
                            auto boxSellOut = reinterpret_cast<BoxSellOut *>(&monitorConfig.bso);
                            if (Button_GetCheck(m_hWndCheck_diyT))
                            {
                                for (size_t i = 0; i < hwnd_magicHatCkBox.size(); i++)
                                {
                                    if (Button_GetCheck(hwnd_magicHatCkBox[i]))
                                    {
                                        boxSellOut->Count++;
                                        *reinterpret_cast<BOOL *>(reinterpret_cast<DWORD>(&boxSellOut->IsOut_Box1) + (i * 4)) = TRUE;
                                    }
                                }
                            }
                            else
                            {
                                boxSellOut->Count = -1;
                            }
                            monitorConfig.监视代码 = ComboBox_GetItemData(m_hWndCombo_Items, ComboBox_GetCurSel(m_hWndCombo_Items));
                            BOOL lpTranslated;
                            DWORD dwValue = GetDlgItemInt(hDlg, IDC_EDIT_notGnum, &lpTranslated, TRUE);
                            monitorConfig.非金框售罄条件 = Button_GetCheck(m_hWndCheck_notgs) == 1 ? dwValue : -1;
                            dwValue = GetDlgItemInt(hDlg, IDC_EDIT_lowNum, &lpTranslated, TRUE);
                            monitorConfig.后排小于数量 = Button_GetCheck(m_hWndCheck_lastlow) == 1 ? dwValue : -1;
                            monitorConfig.抽到后停止 = Button_GetCheck(m_hWndCheck_lastlow);

                            if (Button_GetCheck(m_hWndCheck_diyT))
                            {
                                dwValue = GetDlgItemInt(hDlg, IDC_EDIT_num, &lpTranslated, TRUE);
                                monitorConfig.位置条件 = ComboBox_GetCurSel(m_hWndCombo_type) == 1 ? dwValue : -1;
                            }
                            else
                            {
                                monitorConfig.位置条件 = -1;
                            }
                        }
                        else
                        {
                            memset(&monitorConfig, 0, sizeof(MonitorConfig));
                        }

                        // Button_GetCheck(m_hWndCombo_Items)
                    };

                    if (Button_GetCheck(m_hWndCheck_begin))
                    {

                        设置变量数据(TRUE);
                        InterlockedExchange(&lock_thread, 1);
                        BOOL lpTranslated;
                        int delayed = GetDlgItemInt(hDlg, IDC_EDIT_sleep, &lpTranslated, TRUE);
						m_thread_Monitor = std::thread(fn_thread, delayed);
                    }
                    else
                    {
                        设置变量数据(FALSE);
                        InterlockedExchange(&lock_thread, 2);
                        m_thread_Monitor.join();
                    }
				}
				//else if (wID == IDC_BUTTON1) 
				//{
				//	auto boxSellOut = reinterpret_cast<BoxSellOut *>(&monitorConfig.bso);
				//	for (auto i = 8l; i < m_magicHatItem->length; i++)
				//	{
				//		auto stockItem = reinterpret_cast<StockItem *>(reinterpret_cast<DWORD>(&m_magicHatItem->si) + (i * sizeof(StockItem)));
				//		stockItem->dwCurCount = 0;
				//		//auto boxIsSet = *reinterpret_cast<BOOL *>(reinterpret_cast<DWORD>(&boxSellOut->IsOut_Box1) + (i * 4));
				//	}
				//	UpdateMagicHatPanel(m_magicHatItem);
				//}
				//else if (wID == IDC_BUTTON2) 
				//{
				//}
            }
            case CBN_SELCHANGE:
            {
                if (wID == IDC_COMBO_type)
                {
                    auto cur = ComboBox_GetCurSel(m_hWndCombo_type);
                    if (cur == 0)
                    {
                        ShowWindow(m_hWndEdit_typeNum, SW_HIDE);
                    }
                    else
                    {
                        ShowWindow(m_hWndEdit_typeNum, SW_SHOW);
                    }
                }
            }
            break;

            default:
                break;
            }
        }
        break;
        case WM_CLOSE:
#ifdef ___DEBUGMODE
			ExitProcess(0);
#else
			ShowWindow(hDlg, SW_HIDE);
#endif // ___DEBUGMODE

            break;
        case WM_DESTROY:
            EndDialog(hWnd, 0);
            break;
        default:
            break;
        }
		VMProtectEnd();
        return FALSE;
    }

    void initialization(HMODULE hModule)
    {
		
#ifdef ___DEBUGMODE
		g_hModule = hModule;
		//MessageBox(nullptr, L"msg", nullptr, MB_OK);
		DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG_magicHat), 0, WndProc_wx);
#else
		while (g_GamehWnd == 0)
			Sleep(1000);
		Sleep(1000);

#ifdef MemPlug
		DialogBoxIndirectParam(0, pDlgtmp_Dialog_MagicHat, 0, WndProc_wx, NULL);
#else
		DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG_magicHat), 0, WndProc_wx);
#endif // !MemPlug

#endif // ___DEBUGMODE



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