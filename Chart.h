#pragma once
#include <vector>

class Document;

struct Point
{
	int m_nX;
	int m_nY;
};

struct ZoneChart
{
	Point m_pos;
	int m_nWidth;
	int m_nHeight;
	int m_nColor;
	char m_szName;
	int m_nIndex;
};

struct FlowChart
{
	Point m_start;
	Point m_end;
	char szUpperText[128];
	char szLowerText[128];
};

class Chart
{
	std::vector<ZoneChart> m_Zones;
	std::vector<FlowChart> m_Flows;
public:
	bool Parse(Document* pDoc);
	bool Calculate();
	bool Draw();
};

