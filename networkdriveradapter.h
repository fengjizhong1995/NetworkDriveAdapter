#pragma once

#include <stdint.h>
#include <string>
#include <list>

class NetworkDriverAdapter
{
public:
	NetworkDriverAdapter();
	~NetworkDriverAdapter();

	int32_t Add(std::string& remoteName, std::string& userName, std::string& password);

	int32_t Add(std::string& localName, std::string& remoteName, std::string& userName, std::string& password);

	int64_t Scan();

	void Close(std::string& localName);

	void CloseAll();

private:
	std::string searchValidDriverLetter();

private:
	std::list<std::string> drive_list_;

};