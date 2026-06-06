#include <iostream>
#include <fstream>
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "shell32.lib")
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

int tree(wstring currentDir, wregex searchRegex);
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
    int count;
    for (int i = 0;i < vectorArg.size(); i++){//搜索输入参数循环
        if (vectorArg[i] == L"-s"){
            wregex searchRegex;
            if (i != vectorArg.size() - 1){
                searchRegex = (vectorArg[i+1]);//搜索具体文件
            }else{
                searchRegex = L".*";//列出所有文件
            }
            count = tree(RootDir, searchRegex);//写入文件、计数
        }
        if (vectorArg[i] == L"-pdf"){
            wregex searchRegex;
            if (i != vectorArg.size() - 1){
                searchRegex = (L"^(?=.*" + vectorArg[i+1] + L").*\\.pdf$");//搜索具体pdf
            }else{
                searchRegex = L"\\.pdf$";//搜索.pdf
            }
            count = tree(RootDir, searchRegex);//写入文件、计数
        }
        if (vectorArg[i] == L"-o"){
            if (i == vectorArg.size() - 1){break;}
            wifstream inputDocList(L"docList.txt");
            wstring line;

            inputDocList.imbue(chineseLocale);

            int linecount = 0;
            while(getline(inputDocList, line)){
                linecount++;
                if(linecount == stoi(vectorArg[i+1])){
                    HINSTANCE result = ShellExecuteW(NULL, L"open", line.c_str(), NULL, NULL, SW_SHOWNORMAL);
                    if ((INT_PTR)result <= 32) {
                        cerr << "打开失败，错误码: " << GetLastError() << endl;
                    }
                    break;
                }
            }
        }
    }

/*******************************************************************/
    return 0;
}

int tree(wstring currentDir, wregex searchRegex){//递归函数来深度遍历目录树,返回写入个数
    static wofstream outputDocList(L"docList.txt");

    outputDocList.imbue(chineseLocale);
    wcout.imbue(chineseLocale);
    //绑定语言

    static int count = 0;//写入计数

    WIN32_FIND_DATAW dirData;
    HANDLE dirHandle = FindFirstFileW((currentDir + wildcard).c_str(), &dirData);
    do{
        if (compareDir(dirData.cFileName)){
            continue;//黑名单检测
        }
        if (regex_search(dirData.cFileName, searchRegex) && (!isDir(dirData.dwFileAttributes))){
            //文件路径写入docList
            outputDocList << (currentDir + toWideString(L"\\") + toWideString(dirData.cFileName)) << endl;
            count++;//增加计数
            wcout << count << " = " << dirData.cFileName << endl;
        }
        if (isDir(dirData.dwFileAttributes)){
            //递归在这里
            tree((currentDir + toWideString(L"\\") + toWideString(dirData.cFileName)), searchRegex);

        }
    }while(FindNextFileW(dirHandle, &dirData) != 0);
    return count;
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