#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->com_Comports->addItems(bworker.GetComPorts());
    tImg = new ThermalImage(10,10);
    tImg->addPixel(0,0,20);
    tImg->addPixel(1,1,30);
    tImg->addPixel(2,2,40);
    tImg->addPixel(3,3,50);
    tImg->addPixel(9,9,100);
    QLabel *myLabel = ui->img_label; // sets parent of label to main window
    myLabel->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    myLabel->setAlignment(Qt::AlignCenter);
    myLabel->setMinimumSize(400,400);
    int w = myLabel->width ();
    int h = myLabel->height ();
    myLabel->setPixmap(QPixmap::fromImage(tImg->getImg()->scaledToWidth(w)));
    myLabel->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_com_refresh_clicked()
{
    ui->com_Comports->addItems(bworker.GetComPorts());
}

