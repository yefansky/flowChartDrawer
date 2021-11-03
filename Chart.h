#pragma once
#include <list>
#include <string>
#include <graphics.h>		// 引用图形库头文件

class Document;

struct Point
{
	int m_nX = 0;
	int m_nY = 0;
};

struct ZoneChart
{
	Point m_pos;
	int m_nWidth = 0;
	int m_nHeight = 0;
	COLORREF m_color;
	std::wstring m_strName = L"";
	int m_nIndex = 0;
	RECT m_titleRect = {0,0,0,0};

	int CenterX()
	{
		return m_pos.m_nX + m_nWidth / 2;
	}
};

struct FlowChart
{
	Point m_start;
	Point m_end;
	std::wstring m_strUpperText = L"";
	std::wstring m_strLowerText = L"";
	RECT m_labelRect = { 0,0,0,0 };
	RECT m_labelRect2 = { 0,0,0,0 };
	unsigned int m_uFormat = 0;
};

class Color
{
private:
	COLORREF m_OrinColor;

public:
	Color(COLORREF color)
	{
		m_OrinColor = getlinecolor();
		setlinecolor(color);
		setfillcolor(color);
		settextcolor(color);
	}

	~Color() {
		setlinecolor(m_OrinColor);
		setfillcolor(m_OrinColor);
		settextcolor(m_OrinColor);
	}
};

class Chart
{
	std::list<ZoneChart> m_Zones;
	std::list<FlowChart> m_Flows;
public:
	bool Init();
	void UnInit();
	void Reset()
	{ 
		m_Zones.clear(); 
		m_Flows.clear();
		m_nColorRandomIndex = 0;
	}

	bool Parse(Document* pDoc);
	bool Calculate();
	bool Draw();
	bool GetSize(int* pnWidth, int* pnHeight);
private:
	COLORREF GetRandomLightColor();

private:
	int m_nColorRandomIndex = 0;
};

