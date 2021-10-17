#include "stdafx.h"
#include "Document.h"
#include "Chart.h"
#include <time.h>

int main(int nArgNum, char** ppArgs)
{
	bool bRetCode = false;
	Document doc;
	Chart chart;
	POINT mousepos = {-1, -1};
	const char* cpszDocPath = "testdata.txt";
	
	srand(time(nullptr));

	if (nArgNum == 2)
		cpszDocPath = ppArgs[1];

	bRetCode = chart.Init();
	KGLOG_PROCESS_ERROR(bRetCode &&"Init");
	
	bRetCode = doc.Load(cpszDocPath);
	KGLOG_PROCESS_ERROR(bRetCode && "Load");
	
	bRetCode = chart.Parse(&doc);
	KGLOG_PROCESS_ERROR(bRetCode &&"Parse");

	while (true)
	{
		auto msg = getmessage(EM_MOUSE);

		static bool s_bFirst = true;
		static POINT origPos = { 0, 0 };

		bool bUpdate = false;

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

		if (bUpdate || s_bFirst)
		{
			bRetCode = chart.Draw();
			KGLOG_PROCESS_ERROR(bRetCode && "Draw");
		}

		s_bFirst = false;
	}
Exit0:	
	return 0;
}