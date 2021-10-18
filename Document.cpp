#include "stdafx.h"
#include "Document.h"
#include <map>

enum class SECTION
{
	INVALID,
	ZONE,
	FLOW
};

static int key2index(const char* cpszKey, const std::map<std::string, int>& cMap)
{
	auto itFinder = cMap.find(cpszKey);
	if (itFinder != cMap.end())
		return itFinder->second;
	return -1;
}

bool Document::Load(const char* cpszPath)
{
	bool bResult = false;
	int nRetCode = 0;
	FILE* pfFile = nullptr;
	char szBuffer[1024];
	SECTION section = SECTION::INVALID;
	int nZoneIndexGenerator = 0;
	std::map<std::string, int> nick2indexMap;
	errno_t nError;

	nError = fopen_s(&pfFile, cpszPath, "r");
	KGLOG_PROCESS_ERROR(nError == 0 && pfFile);

	while (fgets(szBuffer, sizeof(szBuffer), pfFile))
	{
		if (szBuffer[0] == 0 || szBuffer[0] == '\n')
			continue;

		if (strstr(szBuffer, "zone:"))
		{
			section = SECTION::ZONE;
		}
		else if (strstr(szBuffer, "flow:"))
		{
			section = SECTION::FLOW;
		}
		else
		{
			switch (section)
			{
			case SECTION::ZONE:
				{
					Zone z;

					nRetCode = sscanf_s(szBuffer, "%[^(](%[^)]", z.m_szName, (unsigned)_countof(z.m_szName), z.m_szNickName, (unsigned)_countof(z.m_szNickName));
					KGLOG_PROCESS_ERROR(nRetCode == 1 || nRetCode == 2);

					if (nRetCode == 1)
						SAFE_STR_CPY(z.m_szNickName, z.m_szName);
					z.m_nIndex = ++nZoneIndexGenerator;
					auto insertRet = nick2indexMap.emplace(z.m_szNickName, z.m_nIndex);
					KGLOG_PROCESS_ERROR(insertRet.second && "zone nick name duplicate!");
					m_Zones.push_back(z);
				}
				break;
			case SECTION::FLOW:
				{
					Flow f;
					char szSrc[32];
					char szDst[32];
					nRetCode = sscanf_s(szBuffer, "%[^2]2%s %s %[^#\n]", 
						szSrc, (unsigned)_countof(szSrc), 
						szDst, (unsigned)_countof(szDst), 
						f.m_szProtocolName, (unsigned)_countof(f.m_szProtocolName), 
						f.m_szDatas, (unsigned)_countof(f.m_szDatas)
					);
					KGLOG_PROCESS_ERROR(nRetCode >= 3);

					sscanf_s(szBuffer, "%*[^#]#%[^$\n]", f.m_szComment, (unsigned)_countof(f.m_szComment));

					f.m_nSrc = key2index(szSrc, nick2indexMap);
					f.m_nDst = key2index(szDst, nick2indexMap);
					m_Flows.push_back(f);
				}
				break;
			}
		}
	}

	bResult = true;
Exit0:
	if (pfFile)
	{
		fclose(pfFile);
		pfFile = nullptr;
	}
	return bResult;
}