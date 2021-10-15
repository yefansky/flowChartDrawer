#include "stdafx.h"
#include "Chart.h"
#include "Document.h"
#include <map>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>
#include <string>
#include <locale>
#include <codecvt>

static void DrawArraw(Point s, Point d)
{
	int x1 = s.m_nX, y1 = s.m_nY, x2 = d.m_nX, y2 = d.m_nY;

	line(x1, y1, x2, y2);
	double distance = sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
	double tmpx = double(x1 + (x2 - x1) * (1 - (12 * sqrt(3) / 2) / distance));
	double tmpy = double(y1 + (y2 - y1) * (1 - (12 * sqrt(3) / 2) / distance));
	if (y1 == y2)
	{
		line(x2, y2, int(tmpx), int(tmpy + 6));
		line(x2, y2, int(tmpx), int(tmpy - 6));
	}
	else
	{
		double k = (double(x2) - double(x1)) / (double(y1) - double(y2));
		double increX = 6 / sqrt(k * k + 1);
		double increY = 6 * k / sqrt(k * k + 1);
		line(x2, y2, int(tmpx + increX), int(tmpy + increY));
		line(x2, y2, int(tmpx - increX), int(tmpy - increY));
	}
}

inline std::wstring to_wide_string(const std::string& input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}
// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}

bool Chart::Parse(Document* pDoc)
{
	bool bResult = false;
	int nRetCode = 0;
	int nX = 50;
	int nY = 50;
	std::map<int, ZoneChart*> index2ZoneChartMap;

	m_Zones.clear();
	m_Flows.clear();

	for (auto& rData : pDoc->m_Zones)
	{
		ZoneChart z;
		z.m_pos = { nX, nY };
		nX += 250;
		z.m_nWidth = 200;
		z.m_nHeight = 800;
		z.m_strName = rData.m_szName;
		z.m_nIndex = rData.m_nIndex;

		m_Zones.push_back(z);
		index2ZoneChartMap[z.m_nIndex] = &m_Zones.back();
	}

	nY += 100;

	for (auto& rData : pDoc->m_Flows)
	{
		FlowChart f;
		nY += 100;
		auto* pSrcZone = index2ZoneChartMap[rData.m_nSrc];
		auto* pDstZone = index2ZoneChartMap[rData.m_nDst];

		KGLOG_PROCESS_ERROR(pSrcZone);
		KGLOG_PROCESS_ERROR(pDstZone);

		f.m_start = { pSrcZone->CenterX(), nY };

		if (pSrcZone != pDstZone)
		{
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY - 30,
				max(f.m_start.m_nX, f.m_end.m_nY),
				f.m_start.m_nY - 10
			};
			f.m_labelRect2 = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY + 10,
				max(f.m_start.m_nX, f.m_end.m_nY),
				f.m_start.m_nY + 50
			};
			f.m_uFormat = f.m_start.m_nX < f.m_end.m_nX ? DT_LEFT : DT_RIGHT;
		}
		else
		{
			nY += 100;
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = { 
				f.m_start.m_nX + 10, f.m_start.m_nY + 10, 
				f.m_start.m_nX + 10 + 1000, (f.m_end.m_nY + 10 + f.m_start.m_nY) / 2
			};
			f.m_labelRect2 = {
				f.m_start.m_nX + 10, (f.m_end.m_nY + 10 + 10 + f.m_start.m_nY) / 2,
				f.m_start.m_nX + 10 + 1000, f.m_end.m_nY
			};
			f.m_uFormat = DT_LEFT;
		}

		nY += 5;
		f.m_strUpperText = rData.m_szProtocolName;
		f.m_strLowerText = rData.m_szDatas;
		if (rData.m_szComment[0])
		{
			f.m_strLowerText += "\n----------------\n";
			f.m_strLowerText += rData.m_szComment;
		}

		m_Flows.push_back(f);
	}

	bResult = true;
Exit0:
	return bResult;
}

bool Chart::Calculate()
{
	return true;
}

bool Chart::Draw()
{	
	initgraph(1280, 960);	// 创建绘图窗口，大小为 640x480 像素
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);
	cleardevice();

	for (auto& rZone : m_Zones)
	{
		auto& rPos = rZone.m_pos;
		Color c(RGB(200,200,255));
		fillroundrect(rPos.m_nX, rPos.m_nY, rPos.m_nX + rZone.m_nWidth, rPos.m_nY + rZone.m_nHeight, 20, 20);
		
		{
			Color c(BLACK);
			RECT rect{ rPos.m_nX, rPos.m_nY + 20, rPos.m_nX + rZone.m_nWidth, rPos.m_nY + 100 };
			drawtext(to_wide_string(rZone.m_strName).c_str(), &rect, DT_CENTER);
		}
	}

	for (auto& rF : m_Flows)
	{
		Color c(BLACK);
		DrawArraw(rF.m_start, rF.m_end);

		drawtext(to_wide_string(rF.m_strUpperText).c_str(), &rF.m_labelRect, rF.m_uFormat | DT_NOCLIP);
		drawtext(to_wide_string(rF.m_strLowerText).c_str(), &rF.m_labelRect2, rF.m_uFormat | DT_NOCLIP);
	}
	
	_getch();				// 按任意键继续
	closegraph();			// 关闭绘图窗口
	return true;
}