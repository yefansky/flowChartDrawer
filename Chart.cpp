#include "stdafx.h"
#include "Chart.h"
#include "Document.h"
#include <map>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>

static const int s_cnContentMaxWidthPerLine = 400;

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

template <typename STR>
static int lines(const STR& str)
{
	int nResult = 0;
	for (auto& c : str)
		if (c == '\n')
			++nResult;
	return nResult;
}

static void WordWrap(std::wstring& rstrText, const size_t cnPixelWidthPerLine = 120)
{
	std::wstring strResult;
	int nCount = 0;
	wchar_t preC = 0;

	for (auto c : rstrText)
	{
		if (nCount == 0 && iswspace(c))
			NULL;
		else
		{
			nCount+= textwidth(c);

			if (c == L'\n')
			{
				nCount = 0;
			}
			else if (nCount > cnPixelWidthPerLine && !iswpunct(c))
			{
				if (iswalpha(c) && iswalpha(preC))
					strResult += L"-";

				strResult += L"\n";
				nCount = iswspace(c) ? 0 : textwidth(c);
			}
			strResult += c;
		}
		preC = c;
	}
	rstrText = strResult;
}

bool Chart::Init()
{
	//initgraph(1000, 1000, EW_SHOWCONSOLE);
	initgraph(1800, 1600);
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);
	return true;
}

void Chart::UnInit()
{
	closegraph();			// 关闭绘图窗口
}

COLORREF Chart::GetRandomLightColor()
{
	COLORREF colors[] = {
		RGB(200,200,200),
		RGB(200,200,255),
		RGB(255,200,200),
		RGB(200,255,200)
	};
	m_nColorRandomIndex = ++m_nColorRandomIndex % _countof(colors);
	return colors[m_nColorRandomIndex];
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
		Point pos;
		z.m_pos = pos = { nX, nY };
		nX += 250;
		z.m_nWidth = 200;
		z.m_nHeight = 1800;
		z.m_strName = to_wide_string(rData.m_szName);
		z.m_nIndex = rData.m_nIndex;

		z.m_titleRect = { pos.m_nX, pos.m_nY + 20, pos.m_nX + z.m_nWidth, pos.m_nY + 100 };
		z.m_color = GetRandomLightColor();

		m_Zones.push_back(z);
		index2ZoneChartMap[z.m_nIndex] = &m_Zones.back();
	}

	KGLOG_PROCESS_ERROR(!m_Zones.empty());
	nY = m_Zones.front().m_titleRect.bottom;

	for (auto& rData : pDoc->m_Flows)
	{
		FlowChart f;

		auto* pSrcZone = index2ZoneChartMap[rData.m_nSrc];
		auto* pDstZone = index2ZoneChartMap[rData.m_nDst];

		KGLOG_PROCESS_ERROR(pSrcZone);
		KGLOG_PROCESS_ERROR(pDstZone);

		f.m_strUpperText = to_wide_string(rData.m_szProtocolName);
		f.m_strLowerText = to_wide_string(rData.m_szDatas);
		if (rData.m_szComment[0])
		{
			f.m_strLowerText += L"\n";
			f.m_strLowerText += to_wide_string(rData.m_szComment);
		}

		if (pSrcZone != pDstZone)
		{
			if (!f.m_strUpperText.empty())
			{
				nY += lines(f.m_strUpperText) * textheight(f.m_strUpperText.c_str()) + 30;
			}

			f.m_start = { pSrcZone->CenterX(), nY };
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY - 30,
				max(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY - 10
			};

			WordWrap(f.m_strLowerText, min(abs(f.m_start.m_nX - f.m_end.m_nX), s_cnContentMaxWidthPerLine));
			if (!f.m_strLowerText.empty())
			{
				nY += lines(f.m_strLowerText) * textheight(f.m_strLowerText.c_str()) + 30;
			}

			f.m_labelRect2 = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY + 10,
				max(f.m_start.m_nX, f.m_end.m_nX),
				nY
			};
			f.m_uFormat = f.m_start.m_nX < f.m_end.m_nX ? DT_LEFT : DT_RIGHT;
		}
		else
		{
			f.m_start = { pSrcZone->CenterX(), nY };
			WordWrap(f.m_strLowerText, s_cnContentMaxWidthPerLine);

			if (!f.m_strLowerText.empty())
				f.m_strLowerText = L"----------------\n" + f.m_strLowerText;
			auto s = f.m_strUpperText + L"\n" + f.m_strLowerText;
			int textHeight = textheight(s.c_str()) * lines(s);
			nY += textHeight + 60;
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = { 
				f.m_start.m_nX + 10, f.m_start.m_nY + 10, 
				f.m_start.m_nX + 10 + 1000, (f.m_end.m_nY + f.m_start.m_nY - 10) / 2
			};
			f.m_labelRect2 = {
				f.m_start.m_nX + 10, (f.m_end.m_nY + f.m_start.m_nY - 10) / 2,
				f.m_start.m_nX + 10 + 60, f.m_end.m_nY - 10
			};
			f.m_uFormat = DT_LEFT;
		}

		nY += 50;

		m_Flows.push_back(f);
	}

	for (auto& rZone : m_Zones)
		rZone.m_nHeight = nY;

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
	setbkcolor(WHITE);
	setbkmode(TRANSPARENT);
	cleardevice();

	for (auto& rZone : m_Zones)
	{
		auto& rPos = rZone.m_pos;
		Color c(rZone.m_color);
		fillroundrect(rPos.m_nX, rPos.m_nY, rPos.m_nX + rZone.m_nWidth, rPos.m_nY + rZone.m_nHeight, 20, 20);
		
		{
			Color c(BLACK);
			drawtext(rZone.m_strName.c_str(), &rZone.m_titleRect, DT_CENTER);
		}

		{
			Color c(WHITE);
			const int nDistance = 500;
			for (int nY = nDistance; nY < rZone.m_nHeight; nY += nDistance)
			{
				RECT rect = { rPos.m_nX, rPos.m_nY + nY,  rPos.m_nX + rZone.m_nWidth,  rPos.m_nY + nY + 50 };
				drawtext(rZone.m_strName.c_str(), &rect, DT_CENTER);
			}
		}
	}

	for (auto& rF : m_Flows)
	{
		Color c(BLACK);
		DrawArraw(rF.m_start, rF.m_end);

		drawtext(rF.m_strUpperText.c_str(), &rF.m_labelRect, rF.m_uFormat | DT_NOCLIP | DT_BOTTOM);
		drawtext(rF.m_strLowerText.c_str(), &rF.m_labelRect2, rF.m_uFormat | DT_NOCLIP | DT_TOP);
	}

	return true;
}

bool Chart::GetSize(int* pnWidth, int* pnHeight)
{
	int nHeight = 0;

	assert(pnWidth);
	assert(pnHeight);

	*pnWidth = m_Zones.size()* 250 + 100;
	
	for (auto& rZ : m_Zones)
		if (nHeight < rZ.m_nHeight)
			nHeight = rZ.m_nHeight;

	*pnHeight = nHeight;

	return true;
}