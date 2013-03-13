#include <atlstr.h>
#include <Windows.h>

#include "VirtualKeyboard.h"




void VirtualKeyboard::pressKey(char key) {
	/*
	INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.time = 0;
    ip.ki.dwFlags = KEYEVENTF_UNICODE; // Specify the key as a unicode character
    ip.ki.wScan = key; // Which keypress to simulate
    ip.ki.wVk = 0;
    ip.ki.dwExtraInfo = 0;
    SendInput(1, &ip, sizeof(INPUT));

    // Release the key
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release
    SendInput(1, &ip, sizeof(INPUT));
	*/
}


void SetClipBoard(CString input)
{
	//CString csText = _T("你好我的朋友");
	//std::wstring chine = L"你好我的朋友";
	
	CString csText = input;//strstr.c_str();
	
	HGLOBAL hglbCopy;
	// Open the clip board and set the data in to the clip board.
	if( OpenClipboard(0))
	{ 
	EmptyClipboard();
	wchar_t *wcBuffer = 0;
	hglbCopy = GlobalAlloc( GMEM_MOVEABLE,
	( csText.GetLength() + 1 ) * sizeof (wchar_t) ); 

	wcBuffer = ( wchar_t* )GlobalLock( hglbCopy );
	wcscpy( wcBuffer,  csText );

	GlobalUnlock( hglbCopy );
	SetClipboardData( CF_UNICODETEXT, hglbCopy );
	CloseClipboard();
	}
	
}


void VirtualKeyboard::message(std::wstring msg){

	SetClipBoard(msg.c_str());

	// Create a generic keyboard event structure
	
	INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

	// press ctrl + v

	// Press the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
 
	// Press the "V" key
    ip.ki.wVk = 'V';
    ip.ki.dwFlags = 0; // 0 for key press
    SendInput(1, &ip, sizeof(INPUT));
    // Release the "V" key
    ip.ki.wVk = 'V';
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
 
    // Release the "Ctrl" key
    ip.ki.wVk = VK_CONTROL;
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));

	

	
}


