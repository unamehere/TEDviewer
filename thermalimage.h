#ifndef THERMALIMAGE_H
#define THERMALIMAGE_H

#include <QWidget>

//Attention: every get/set Pixel from/to Img Needs Inverted Y-Axis y = resY-1-y!!!!!

/*   0 1 2 3 4 5 6 7 8 x
 * 8
 * 7
 * 6
 * 5
 * 4
 * 3
 * 2
 * 1
 * 0
 * y
 */

class ThermalImage : public QWidget
{
    Q_OBJECT
private:
    QImage* img;
    QImage* gradientImg;
    QVector<QVector<float>>* tempMap;
    QList<QColor> colorGradient;
    unsigned resX;
    unsigned resY;
    float minT = 10;
    float maxT = 30;
    QPoint minP;
    QPoint maxP;
    bool fixedMinMax;
    int getColorIndex(const float& val);
    void fillGradient();
    void findMinTemp();
    void findMaxTemp();
    void fillGradientImg();
public:
    explicit ThermalImage(unsigned resX_ = 1 , unsigned resY_ = 1,QWidget *parent = nullptr);
    ~ThermalImage();
    const float& getMin() const;
    const float& getMax() const;
    unsigned getResX() const;
    unsigned getResY() const;
    void addPixel(int x, int y, float value);
    bool addBlock(int startX, int startY, int sizeX, int sizeY, QVector<QVector<float>>& vals);
    const QImage *getImg() const;
    const float& getTemp(int x, int y);

    const QImage *getGradientImg() const;

    void setFixedMinMax(bool value);

signals:
    void newMinMax();
    void imageComplete();

public slots:
    void updateGradient();
    void handleImageComplete();
    void handle_newFixedTemps(float min, float max);
};

#endif // THERMALIMAGE_H
