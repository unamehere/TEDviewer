#ifndef THERMALIMAGE_H
#define THERMALIMAGE_H

#include <QWidget>

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
    float maxT = 100;
    int getPercent(const float& val); //0->255
    void fillGradient();
public:
    explicit ThermalImage(unsigned resX_ = 1 , unsigned resY_ = 1,QWidget *parent = nullptr);
    ~ThermalImage();
    float getMin() const;
    float getMax() const;
    unsigned getResX() const;
    unsigned getResY() const;
    void addPixel(int x, int y, float value);
    void addRow(int y, QVector<float>* row);
    void addColumn(int x, QVector<float>* column);
    QImage *getImg() const;
    void getTemp(int x, int y);

signals:
    void newMinMax();

public slots:
    void updateGradient();
};

#endif // THERMALIMAGE_H
