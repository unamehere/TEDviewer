#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralwidget->resize(600,600);
    ui->com_Comports->addItems(bworker.GetComPorts());
    tCoHa = new thermalComHandler();
    ThermalImage* tImg = tCoHa->getTImgP();

    //Signal Connections
    /*
    int BlockSizeY = 16;
    int BlockSizeX = 4;
    QVector<QVector<float>> vect;
    for(int yc = 0; yc < BlockSizeX; yc++ )
    {
        QVector<float> tvec;
        for (int xc = 0; xc< BlockSizeY; xc++)
        {
            tvec.append(0+4*xc+yc);
        }
        vect.append(tvec);
    }
    tImg->addBlock(0,0,BlockSizeX,BlockSizeY,vect);
    */

    QLabel *myLabel = ui->img_label; // sets parent of label to main window
    myLabel->setSizePolicy(QSizePolicy::Expanding,
                           QSizePolicy::Expanding);
    myLabel->setAlignment(Qt::AlignCenter);
    myLabel->setMinimumSize(400,400);
    int w = myLabel->width ();
    int h = myLabel->height ();
    QPixmap pmap = QPixmap::fromImage(*tImg->getImg());
    myLabel->setPixmap(pmap.scaled(w,h,Qt::KeepAspectRatio));

    ui->control_res->addItems(tCoHa->getResolutionList());

    //connections
    connect(this, SIGNAL(startStopS(bool,QString)), tCoHa, SLOT(handleStartStopSignal(bool,QString)));
    connect(&bworker, SIGNAL(ErrorMessageFF(QString)), ui->errorMessage, SLOT(setText(QString)));
    connect(&bworker,SIGNAL(connectedPort()), this, SLOT(handle_com_opened()));
    connect(&bworker, SIGNAL(closedPort()), this, SLOT(handle_com_closed()));
    connect(&bworker,SIGNAL(newRXMessageComS(QByteArray)), tCoHa,SLOT(handleCommandCallback(QByteArray)));
    connect(tCoHa, SIGNAL(sendCommandMessage(QByteArray)), &bworker,SIGNAL(sendMessageCom(QByteArray)));
    connect(tCoHa, SIGNAL(newImageData()),this, SLOT(handle_NewImageData()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    handle_resizeImgLabel();
}

void MainWindow::on_com_refresh_clicked()
{
    ui->com_Comports->addItems(bworker.GetComPorts());
}

void MainWindow::on_com_connect_clicked()
{
    bool connected = bworker.getConnected();
    if(!connected)
    {
        QString portName = ui->com_Comports->currentText();
        emit bworker.connectPort(portName,COM_BAUDRATE,QSerialPort::Data8,QSerialPort::NoParity,QSerialPort::OneStop);
    }
    else
    {
        emit bworker.ClosePort();
    }
}

void MainWindow::on_control_start_clicked()
{
    QString stateStr = ui->control_start->text();
    if(stateStr == "Start")
    {
        bool stat = true;
        QString res = ui->control_res->currentText();
        emit startStopS(stat,res);
        ui->control_start->setText("Stop");
    }
    else if(stateStr == "Stop")
    {
        bool stat = false;
        emit startStopS(stat,"");
        ui->control_start->setText("Start");
    }
}

void MainWindow::handle_resizeImgLabel()
{
    ThermalImage* tImg = tCoHa->getTImgP();
    QLabel *myLabel = ui->img_label; // sets parent of label to main window
    int w = myLabel->width();
    int h = myLabel->height();
    QPixmap pmap = QPixmap::fromImage(*tImg->getImg());
    myLabel->setPixmap(pmap.scaled(w,h,Qt::KeepAspectRatio));
}

void MainWindow::handle_com_opened()
{
    bworker.setConnected(true);
    ui->com_connect->setText("Disconnect");
    ui->control_start->setEnabled(true);
}

void MainWindow::handle_com_closed()
{
    bworker.setConnected(false);
    ui->com_connect->setText("Connect");
    ui->control_start->setEnabled(false);
}

void MainWindow::handle_NewImageData()
{
    QLabel *myLabel = ui->img_label; // sets parent of label to main window
    int w = myLabel->width ();
    int h = myLabel->height ();
    ThermalImage* tImg =  tCoHa->getTImgP();
    QPixmap pmap = QPixmap::fromImage(*tImg->getImg());
    myLabel->setPixmap(pmap.scaled(w,h,Qt::KeepAspectRatio));
}
