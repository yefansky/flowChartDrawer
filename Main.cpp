#include "stdafx.h"
#include "Document.h"
#include "Chart.h"
#include <time.h>
#include <sys/stat.h>

time_t GetFileModifyTime(const char* cpszFilePath)
{
	struct stat attrib;
	time_t		date;

	stat(cpszFilePath, &attrib);
	date = attrib.st_mtime;
	
	return date;
}

int main(int nArgNum, char** ppArgs)
{
	bool		bRetCode					= false;
	const char* cpszDocPath					= "testdata.txt";
	ExMessage	msg;
	POINT		mousepos					= { -1, -1 };
	POINT		origPos						= { 0, 0 };
	time_t		nLastModifyTime				= 0;
	time_t		nNextCheckFileChangeTime	= 0;
	IMAGE		img(2000, 20000);
	Document	doc;
	Chart		chart;

	if (nArgNum == 2)
		cpszDocPath = ppArgs[1];

	bRetCode = chart.Init();
	KGLOG_PROCESS_ERROR(bRetCode &&"Init");
	
	bRetCode = doc.Load(cpszDocPath);
	KGLOG_PROCESS_ERROR(bRetCode && "Load");

	nLastModifyTime = GetFileModifyTime(cpszDocPath);

	bRetCode = chart.Parse(&doc);
	KGLOG_PROCESS_ERROR(bRetCode && "Parse");

	while (true)
	{
		bool bMove = false;
		bool bRedraw = false;
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
					bRedraw = true;
					nLastModifyTime = modifyTime;
				}
			}
			nNextCheckFileChangeTime = nNow + 1;
		}

		if (bRedraw || s_bFirst)
		{
			SetWorkingImage(&img);
			bRetCode = chart.Draw();
			SetWorkingImage();
			KGLOG_PROCESS_ERROR(bRetCode && "Draw");

			cleardevice();
			putimage(origPos.x, origPos.y, &img);
		}

		if (peekmessage(&msg, EM_MOUSE, true))
		{
			switch (msg.message)
			{
			case WM_MOUSEWHEEL:
				{
					static int nYOffset = 0;
					origPos.y += msg.wheel;;
					bMove = true;
				}
				break;
			case WM_MOUSEMOVE:
				if (msg.lbutton)
				{
					if (mousepos.x != -1)
					{
						origPos.x += msg.x - mousepos.x;
						origPos.y += msg.y - mousepos.y;
					}
					bMove = true;
				}
				mousepos = { msg.x, msg.y };
				break;
			}

			if (bMove)
			{
				cleardevice();
				putimage(origPos.x, origPos.y, &img);
			}
		}

		s_bFirst = false;

		Sleep(1);
	}
Exit0:	
	return 0;
}