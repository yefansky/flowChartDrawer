#pragma once

#include <string>
#include <vector>

#define ZONE_NAME_LEN			128
#define ZONE_NICKNAME_LEN		32
#define FLOW_NAME_LEN			128
#define FLOW_DATA_TEXT_LEN		512
#define FLOW_COMMENT_TEXT_LEN	512

struct Zone
{
	char m_szName[ZONE_NAME_LEN]			= "";
	char m_szNickName[ZONE_NICKNAME_LEN]	= "";
	int m_nIndex = -1;
};

struct Flow
{
	char m_szProtocolName[FLOW_NAME_LEN]	= "";
	char m_szDatas[FLOW_DATA_TEXT_LEN]		= "";
	char m_szComment[FLOW_COMMENT_TEXT_LEN] = "";
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
	void Reset()
	{
		m_Zones.clear();
		m_Flows.clear();
	}
};

