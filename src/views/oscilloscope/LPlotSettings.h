#ifndef LPLOTSETTINGS_H
#define LPLOTSETTINGS_H

#include <qglobal.h>

class LPlotter;


//PlotSettings determines the range of x and y, and number of scales
class LPlotSettings
{
    friend class LPlotter;

public:
    LPlotSettings();

private:
    static void adjustAxis(qreal &min, qreal &max, int &numTicks);

    void scroll(int dx, int dy);
    void adjust();
    qreal spanX() const { return maxX - minX; }
    qreal spanY() const { return maxY - minY; }

    qreal minX;
    qreal maxX;
    int numXTicks;
    qreal minY;
    qreal maxY;
    int numYTicks;
};

#endif // LPLOTSETTINGS_H
