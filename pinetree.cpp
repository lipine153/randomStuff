#include <iostream>
#include <windows.h>
#include <string>
#include <clocale>
#define BUFSIZE MAX_PATH

using namespace std;

wchar_t* BlackList[3] = {L".", L"..", L"desktop.ini"};
wstring RootDir = L"D:\\document";
wstring wildcard = L"\\*";


bool compareDir(wchar_t str[]);
bool isDir(unsigned int dir);
inline wstring toWideString(wchar_t* arr) {
    return wstring(arr);
}
void tree(wstring currentDir);
int wmain(int argc, wchar_t* argv[]){
    setlocale(LC_ALL, "chs");

    if (argc >= 2){
        std::wcout << argv[1] << std::endl;
    }else{
        std::cout << "No input args" << std::endl;
    }

//**************************************************
    tree(RootDir);
    return 0;
}

void tree(wstring currentDir){
    WIN32_FIND_DATAW dirData;
    HANDLE dirHandle = FindFirstFileW((currentDir + wildcard).c_str(), &dirData);
    do{
        if (compareDir(dirData.cFileName)){
            continue;
        }
        wcout << dirData.cFileName << endl;
        cout << dirData.dwFileAttributes << endl;
        if (isDir(dirData.dwFileAttributes)){
            tree((currentDir + toWideString(L"\\") + toWideString(dirData.cFileName)));
        }
    }while(FindNextFileW(dirHandle, &dirData) != 0);
}

bool compareDir(wchar_t str[]){//若在黑名单，返回true
    for (int i = 0; i <=2; i++){
        if (wcscmp(str, BlackList[i]) == 0)
            return true;
    }
    return false;
}

bool isDir(unsigned int dir){
    if ((dir & 0x10) == 0x10)
        return true;
    return false;
}