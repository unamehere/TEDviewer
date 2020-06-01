#include "thermalimage.h"


float ThermalImage::getMin() const
{
    return minT;
}

float ThermalImage::getMax() const
{
    return maxT;
}

unsigned ThermalImage::getResX() const
{
    return resX;
}

unsigned ThermalImage::getResY() const
{
    return resY;
}

void ThermalImage::addPixel(int x, int y, float value)
{
    if(x >= 0 && x < static_cast<int>(resX))
    {
        if(y >= 0 && y < static_cast<int>(resY))
        {
            QVector<float>* vec = tempMap->data();
            vec[x][y] = value;
            if(value > maxT)
            {
                maxT = value;
                emit newMinMax();
            }
            if(value < minT)
            {
                minT = value;
                emit newMinMax();
            }
            int cindex = getPercent(value);
            QColor c = colorGradient.at(cindex);
            img->setPixelColor(x,y,c);
        }
    }
}

QImage *ThermalImage::getImg() const
{
    return img;
}

void ThermalImage::updateGradient()
{

}

int ThermalImage::getPercent(const float &val)
{
    int perc = 0;
    if(abs(maxT - minT) > 0.01)
        perc = static_cast<int>(1/(maxT-minT)*(val-minT)*255);
    return perc;
}

void ThermalImage::fillGradient()
{
    for(int i = 0; i <= 255; i++)
    {
        QColor c;
        c.setRgb(i,0,255-i);
        colorGradient.append(c);
    }
}

ThermalImage::ThermalImage(unsigned resX_, unsigned resY_, QWidget *parent):resX(resX_), resY(resY_)
{
    img = new QImage(static_cast<int>(resX_),static_cast<int>(resY_),QImage::Format_RGB16);
    img->fill(Qt::black);
    tempMap = new QVector<QVector<float>> (static_cast<int>(resX_),QVector<float>(static_cast<int>(resY_)));
    connect(this,SIGNAL(newMinMax()), this, SLOT(updateGradient()));
    fillGradient();
}

ThermalImage::~ThermalImage()
{
    delete img;
    delete [] tempMap;
}
