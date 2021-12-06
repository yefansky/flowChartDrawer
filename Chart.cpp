#include "stdafx.h"
#include "Chart.h"
#include "Document.h"
#include <map>
#include <graphics.h>		// 引用图形库头文件
#include <conio.h>

#define DEVICE_WIDTH 1800
#define DEVICE_HEIGHT 1600

#define CONTENT_MAX_WIDTH_PER_LINE 400
#define PAGE_LEFT_DISTANCE	50
#define PAGE_TOP_DISTANCE 50
#define PAGE_RIGHT_DISTANCE 100

#define ZONE_LEFT_EDGE_INTERVAL 250
#define ZONE_WIDTH 200
#define ZONE_HEIGHT_INIT_VALUE 1800
#define ZONE_TITLE_TOP_DISTANCE 20
#define ZONE_TITLE_HEIGHT 100
#define ZONE_ROUND_RECT_CORNER_LENGTH 20
#define ZONE_WATER_MARK_TEXT_HEIGHT 50
#define ZONE_WATER_MARK_Y_INTERVAL 500

#define FLOW_UPPER_TEXT_REC2FAR_LINE_DISTANC 30
#define FLOW_UPPER_TEXT_REC2NEAR_LINE_DISTANC 10
#define FLOW_VERTICAL_LEFT_DISTANCE 10
#define FLOW_VERTICAL_TOP_DISTANCE 10
#define FLOW_VERTICAL_UPPER_TEXT_WIDTH 1000
#define FLOW_VERTICAL_LOWER_TEXT_WIDTH 60
#define FLOW_VERTICAL_BOTTOM_DISTANCE 10
#define FLOW_BLOCK_Y_DISTANCE 50
#define FLOW_VERTICAL_ARROW_EXT_LENGTH 60

static void DrawArraw(Point s, Point d)
{
	const int cnArrowEdgeDistance = 6;

	int x1 = s.m_nX, y1 = s.m_nY, x2 = d.m_nX, y2 = d.m_nY;

	line(x1, y1, x2, y2);
	double distance = sqrt((y1 - y2) * (y1 - y2) + (x1 - x2) * (x1 - x2));
	double tmpx = double(x1 + (x2 - x1) * (1 - (12 * sqrt(3) / 2) / distance));
	double tmpy = double(y1 + (y2 - y1) * (1 - (12 * sqrt(3) / 2) / distance));
	if (y1 == y2)
	{
		line(x2, y2, int(tmpx), int(tmpy + cnArrowEdgeDistance));
		line(x2, y2, int(tmpx), int(tmpy - cnArrowEdgeDistance));
	}
	else
	{
		double k = (double(x2) - double(x1)) / (double(y1) - double(y2));
		double increX = cnArrowEdgeDistance / sqrt(k * k + 1);
		double increY = cnArrowEdgeDistance * k / sqrt(k * k + 1);
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
	int				nCount	= 0;
	wchar_t			preC	= 0;
	std::wstring	strResult;

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
	initgraph(DEVICE_WIDTH, DEVICE_HEIGHT);
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
	bool						bResult		= false;
	int							nRetCode	= 0;
	int							nX			= PAGE_LEFT_DISTANCE;
	int							nY			= PAGE_TOP_DISTANCE;
	std::map<int, ZoneChart*>	index2ZoneChartMap;

	m_Zones.clear();
	m_Flows.clear();

	for (auto& rData : pDoc->m_Zones)
	{
		ZoneChart z;
		Point pos;
		z.m_pos = pos = { nX, nY };
		nX += ZONE_LEFT_EDGE_INTERVAL;
		z.m_nWidth = ZONE_WIDTH;
		z.m_nHeight = ZONE_HEIGHT_INIT_VALUE;
		z.m_strName = to_wide_string(rData.m_szName);
		z.m_nIndex = rData.m_nIndex;

		z.m_titleRect = { 
			pos.m_nX, pos.m_nY + ZONE_TITLE_TOP_DISTANCE, 
			pos.m_nX + z.m_nWidth, pos.m_nY + ZONE_TITLE_HEIGHT
		};
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
				nY += lines(f.m_strUpperText) * textheight(f.m_strUpperText.c_str()) + FLOW_UPPER_TEXT_REC2FAR_LINE_DISTANC;
			}

			f.m_start = { pSrcZone->CenterX(), nY };
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY - FLOW_UPPER_TEXT_REC2FAR_LINE_DISTANC,
				max(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY - FLOW_UPPER_TEXT_REC2NEAR_LINE_DISTANC
			};

			WordWrap(f.m_strLowerText, min(abs(f.m_start.m_nX - f.m_end.m_nX), CONTENT_MAX_WIDTH_PER_LINE));
			if (!f.m_strLowerText.empty())
			{
				nY += lines(f.m_strLowerText) * textheight(f.m_strLowerText.c_str()) + FLOW_UPPER_TEXT_REC2FAR_LINE_DISTANC;
			}

			f.m_labelRect2 = {
				min(f.m_start.m_nX, f.m_end.m_nX),
				f.m_start.m_nY + FLOW_UPPER_TEXT_REC2NEAR_LINE_DISTANC,
				max(f.m_start.m_nX, f.m_end.m_nX),
				nY
			};
			f.m_uFormat = f.m_start.m_nX < f.m_end.m_nX ? DT_LEFT : DT_RIGHT;
		}
		else
		{
			f.m_start = { pSrcZone->CenterX(), nY };
			WordWrap(f.m_strLowerText, CONTENT_MAX_WIDTH_PER_LINE);

			if (!f.m_strLowerText.empty())
				f.m_strLowerText = L"----------------\n" + f.m_strLowerText;
			auto s = f.m_strUpperText + L"\n" + f.m_strLowerText;
			int textHeight = textheight(s.c_str()) * lines(s);
			nY += textHeight + FLOW_VERTICAL_ARROW_EXT_LENGTH;
			f.m_end = { pDstZone->CenterX(), nY };
			f.m_labelRect = { 
				f.m_start.m_nX + FLOW_VERTICAL_LEFT_DISTANCE, 
				f.m_start.m_nY + FLOW_VERTICAL_TOP_DISTANCE,
				f.m_start.m_nX + FLOW_VERTICAL_LEFT_DISTANCE + FLOW_VERTICAL_UPPER_TEXT_WIDTH,
				(f.m_end.m_nY + f.m_start.m_nY - FLOW_VERTICAL_TOP_DISTANCE) / 2
			};
			f.m_labelRect2 = {
				f.m_start.m_nX + FLOW_VERTICAL_LEFT_DISTANCE, 
				(f.m_end.m_nY + f.m_start.m_nY - FLOW_VERTICAL_TOP_DISTANCE) / 2,
				f.m_start.m_nX + FLOW_VERTICAL_LEFT_DISTANCE + FLOW_VERTICAL_LOWER_TEXT_WIDTH,
				f.m_end.m_nY - FLOW_VERTICAL_BOTTOM_DISTANCE
			};
			f.m_uFormat = DT_LEFT;
		}

		nY += FLOW_BLOCK_Y_DISTANCE;

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
		fillroundrect(rPos.m_nX, rPos.m_nY, rPos.m_nX + rZone.m_nWidth, rPos.m_nY + rZone.m_nHeight, 
			ZONE_ROUND_RECT_CORNER_LENGTH, ZONE_ROUND_RECT_CORNER_LENGTH
		);
		
		{
			Color c(BLACK);
			drawtext(rZone.m_strName.c_str(), &rZone.m_titleRect, DT_CENTER | DT_NOPREFIX);
		}

		{
			Color c(WHITE);
			const int nDistance = ZONE_WATER_MARK_Y_INTERVAL;
			for (int nY = nDistance; nY < rZone.m_nHeight; nY += nDistance)
			{
				RECT rect = { 
					rPos.m_nX, rPos.m_nY + nY,  rPos.m_nX + rZone.m_nWidth,  
					rPos.m_nY + nY + ZONE_WATER_MARK_TEXT_HEIGHT 
				};
				drawtext(rZone.m_strName.c_str(), &rect, DT_CENTER | DT_NOPREFIX);
			}
		}
	}

	for (auto& rF : m_Flows)
	{
		Color c(BLACK);
		DrawArraw(rF.m_start, rF.m_end);

		drawtext(rF.m_strUpperText.c_str(), &rF.m_labelRect, rF.m_uFormat | DT_NOCLIP | DT_BOTTOM | DT_NOPREFIX);
		drawtext(rF.m_strLowerText.c_str(), &rF.m_labelRect2, rF.m_uFormat | DT_NOCLIP | DT_TOP | DT_NOPREFIX);
	}

	return true;
}

bool Chart::GetSize(int* pnWidth, int* pnHeight)
{
	int nHeight = 0;

	assert(pnWidth);
	assert(pnHeight);

	*pnWidth = m_Zones.size() * ZONE_LEFT_EDGE_INTERVAL + PAGE_RIGHT_DISTANCE;
	
	for (auto& rZ : m_Zones)
		if (nHeight < rZ.m_nHeight)
			nHeight = rZ.m_nHeight;

	*pnHeight = nHeight;

	return true;
}