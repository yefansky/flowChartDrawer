#pragma once

#include <string>
#include <vector>

struct Zone
{
	char m_szName[128] = "";
	char m_szNickName[32] = "";
	int m_nIndex = -1;
};

struct Flow
{
	char m_szProtocolName[128] = "";
	char m_szDatas[512] = "";
	char m_szComment[512] = "";
	int m_nSrc = -1;
	int m_nDst = -1;
};

class Document
{
public:
	std::vector<Zone> m_Zones;
	std::vector<Flow> m_Flows;
public:
	bool Load(const char* cpszPath);
};

