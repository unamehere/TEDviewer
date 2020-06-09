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
    QVector<QVector<float>>* tempMap;
    QList<QColor> colorGradient;
    unsigned resX;
    unsigned resY;
    float minT = 20;
    float maxT = 21;
    int getPercent(const float& val); //0->255
    void fillGradient();
public:
    explicit ThermalImage(unsigned resX_ = 1 , unsigned resY_ = 1,QWidget *parent = nullptr);
    ~ThermalImage();
    const float& getMin() const;
    const float& getMax() const;
    unsigned getResX() const;
    unsigned getResY() const;
    void addPixel(int x, int y, float value);
    bool addBlock(int startX, int startY, int sizeX, int sizeY, QVector<QVector<float>>& vals);
    QImage *getImg() const;
    const float& getTemp(int x, int y);

signals:
    void newMinMax();

public slots:
    void updateGradient();
};

#endif // THERMALIMAGE_H
