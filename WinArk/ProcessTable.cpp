#include "stdafx.h"
#include "ProcessTable.h"
#include "FormatHelper.h"
#include "resource.h"
#include "DriverHelper.h"
#include "ProcessModuleDlg.h"
#include "ProcessPropertiesDlg.h"
#include "ProcessThreadDlg.h"
#include "ProcessHandleDlg.h"
#include "ProcessMemoryDlg.h"

#pragma comment(lib,"WinSysCore")

CProcessTable::CProcessTable(BarInfo& bars,TableInfo& table)
	:CTable(bars,table){
	SetTableWindowInfo(bars.nbar);
}

int CProcessTable::ParseTableEntry(CString& s, char& mask, int& select, std::shared_ptr<WinSys::ProcessInfo>& info, int column) {
	// Name,Id,Session,Priority,Threads,Handles,Attributes,CreateTime,CompanyName,Description,ExePath,CmdLines
	auto& px = GetProcessInfoEx(info.get());
	switch (static_cast<ProcessColumn>(column)) {
		case ProcessColumn::Name:
			s = info->GetImageName().c_str();
			break;
		case ProcessColumn::Id:
			s.Format(L"%6u (0x%05X)", info->Id,info->Id);
			break;
		case ProcessColumn::Session:
			s.Format(L"%2d ", info->SessionId);
			break;
		case ProcessColumn::UserName:
			s = px.UserName().c_str();
			break;
		case ProcessColumn::Priority:
			s = FormatHelper::PriorityClassToString(px.GetPriorityClass());
			break;
		case ProcessColumn::Threads:
			s.Format(L"%u", info->ThreadCount);
			break;
		case ProcessColumn::Handles:
			s.Format(L"%u  ", info->HandleCount);
			break;
		case ProcessColumn::Attributes:
			s = FormatHelper::ProcessAttributesToString(px.GetAttributes(m_ProcMgr));
			break;
		case ProcessColumn::CreateTime:
			s = FormatHelper::TimeToString(info->CreateTime);
			break;
		case ProcessColumn::Description:
			s = px.GetDescription().c_str();
			break;
		case ProcessColumn::CompanyName:
			s = px.GetCompanyName().c_str();
			break;
		case ProcessColumn::Version:
			s = px.GetVersion().c_str();
			break;
		case ProcessColumn::ExePath:
			s = px.GetExecutablePath().c_str();
			break;
		case ProcessColumn::CmdLine:
			s = px.GetCommandLine().c_str();
			break;
		default:
			break;
	}
	return s.GetLength();
}

LRESULT CProcessTable::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled) {
	m_Table.data.info.clear();
	bHandled = false;
	return 0;
}

LRESULT CProcessTable::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	PaintTable(m_hWnd);
	return 0;
}

LRESULT CProcessTable::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	bHandled = false;
	return 0;
}

LRESULT CProcessTable::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	Tablefunction(m_hWnd, uMsg, wParam, lParam);
	return 0;
}

LRESULT CProcessTable::OnUserVabs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnUserVrel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnUserChgs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	return Tablefunction(m_hWnd, WM_VSCROLL, zDelta >= 0 ? 0 : 1, wParam);
}

LRESULT CProcessTable::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnLBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnLBtnUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

bool CProcessTable::CompareItems(const std::shared_ptr<WinSys::ProcessInfo>& p1, const std::shared_ptr<WinSys::ProcessInfo>& p2, int col,bool asc) {
	switch (static_cast<ProcessColumn>(col)) {
		case ProcessColumn::Name: return SortHelper::SortStrings(p1->GetImageName(), p2->GetImageName(), asc);
		case ProcessColumn::Id: return SortHelper::SortNumbers(p1->Id, p2->Id, asc);
	}
	return false;
}

LRESULT CProcessTable::OnUserSts(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnSysKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	return Tablefunction(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CProcessTable::OnGetDlgCode(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	return DLGC_WANTARROWS;	// Direction keys.����Ҫ���������
}

void CProcessTable::Refresh() {
	bool first = m_Table.data.info.empty();
	auto count = (int)m_ProcMgr.EnumProcesses();

	if (first) {
		m_Table.data.info = m_ProcMgr.GetProcesses();
		m_Table.data.n = count;
		return;
	}

	auto tick = ::GetTickCount64();
	count = (int)m_Table.data.info.size();
	for (int i = 0; i < count; i++) {
		auto& p = m_Table.data.info[i];
		auto& px = GetProcessInfoEx(p.get());
		if (px.IsTerminated && tick > px.TargetTime) {
			m_ProcessesEx.erase(p.get());
			m_Table.data.info.erase(m_Table.data.info.begin() + i);
			i--;
			count--;
			continue;
		}
		if (px.IsNew && tick > px.TargetTime) {
			px.IsNew = false;
		}
	}

	for (auto& p : m_ProcMgr.GetNewProcesses()) {
		m_Table.data.info.push_back(p);
		count++;
		auto& px = GetProcessInfoEx(p.get());
		px.IsNew = true;
		px.TargetTime = tick + 2000;
	}

	for (auto& p : m_ProcMgr.GetTerminatedProcesses()) {
		auto& px = GetProcessInfoEx(p.get());
		px.IsTerminated = true;
		px.TargetTime = tick + 2000;
	}

	m_Table.data.n = count;
	return;
}

ProcessInfoEx& CProcessTable::GetProcessInfoEx(WinSys::ProcessInfo* pi) const {
	auto it = m_ProcessesEx.find(pi);
	if (it != m_ProcessesEx.end())
		return it->second;

	ProcessInfoEx px(pi);
	auto pair = m_ProcessesEx.insert({ pi,std::move(px) });
	return pair.first->second;
}

LRESULT CProcessTable::OnRBtnDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
	if (m_UpdateInterval)
		KillTimer(1);

	CMenu menu;
	CMenuHandle hSubMenu;
	menu.LoadMenu(IDR_CONTEXT);
	hSubMenu = menu.GetSubMenu(0);
	POINT pt;
	::GetCursorPos(&pt);
	bool show  = Tablefunction(m_hWnd, uMsg, wParam, lParam);
	if (show) {
		auto id = (UINT)TrackPopupMenu(hSubMenu, TPM_RETURNCMD, pt.x, pt.y, 0, m_hWnd, nullptr);
		if (id) {
			PostMessage(WM_COMMAND, id);
		}
	}
	
	if (m_UpdateInterval)
		SetTimer(1, m_UpdateInterval, nullptr);
	return 0;
}

int CProcessTable::GetRowImage(HWND, int row) const {
	return GetProcessInfoEx(m_Table.data.info[row].get()).GetImageIndex(m_Images);
}

LRESULT CProcessTable::OnTimer(UINT, WPARAM id, LPARAM, BOOL&) {
	if (id == 1)
		Refresh();
	return 0;
}

LRESULT CProcessTable::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetTimer(1, m_UpdateInterval, nullptr);
	return 0;
}

LRESULT CProcessTable::OnProcessKill(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& p = m_Table.data.info[selected];

	CString text;
	text.Format(L"ɱ�����̣�%u (%ws)?", p->Id, p->GetImageName().c_str());
	if (AtlMessageBox(*this, (PCWSTR)text, IDS_TITLE, MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2) == IDCANCEL)
		return 0;

	auto hProcess = DriverHelper::OpenProcess(p->Id, PROCESS_TERMINATE);
	BOOL ok = false;
	if (hProcess) {
		ok = ::TerminateProcess(hProcess, 0);
	}
	if (!ok)
		AtlMessageBox(*this, L"Failed to kill process", IDS_TITLE, MB_ICONERROR);
	
	return 0;
}

LRESULT CProcessTable::OnProcessRefresh(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	Refresh();
	return 0;
}

LRESULT CProcessTable::OnProcessModules(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& p = m_Table.data.info[selected];

	CModuleDlg dlg;
	dlg.DoModal(m_hWnd,(LPARAM)p->Id);

	return 0;
}

LRESULT CProcessTable::OnProcessProperties(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& process = m_Table.data.info[selected];
	auto& px = GetProcessInfoEx(process.get());
	auto dlg = new CProcessPropertiesDlg(m_ProcMgr, px);
	dlg->Create(::GetAncestor(m_hWnd, GA_ROOT));
	dlg->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CProcessTable::OnProcessGoToFileLocation(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& process = m_Table.data.info[selected];
	auto& px = GetProcessInfoEx(process.get());

	if ((INT_PTR)::ShellExecute(nullptr, L"open", L"explorer",
		(L"/select,\"" + px.GetExecutablePath() + L"\"").c_str(),
		nullptr, SW_SHOWDEFAULT) < 32)
		AtlMessageBox(*this, L"Failed to locate executable", IDS_TITLE, MB_ICONERROR);

	return 0;
}

LRESULT CProcessTable::OnProcessThreads(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& p = m_Table.data.info[selected];

	CThreadDlg dlg;
	dlg.DoModal(m_hWnd, (LPARAM)p->Id);

	return 0;
}

LRESULT CProcessTable::OnProcessHandles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& p = m_Table.data.info[selected];

	CHandleDlg dlg;
	dlg.DoModal(m_hWnd, (LPARAM)p->Id);

	return 0;
}

LRESULT CProcessTable::OnProcessMemory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
	int selected = m_Table.data.selected;
	ATLASSERT(selected >= 0);
	auto& p = m_Table.data.info[selected];

	CMemoryDlg dlg;
	dlg.DoModal(m_hWnd, (LPARAM)p->Id);

	return 0;
}