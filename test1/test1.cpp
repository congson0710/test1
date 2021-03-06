// test1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "test1.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEST1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEST1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TEST1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TEST1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
// global value
HWND folderTree; HWND listView; LPITEMIDLIST gPidl = NULL;
WCHAR **drives; HTREEITEM *hItemDrive; HTREEITEM hItemRoot; 
// set up system font
LOGFONT lf; HFONT hFont;
// get local drives
int gMyDrives = 100;
WCHAR gIpBuffer[50];
WCHAR path[20];

// eliminate terminated character
void eliminateNullCharacter(WCHAR* ipBuffer, WCHAR* path) {
	int j = 0;
	for (int i = 0; i < 20; i++) {
		if (ipBuffer[i] == '\0' && ipBuffer[i + 1] == '\0')
			break;
		else if (ipBuffer[i] == '\0')
			continue;
		else {
			path[j] = ipBuffer[i];
			j++;
		}
	}
}
// insert struct
void insertNode(HTREEITEM hParent, WCHAR *buffer) {
	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT;
	tvInsert.item.pszText = buffer;
	HTREEITEM hParentNext = TreeView_InsertItem(folderTree, &tvInsert);
}
// create child node (child folder)
TV_INSERTSTRUCT createChildNode(HTREEITEM parent, WCHAR *fName) {
	TV_INSERTSTRUCT tvInsert;

	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsert.item.pszText = fName;

	return tvInsert;
}
// create drive node 
TV_INSERTSTRUCT createDrivesNode(HTREEITEM parent, WCHAR *drive) {
	TV_INSERTSTRUCT tvInsert;

	tvInsert.hParent = parent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
	tvInsert.item.pszText = drive;

	return tvInsert;
}
// init treeview
HTREEITEM InitTreeView(HWND hWnd, HINSTANCE hInst) {
	RECT rcClient;                       // The parent window's client area.
	GetClientRect(hWnd, &rcClient);
	folderTree = CreateWindow(WC_TREEVIEW,
		L"",
		WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
		0, 40,
		(rcClient.right - rcClient.left) / 4,
		rcClient.bottom - rcClient.top,
		hWnd,
		NULL,
		hInst,
		NULL);

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.mask = TVIF_TEXT;
	tvInsert.item.pszText = L"This PC";
	return hItemRoot = TreeView_InsertItem(folderTree, &tvInsert);
}

void InitListView(HWND hWnd, HINSTANCE hInst) {
	RECT rcClient;                       // The parent window's client area.
	GetClientRect(hWnd, &rcClient);
	//Create ListView Control
	listView = CreateWindow(WC_LISTVIEW,
		L"",
		WS_CHILD | WS_BORDER | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS,
		(rcClient.right - rcClient.left) / 4, 40,
		((rcClient.right - rcClient.left) * 3) / 4,
		rcClient.bottom - rcClient.top,
		hWnd,
		NULL,
		hInst,
		NULL);
	//Insert Column
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvc.fmt = LVCFMT_LEFT;

	lvc.cx = 300;
	lvc.pszText = L"Name";
	ListView_InsertColumn(listView, 0, &lvc);

	lvc.cx = 100;
	lvc.pszText = L"Type";
	ListView_InsertColumn(listView, 1, &lvc);

	lvc.cx = 200;
	lvc.pszText = L"Date Modified";
	ListView_InsertColumn(listView, 2, &lvc);

	lvc.cx = 200;
	lvc.pszText = L"Size";
	ListView_InsertColumn(listView, 3, &lvc);
}

void loadTreeViewShell(HTREEITEM hParentChild, LPSHELLFOLDER &psf, bool stop) {
	//
	TV_INSERTSTRUCT tvInsert;
	LPITEMIDLIST pidl = NULL;
	LPENUMIDLIST penumlist = NULL;
	HRESULT hr;
	//
	if (psf == NULL) { return; }
	psf->EnumObjects(NULL, SHCONTF_FOLDERS, &penumlist);
	if (penumlist == NULL) { return; }

	do {
		hr = penumlist->Next(1, &pidl, NULL);
		if (S_OK == hr)
		{
			STRRET strret;
			WCHAR buffer[128];
			psf->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strret);
			StrRetToBuf(&strret, pidl, buffer, 128);

			if (wcscmp(buffer, L"") == 0) {
				continue;
			}

			LPSHELLFOLDER psfChild = NULL;
			psf->BindToObject(pidl, NULL, IID_IShellFolder, (void **)&psfChild);
			tvInsert.hParent = hParentChild;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
			tvInsert.item.lParam = (LPARAM)psfChild;
			tvInsert.item.pszText = buffer;
			HTREEITEM hParentNext = TreeView_InsertItem(folderTree, &tvInsert);

			//continue loading child node
			if (stop) {
				loadTreeViewShell(hParentNext, psfChild, false);
			}
		}
	} while (S_OK == hr);
}

void loadListViewShell(LPSHELLFOLDER &psf, bool stop)
{
	LPITEMIDLIST pidl = NULL;
	LPENUMIDLIST penumlist = NULL;
	HRESULT hr;
	//
	int index = 0;
	LVITEM lvi;
	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	lvi.iItem = index;
	lvi.iSubItem = 0;
	//
	if (psf == NULL) { return; }
	psf->EnumObjects(NULL, SHCONTF_FOLDERS, &penumlist);
	if (penumlist == NULL) { return; }

	do {
		hr = penumlist->Next(1, &pidl, NULL);
		if (S_OK == hr)
		{
			STRRET strret;
			WCHAR buffer[128];
			psf->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strret);
			StrRetToBuf(&strret, pidl, buffer, 128);

			if (wcscmp(buffer, L"") == 0) {
				continue;
			}

			LPSHELLFOLDER psfChild = NULL;
			psf->BindToObject(pidl, NULL, IID_IShellFolder, (void **)&psfChild);
			lvi.lParam = (LPARAM)psfChild;
			lvi.pszText = buffer;
			//
			ListView_InsertItem(listView, &lvi);
			ListView_SetItemText(listView, index, 1, L"Folder");
			//
			lvi.iItem = ++index;
			////continue loading child node
			//if (stop) {
			//	loadListViewShell(psfChild, false);
			//}
		}
	} while (S_OK == hr);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
	{
		// get system font
		GetObject(GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
		hFont = CreateFont(lf.lfHeight, lf.lfWidth,
			lf.lfEscapement, lf.lfOrientation, lf.lfWeight,
			lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
			lf.lfOutPrecision, lf.lfClipPrecision, lf.lfQuality,
			lf.lfPitchAndFamily, lf.lfFaceName);
		// common control DLL is loaded
		InitCommonControls();
		// create treeview
		hItemRoot = InitTreeView(hWnd,  hInst);
		// create listview
		InitListView(hWnd, hInst);
		//load child
		LPITEMIDLIST pidl;
		LPSHELLFOLDER psf = NULL;

		SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &pidl);
		SHBindToObject(NULL, pidl, NULL, IID_IShellFolder, (void**)&psf);
		
		loadTreeViewShell(hItemRoot, psf, true);
		break;
	}
	
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case TVN_ITEMEXPANDING:
		{
			TVITEM a; WCHAR buffer[100], buffer1[100];

			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			a.hItem = pnmtv->itemNew.hItem;
			a.pszText = buffer;
			a.cchTextMax = 100;
			a.mask = TVIF_TEXT | TVIF_PARAM;

			TreeView_GetItem(folderTree, &a);

			if (wcscmp(a.pszText, L"This PC") == 0)
			{
				break;
			}
			//delete child node
			HTREEITEM  hChild = TreeView_GetChild(folderTree, a.hItem);
			while (hChild != NULL)
			{
				TVITEM b;
				b.hItem = TreeView_GetChild(folderTree, a.hItem);
				b.pszText = buffer1;
				b.cchTextMax = 100;
				b.mask = TVIF_TEXT | TVIF_PARAM;

				TreeView_GetItem(folderTree, &b);
				
				TreeView_DeleteItem(folderTree, hChild);
				hChild = TreeView_GetChild(folderTree, a.hItem);
			}
			//
			LPITEMIDLIST pidl = NULL;
			LPSHELLFOLDER psfChild = (LPSHELLFOLDER)a.lParam;
			loadTreeViewShell(a.hItem, psfChild, true);
			break;
		}
		case TVN_SELCHANGED:
		{
			ListView_DeleteAllItems(listView);
			WCHAR buffer[100];
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW)lParam;
			TVITEM a;
			a.hItem = pnmtv->itemNew.hItem;
			a.cchTextMax = 100;
			a.pszText = buffer;
			a.mask = TVIF_TEXT | TVIF_PARAM;
			a.lParam = pnmtv->itemNew.lParam;
			//
			TreeView_GetItem(folderTree, &a);
			//
			LPSHELLFOLDER psfChild = (LPSHELLFOLDER)a.lParam;
			loadListViewShell(psfChild, true);
			break;
		}
		case NM_DBLCLK:
		{
			WCHAR buffer[100];
			LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)lParam;
			LVITEM a;
			a.iItem = lpnmitem->iItem;
			a.iSubItem = lpnmitem->iSubItem;
			a.mask = LVIF_TEXT | LVIF_PARAM;
			a.pszText = buffer;
			a.cchTextMax = 100;
			a.lParam = lpnmitem->lParam;
			if (ListView_GetItem(listView, &a)) {
				ListView_DeleteAllItems(listView);
				LPITEMIDLIST pidl = NULL;
				LPSHELLFOLDER psfChild = (LPSHELLFOLDER)a.lParam;
				loadListViewShell(psfChild, true);
			}
			break;
		}
		default:
			break;
		}
		break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
