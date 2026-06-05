#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include <regex>

#include <locale>
#define BUFSIZE MAX_PATH

using namespace std;

wchar_t* BlackList[3] = {L".", L"..", L"desktop.ini"};
wstring RootDir = L"D:\\document";
wstring wildcard = L"\\*";

locale chineseLocale("chs");

bool compareDir(wchar_t str[]);
bool isDir(unsigned int dir);
inline wstring toWideString(wchar_t* arr) {//将wchar_t* 变为wstring
    return wstring(arr);
}
void tree(wstring currentDir, wregex searchRegex);
int wmain(int argc, wchar_t* argv[]){
    wcout.imbue(chineseLocale);

    vector<wstring> vectorArg;
    for (int i = 1; i < argc; i++){
        vectorArg.emplace_back(toWideString(argv[i]));
        wcout << vectorArg[i-1] << endl;
    }//得到输入参数向量
    if (argc == 1){
        std::cout << "No input args" << std::endl;
    }

/*******************************************************************/
    wregex searchRegex(L"\\.pdf$");
    tree(RootDir, searchRegex);
    return 0;
}

void tree(wstring currentDir, wregex searchRegex){//递归函数来深度遍历目录树
    static wofstream outputDocList(L"docList.txt");

    outputDocList.imbue(chineseLocale);
    wcout.imbue(chineseLocale);
    //绑定语言

    WIN32_FIND_DATAW dirData;
    HANDLE dirHandle = FindFirstFileW((currentDir + wildcard).c_str(), &dirData);
    do{
        if (compareDir(dirData.cFileName)){
            continue;//黑名单检测
        }
        if (regex_search(dirData.cFileName, searchRegex)){
            //文件路径写入docList
            outputDocList << (currentDir + toWideString(L"\\") + toWideString(dirData.cFileName)) << endl;
//            wcout << dirData.cFileName << endl;
        }
        if (isDir(dirData.dwFileAttributes)){
            //递归在这里
            tree((currentDir + toWideString(L"\\") + toWideString(dirData.cFileName)), searchRegex);

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

bool isDir(unsigned int dir){//判断WIN32_FIND_DATAW结构中的DWORD dwFileAttributes值的第四位，表示目录
    if ((dir & 0x10) == 0x10)
        return true;
    return false;
}