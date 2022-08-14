#include "networkdriveradapter.h"
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "mpr.lib")
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Winnetwk.h>
#include <filesystem>

namespace fs = std::filesystem;


NetworkDriverAdapter::NetworkDriverAdapter()
{

}

NetworkDriverAdapter::~NetworkDriverAdapter()
{

}

int64_t NetworkDriverAdapter::Scan()
{
    int64_t count = 0;
    //fs::create_directories("sandbox/a/b");
    //std::ofstream("sandbox/file1.txt");
    //fs::create_symlink("a", "sandbox/syma");
    //for (auto& p : fs::recursive_directory_iterator("sandbox"))
    //    std::cout << p.path() << '\n';
    //fs::remove_all("sandbox");
    // skip_permission_denied
    std::string localName = drive_list_.front() + "\\";
    for (auto& entry : fs::recursive_directory_iterator{ localName, fs::directory_options::skip_permission_denied})
    {
        if (fs::is_regular_file(entry.status()))
        {
            count++;
            std::cout << "file: " << entry.path() << '\n';
        }
        else
            std::cout << "dir: " << entry.path() << '\n';
    }

    std::cout << "found file count: " << count << '\n';

    return count;
}

// https://docs.microsoft.com/en-us/windows/win32/api/winnetwk/nf-winnetwk-wnetaddconnection2a
int32_t NetworkDriverAdapter::Add(std::string& remoteName, std::string& userName, std::string& password)
{
    DWORD dwRetVal;

    NETRESOURCEA nr;
    DWORD dwFlags;
    /*
    wprintf(L"Calling WNetAddConnection2 with\n");
    wprintf(L"  lpLocalName = %s\n", argv[1]);
    wprintf(L"  lpRemoteName = %s\n", argv[2]);
    wprintf(L"  lpUsername = %s\n", argv[3]);
    wprintf(L"  lpPassword = %s\n", argv[4]);
    */
    // Zero out the NETRESOURCE struct
    memset(&nr, 0, sizeof(NETRESOURCE));
    //
    auto localName = searchValidDriverLetter();
    // Assign our values to the NETRESOURCE structure.
    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = (LPSTR)localName.c_str();
    nr.lpRemoteName = (LPSTR)remoteName.c_str();
    nr.lpProvider = NULL;

    // Assign a value to the connection options
    dwFlags = CONNECT_UPDATE_PROFILE;
    //
    // Call the WNetAddConnection2 function to assign
    //   a drive letter to the share.
    //
    dwRetVal = WNetAddConnection2A(&nr, password.c_str(), userName.c_str(), dwFlags);
    //
    // If the call succeeds, inform the user; otherwise,
    //  print the error.
    //
    if (dwRetVal == NO_ERROR)
    {
        drive_list_.push_back(localName);
        wprintf(L"Connection added to %s\n", nr.lpRemoteName);
        return 0;
    }
    else
    {
        wprintf(L"WNetAddConnection2 failed with error: %u\n", dwRetVal);
    }
    //
    return -1;
}

int32_t NetworkDriverAdapter::Add(std::string& localName, std::string& remoteName, std::string& userName, std::string& password)
{
    return 0;
}

void NetworkDriverAdapter::Close(std::string& localName)
{
    DWORD dwFlags = CONNECT_UPDATE_PROFILE;
    WNetCancelConnection2A(localName.c_str(), dwFlags, TRUE);
}

void NetworkDriverAdapter::CloseAll()
{
    for (auto& localName : drive_list_)
        Close(localName);
}

std::string NetworkDriverAdapter::searchValidDriverLetter()
{
    std::vector<std::string> drive_vector;
    //
    const int32_t BUFSIZE = 1024;
    char szTemp[BUFSIZE] = { 0x00 };
    if (GetLogicalDriveStringsA(BUFSIZE - 1, szTemp))
    {
        int32_t offset = 0;
        const int32_t constSegmentSize = 4;
        while (offset < BUFSIZE)
        {
            char szDrive[8] = { 0x00 };
            memcpy(szDrive, szTemp + offset, constSegmentSize);
            if (strlen(szDrive) == 0)
                break;
            //
            drive_vector.emplace_back(std::string(szDrive));
            offset += constSegmentSize;
        }
    }
    const char endLetter = 'A';
    char letter = 'N';
    while (letter > endLetter)
    {
        bool found = false;
        for (auto& drive : drive_vector)
        {
            std::transform(drive.begin(), drive.end(), drive.begin(), ::toupper);
            if (drive.at(0) == letter)
            {
                found = true;
                break;
            }
        }
        if (!found)
            break;
        //
        letter--;
    }
    //
    char szBuf[8] = { 0x00 };
    sprintf_s(szBuf, "%c:", letter);
    //
    return std::string(szBuf);
}