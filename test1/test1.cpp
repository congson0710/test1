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
HWND folderTree; HWND listView;
WCHAR **drives; HTREEITEM *hItemDrive;
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
		folderTree = CreateWindowEx(0,
			WC_TREEVIEW,
			L"TreeView",
			WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT,
			0,
			0,
			250,
			1000,
			hWnd,
			(HMENU)ID_TREEVIEW,
			hInst,
			NULL);
		// create listview
		listView = CreateWindowEx(0, WC_LISTVIEW, L"", WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS, 250, 0, 1250, HIWORD(lParam), hWnd, NULL, hInst, NULL);
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

		lvc.cx = 400;
		lvc.pszText = L"Size";
		ListView_InsertColumn(listView, 3, &lvc);
		// create node root
		TV_INSERTSTRUCT tvInsert;

		tvInsert.hParent = NULL;
		tvInsert.hInsertAfter = TVI_ROOT;
		tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;
		tvInsert.item.pszText = L"This PC";
		tvInsert.item.lParam = (LPARAM)L"This PC";

		HTREEITEM hItemRoot = TreeView_InsertItem(folderTree, &tvInsert);
		// take drives
		GetLogicalDriveStrings(gMyDrives, (LPTSTR)gIpBuffer);
		eliminateNullCharacter(gIpBuffer, path);
		// add drives to node root
		int len = wcslen(path) / 3;
		WCHAR **drives = new WCHAR*[len];
		for (int i = 0; i < len; i++) {
			drives[i] = new WCHAR[100];
			wcsncpy(drives[i], path + i * 3, 3);
			drives[i][3] = L'\0';
		}
		//storage

		hItemDrive = new HTREEITEM[len]; //drive parents
		TV_INSERTSTRUCT *tvInsertDriveNode = new TV_INSERTSTRUCT[len]; //struct drive node

		for (int i = 0; i < len; i++) {
			//
			WCHAR *folderPath = new WCHAR[100];
			wcscpy(folderPath, drives[i]);
			tvInsertDriveNode[i] = createDrivesNode(hItemRoot, drives[i]);
			tvInsertDriveNode[i].item.cChildren = true;
			tvInsertDriveNode[i].item.lParam = (LPARAM)folderPath;

			hItemDrive[i] = TreeView_InsertItem(folderTree, &tvInsertDriveNode[i]);
		}

		LPSHELLFOLDER psfDesktop = NULL;
		SHGetDesktopFolder(&psfDesktop); //lay desktop

		LPENUMIDLIST penumIDL = NULL; //lay enumerator
		psfDesktop->EnumObjects(NULL, SHCONTF_FOLDERS, &penumIDL);

		LPITEMIDLIST pidl = NULL; //pidl dùng để duyệt
		HRESULT hr = NULL;

		IMalloc* pMalloc = nullptr;
		WCHAR info[1024];
		info[0] = '\0';
		do {
			hr = penumIDL->Next(1, &pidl, NULL);
			if (hr == S_OK) {
				WCHAR buffer[1024];
				STRRET strret;
				psfDesktop->GetDisplayNameOf(pidl, SHGDN_NORMAL, &strret);
				StrRetToBuf(&strret, pidl, buffer, 1024);
				StrCat(info, buffer);
				StrCat(info, L"\n");

				pMalloc->Free(pidl);
			}
		} while (hr == S_OK);
		MessageBox(0, info, 0, 0);
		pMalloc->Release();
		psfDesktop->Release();
		// add child node to drives
		for (int i = 0; i < len; i++) {
			WCHAR *folderPath = new WCHAR[100];
			WIN32_FIND_DATA fd;
			wcscpy(folderPath, drives[i]);
			wcscat(folderPath, L"*");
			HANDLE hFile = FindFirstFileW(folderPath, &fd);
			bool bFound = true;
			while (bFound) {
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
					&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
					&& (wcscmp(fd.cFileName, L".") != 0) && (wcscmp(fd.cFileName, L"..") != 0)) {
					WCHAR *folderPathChild = new WCHAR[100];
					wcscpy(folderPathChild, drives[i]);
					if (wcslen(fd.cFileName) != 3)
						wcscat(folderPathChild, L"\\");
					wcscat(folderPathChild, fd.cFileName);

					TV_INSERTSTRUCT tvInsertChildNode;
					tvInsertChildNode = createChildNode(hItemDrive[i], fd.cFileName);
					tvInsertChildNode.item.lParam = (LPARAM)folderPathChild;
					HTREEITEM child = TreeView_InsertItem(folderTree, &tvInsertChildNode);

				}
				bFound = FindNextFileW(hFile, &fd);

			}


		}
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
