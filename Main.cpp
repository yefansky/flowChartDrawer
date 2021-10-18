#include "stdafx.h"
#include "Document.h"
#include "Chart.h"
#include <time.h>
#include <sys/stat.h>

time_t GetFileModifyTime(const char* cpszFilePath)
{
	struct stat attrib;
	stat(cpszFilePath, &attrib);
	time_t date = attrib.st_mtime;
	return date;
}

int main(int nArgNum, char** ppArgs)
{
	bool bRetCode = false;
	Document doc;
	Chart chart;
	POINT mousepos = {-1, -1};
	const char* cpszDocPath = "testdata.txt";
	ExMessage msg;
	time_t nLastModifyTime = 0;
	time_t nNextCheckFileChangeTime = 0;

	if (nArgNum == 2)
		cpszDocPath = ppArgs[1];

	bRetCode = chart.Init();
	KGLOG_PROCESS_ERROR(bRetCode &&"Init");
	
	bRetCode = doc.Load(cpszDocPath);
	KGLOG_PROCESS_ERROR(bRetCode && "Load");

	nLastModifyTime = GetFileModifyTime(cpszDocPath);

	bRetCode = chart.Parse(&doc);
	KGLOG_PROCESS_ERROR(bRetCode && "Parse");

	bRetCode = chart.Draw();
	KGLOG_PROCESS_ERROR(bRetCode && "Draw");

	while (true)
	{
		bool bUpdate = false;
		static bool s_bFirst = true;
		time_t nNow = time(NULL);

		if (nNow > nNextCheckFileChangeTime)
		{
			time_t modifyTime = GetFileModifyTime(cpszDocPath);

			if (nLastModifyTime != modifyTime)
			{
				doc.Reset();
				bRetCode = doc.Load(cpszDocPath);
				if (bRetCode)
				{
					chart.Reset();
					chart.Parse(&doc);
					bUpdate = true;
					nLastModifyTime = modifyTime;
				}
			}
			nNextCheckFileChangeTime = nNow + 1;
		}

		if (peekmessage(&msg, EM_MOUSE, true))
		{
			static POINT origPos = { 0, 0 };

			switch (msg.message)
			{
			case WM_MOUSEWHEEL:
				{
					static int nYOffset = 0;
					origPos.y += msg.wheel;;
					setorigin(origPos.x, origPos.y);
					bUpdate = true;
				}
				break;
			case WM_MOUSEMOVE:
				if (msg.lbutton)
				{
					if (mousepos.x != -1)
					{
						origPos.x += msg.x - mousepos.x;
						origPos.y += msg.y - mousepos.y;
						setorigin(origPos.x, origPos.y);
					}
					bUpdate = true;
				}
				mousepos = { msg.x, msg.y };
				break;
			}
		}

		if (bUpdate || s_bFirst)
		{
			bRetCode = chart.Draw();
			KGLOG_PROCESS_ERROR(bRetCode && "Draw");
		}

		s_bFirst = false;

		Sleep(1);
	}
Exit0:	
	return 0;
}