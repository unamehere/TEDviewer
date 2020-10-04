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
            if(!fixedMinMax)
            {
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
                if(x == static_cast<int>(resX)-1 && y == static_cast<int>(resY)-1) //when last pixel is added
                {
                    findMaxTemp();
                    findMinTemp();
                    emit newMinMax();
                }
            }
            int cindex = getColorIndex(value);
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

const QImage *ThermalImage::getImg() const
{
    return img;
}

const float &ThermalImage::getTemp(int x, int y)
{
    return tempMap->data()[x][y];
}

void ThermalImage::fillGradientImg()
{
    int count = colorGradient.size();
    this->gradientImg = new QImage(4,count, QImage::Format_RGB16);
    for (int i = 0; i< count; i++)
    {
      gradientImg->setPixelColor(0,i,colorGradient.at(count-1-i));
      gradientImg->setPixelColor(1,i,colorGradient.at(count-1-i));
      gradientImg->setPixelColor(2,i,colorGradient.at(count-1-i));
      gradientImg->setPixelColor(3,i,colorGradient.at(count-1-i));
    }
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
                int cindex = getColorIndex(value);
                QColor c = colorGradient.at(cindex);
                img->setPixelColor(x,resY-1-y,c); //Invert Y from Image
            }
        }
    }

}

void ThermalImage::handleImageComplete()
{
    findMinTemp();
    findMaxTemp();
    updateGradient();
}

void ThermalImage::handle_newFixedTemps(float min, float max)
{
    if(fixedMinMax == true)
    {
        this->minT = min;
        this->maxT = max;
        updateGradient();
    }
}

const QImage *ThermalImage::getGradientImg() const
{
    return gradientImg;
}

void ThermalImage::setFixedMinMax(bool value)
{
    fixedMinMax = value;
}

int ThermalImage::getColorIndex(const float &val)
{
    int perc = 0;
    if(abs(maxT - minT) > 0.01)
        perc = static_cast<int>(1/(maxT-minT)*(val-minT)*(colorGradient.size()-1));
    if(perc < 0)
        perc = 0;
    if(perc >= colorGradient.size())
        perc = colorGradient.size()-1;
    return perc;
}

void ThermalImage::fillGradient()
{
    for (int i = 125;i <255; i+=2) //from darkblue to blue
    {
        QColor c;
        c.setRgb(0,0,i);
        colorGradient.append(c);
    }
    for(int i = 0; i <= 255; i++) //from blue to red
    {
        QColor c;
        c.setRgb(i,0,255-i);
        colorGradient.append(c);
    }
    for (int i = 0; i< 255; i+=2) //from red to yellow
    {
        QColor c;
        c.setRgb(255,i,0);
        colorGradient.append(c);
    }
}

void ThermalImage::findMinTemp()
{
    float minTemp = getTemp(0,0);
    QPoint mtempP;
    for (int y = 0; y < static_cast<int>(resY); y++)
    {
        for (int x = 0; x < static_cast<int>(resX); x++)
        {
            QColor nowC = img->pixelColor(x,resY-1-y);
            if(nowC != Qt::black)
            {
                float temp = getTemp(x,y);
                if(temp < minTemp)
                {
                    minTemp = temp;
                    mtempP.setX(x);
                    mtempP.setY(y);
                }
            }
        }
    }
    minT = minTemp;
    minP = mtempP;
}

void ThermalImage::findMaxTemp()
{
    float maxTemp = getTemp(0,0);
    QPoint maxtempP;
    for (int y = 0; y < static_cast<int>(resY); y++)
    {
        for (int x = 0; x < static_cast<int>(resX); x++)
        {
            QColor nowC = img->pixelColor(x,resY-1-y);
            if(nowC != Qt::black)
            {
                float temp = getTemp(x,y);
                if(temp > maxTemp)
                {
                    maxTemp = temp;
                    maxtempP.setX(x);
                    maxtempP.setY(y);
                }
            }
        }
    }
    maxT = maxTemp;
    maxP = maxtempP;
}

ThermalImage::ThermalImage(unsigned resX_, unsigned resY_, QWidget *parent):resX(resX_), resY(resY_), QWidget(parent)
{
    fixedMinMax = false;
    img = new QImage(static_cast<int>(resX_),static_cast<int>(resY_),QImage::Format_RGB16);
    img->fill(Qt::black);
    tempMap = new QVector<QVector<float>> (static_cast<int>(resX_),QVector<float>(static_cast<int>(resY_)));
    connect(this,SIGNAL(newMinMax()), this, SLOT(updateGradient()));
    connect(this,SIGNAL(imageComplete()), this, SLOT(handleImageComplete()));
    fillGradient();
    fillGradientImg();
}

ThermalImage::~ThermalImage()
{
    delete img;
    img = nullptr;
    delete gradientImg;
    gradientImg = nullptr;
    delete tempMap;
    tempMap = nullptr;
}
