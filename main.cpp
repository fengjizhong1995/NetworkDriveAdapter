// NetworkDriverAdapter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "networkdriveradapter.h"

int main()
{
    NetworkDriverAdapter adapter;
    std::string remoteName = "\\\\192.168.137.129\\project";
    std::string userName = "root";
    std::string password = "Wein";
    //
    adapter.Add(remoteName, userName, password);
    adapter.Scan();
}
