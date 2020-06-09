#include "thermalimage.h"


const float& ThermalImage::getMin() const
{
    return minT;
}

const float& ThermalImage::getMax() const
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
            img->setPixelColor(x,resY-1-y,c); //To Invert Y Axis
        }
    }
}

bool ThermalImage::addBlock(int startX, int startY, int sizeX, int sizeY, QVector<QVector<float> > &vals)
{
    bool retVal = true;
    if(startX<0 || startY<0 || startX+sizeX>resX || startY+sizeY > resY)
        retVal = false;
    if(retVal)
    {
        for (int y = 0; y < sizeY; y++)
        {
            for (int x = 0; x < sizeX; ++x)
            {
                    float val = vals.data()[x][y];
                    addPixel(startX+x,startY+y,val);
            }
        }
    }
    return retVal;
}

QImage *ThermalImage::getImg() const
{
    return img;
}

const float &ThermalImage::getTemp(int x, int y)
{
    return tempMap->data()[x][y];
}

void ThermalImage::updateGradient()
{
    for (int y = 0; y < static_cast<int>(resY); y++)
    {
        for (int x = 0; x < static_cast<int>(resX); x++)
        {
            QColor nowC = img->pixelColor(x,resY-1-y);
            if(nowC != Qt::black)
            {
                float value = getTemp(x,y);
                int cindex = getPercent(value);
                QColor c = colorGradient.at(cindex);
                img->setPixelColor(x,resY-1-y,c); //Invert Y from Image
            }
        }
    }

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
    delete tempMap;
}
