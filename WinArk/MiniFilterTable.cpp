#include "stdafx.h"
#include "MiniFilterTable.h"
#include "FormatHelper.h"
#include "ClipboardHelper.h"
#include <fltUser.h>
#include "MiniFilterDlg.h"
#include "DriverHelper.h"
#include "SymbolHelper.h"

#pragma comment(lib,"FltLib")

LRESULT CMiniFilterTable::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return 0;
}

LRESULT CMiniFilterTable::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lparam, BOOL& /*bHandled*/) {
	return 0;
}
LRESULT CMiniFilterTable::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	PaintTable(m_hWnd);
	return 0;
}

LRESULT CMiniFilterTable::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CMiniFilterTable::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CMiniFilterTable::OnUserVabs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CMiniFilterTable::OnUserVrel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CMiniFilterTable::OnUserChgs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CMiniFilterTable::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	return Tablefunction(m_hWnd, WM_VSCROLL, zDelta >= 0 ? 0 : 1, wParam);
}
LRESULT CMiniFilterTable::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnLBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnLBtnUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnRBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	CMenu menu;
	CMenuHandle hSubMenu;
	menu.LoadMenu(IDR_KERNEL_HOOK_CONTEXT);
	hSubMenu = menu.GetSubMenu(0);
	POINT pt;
	::GetCursorPos(&pt);
	bool show = Tablefunction(m_hWnd, uMsg, wParam, lParam);
	if (show) {
		auto id = (UINT)TrackPopupMenu(hSubMenu, TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, nullptr);
		if (id) {
			PostMessage(WM_COMMAND, id);
		}
	}

	return 0;
}
LRESULT CMiniFilterTable::OnUserSts(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}
LRESULT CMiniFilterTable::OnSysKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

CMiniFilterTable::CMiniFilterTable(BarInfo& bars, TableInfo& table) :CTable(bars, table) {
	SetTableWindowInfo(bars.nbar);
	Refresh();
}

int CMiniFilterTable::ParseTableEntry(CString& s, char& mask, int& select, MiniFilterInfo& info, int column) {
	
	switch (static_cast<TableColumn>(column))
	{
		case TableColumn::Altitude:
			s = info.Altitude.c_str();
			break;

		case TableColumn::FilterName:
			s = info.FilterName.c_str();
			break;

		case TableColumn::FrameID:
			s.Format(L"%lu", info.FrameID);
			break;

		case TableColumn::NumberOfInstance:
			s.Format(L"%lu", info.NumberOfInstance);
			break;
	}
	return s.GetLength();
}

bool CMiniFilterTable::CompareItems(const MiniFilterInfo& s1, const MiniFilterInfo& s2, int col, bool asc) {
	switch (col)
	{
		case 0:
			
			break;
		default:
			break;
	}
	return false;
}

void CMiniFilterTable::BuildFilterInfo(PVOID buffer, bool isNewStyle) {
	WCHAR filterName[128] = { 0 };
	WCHAR altitude[64] = { 0 };
	MiniFilterInfo filterInfo;
	if (isNewStyle) {
		PFILTER_AGGREGATE_STANDARD_INFORMATION info;
		info = (PFILTER_AGGREGATE_STANDARD_INFORMATION)buffer;
		if (info->Flags == FLTFL_ASI_IS_MINIFILTER) {
			if (info->Type.MiniFilter.FilterNameLength < 128) {
				CopyMemory(filterName,
					(PUCHAR)info + info->Type.MiniFilter.FilterNameBufferOffset,
					info->Type.MiniFilter.FilterNameLength);
				filterName[info->Type.MiniFilter.FilterNameLength] = UNICODE_NULL;
			}
			if (info->Type.MiniFilter.FilterAltitudeLength < 64) {
				CopyMemory(altitude, (PCHAR)info + info->Type.MiniFilter.FilterAltitudeBufferOffset,
					info->Type.MiniFilter.FilterAltitudeLength);
				filterName[info->Type.MiniFilter.FilterAltitudeLength] = UNICODE_NULL;
			}
			filterInfo.FrameID = info->Type.MiniFilter.FrameID;
			filterInfo.NumberOfInstance = info->Type.MiniFilter.NumberOfInstances;
			filterInfo.Altitude = altitude;
			filterInfo.FilterName = filterName;


			m_Table.data.info.push_back(filterInfo);
		}
	}
}

void CMiniFilterTable::Refresh() {
	BYTE buffer[1 << 10];
	DWORD bytes;
	HANDLE hFind;
	bool isNewStyle = true;
	m_Table.data.info.clear();

	HRESULT hr;
	hr = FilterFindFirst(FilterAggregateStandardInformation, buffer, sizeof(buffer), &bytes, &hFind);
	if (!SUCCEEDED(hr)) {
		isNewStyle = false;
		hr = FilterFindFirst(FilterFullInformation, buffer, sizeof(buffer), &bytes, &hFind);
	}

	if (SUCCEEDED(hr)) {
		BuildFilterInfo(buffer, isNewStyle);
		do
		{
			hr = FilterFindNext(hFind, isNewStyle ? FilterAggregateStandardInformation : FilterFullInformation,
				buffer, sizeof(buffer), &bytes);
			if (SUCCEEDED(hr)) {
				BuildFilterInfo(buffer, isNewStyle);
			}
		} while (SUCCEEDED(hr));

		FilterFindClose(hFind);
	}

	if (!SUCCEEDED(hr) && hr != HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)) {
		WCHAR errorText[256];
		if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, hr, 0, errorText, sizeof(errorText),
			0)) {
			AtlMessageBox(m_hWnd, errorText, IDS_TITLE, MB_ICONERROR);
		}
	}

	m_Table.data.n = m_Table.data.info.size();
}

std::wstring CMiniFilterTable::GetSingleMiniFilterInfo(MiniFilterInfo& info) {
	CString text;
	CString s;

	

	text += L"\r\n";

	return text.GetString();
}

//LRESULT CMiniFilterTable::OnPiDDBCacheCopy(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
//	int selected = m_Table.data.selected;
//	ATLASSERT(selected >= 0);
//	auto& info = m_Table.data.info[selected];
//
//	CString text;
//	CString s;
//
//	
//
//	ClipboardHelper::CopyText(m_hWnd, text);
//	return 0;
//}
//
//LRESULT CMiniFilterTable::OnPiDDBCacheExport(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
//	CSimpleFileDialog dlg(FALSE, nullptr, L"*.txt",
//		OFN_EXPLORER | OFN_ENABLESIZING | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
//		L"文本文档 (*.txt)\0*.txt\0所有文件\0*.*\0", m_hWnd);
//	if (dlg.DoModal() == IDOK) {
//		auto hFile = ::CreateFile(dlg.m_szFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
//		if (hFile == INVALID_HANDLE_VALUE)
//			return FALSE;
//		for (int i = 0; i < m_Table.data.n; ++i) {
//			auto& info = m_Table.data.info[i];
//			std::wstring text = GetSingleMiniFilterInfo(info);
//			Helpers::WriteString(hFile, text);
//		}
//		::CloseHandle(hFile);
//	}
//	return TRUE;
//}

LRESULT CMiniFilterTable::OnRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	Refresh();

	return TRUE;
}

LRESULT CMiniFilterTable::OnCallback(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& info = m_Table.data.info[selected];

	CMiniFilterDlg dlg(info.FilterName);
	dlg.DoModal(m_hWnd);

	return TRUE;
}

LRESULT CMiniFilterTable::OnRemove(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& info = m_Table.data.info[selected];

	auto len = info.FilterName.length();
	DWORD size = len * sizeof(WCHAR) + sizeof(MiniFilterData);

	auto pData = std::make_unique<BYTE[]>(size);
	if (!pData)
		return FALSE;

	ULONG offset = SymbolHelper::GetFltmgrStructMemberOffset("_FLT_OBJECT", "RundownRef");

	auto data = reinterpret_cast<MiniFilterData*>(pData.get());
	data->OperationsOffset = 0;
	data->RundownRefOffset = offset;
	data->Length = len;
	::wcscpy_s(data->Name, len + 1, info.FilterName.c_str());

	bool ok = DriverHelper::RemoveMiniFilter(data, size);
	if (!ok) {
		AtlMessageBox(m_hWnd, L"移除失败!", L"Error", MB_ICONERROR);
	}
	if (ok) {
		Refresh();
		Invalidate();
	}
	return TRUE;
}
