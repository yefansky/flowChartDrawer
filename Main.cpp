#include "Document.h"
#include "Chart.h"

int main()
{
	Document doc;
	Chart chart;
	POINT mousepos = {-1, -1};

	chart.Init();
	
	doc.Load("testdata.txt");
	
	chart.Parse(&doc);

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
			chart.Draw();
		}

		s_bFirst = false;
	}
	
	return 0;
}