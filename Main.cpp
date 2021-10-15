#include "Document.h"
#include "Chart.h"

int main()
{
	Document doc;
	Chart chart;

	doc.Load("testdata.txt");
	chart.Parse(&doc);
	chart.Draw();
	return 0;
}