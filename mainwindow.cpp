#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->com_Comports->addItems(bworker.GetComPorts());
    tCoHa = new thermalComHandler(this);
    ThermalImage* tImg = tCoHa->getTImgP();
    this->setWindowTitle("TED Viewer");

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

    QLabel *gradientLabel = ui->control_gradient_PH;
    QPixmap gradient = QPixmap::fromImage(*tImg->getGradientImg());
    gradientLabel->setPixmap(gradient.scaled(75,300));
    handle_newMinMaxT();

    //connections
    connect(this, SIGNAL(startStopS(bool,QString)), tCoHa, SLOT(handleStartStopSignal(bool,QString)));
    connect(tCoHa, SIGNAL(newImageData()),this, SLOT(handle_NewImageData()));
    connect(tCoHa, SIGNAL(newMinMax()), this, SLOT(handle_newMinMaxT()));
    connect(tCoHa, SIGNAL(connectionError(QString)), this, SLOT(handle_connectionError(QString)));
    connect(ui->control_fixTempCB, SIGNAL(stateChanged(int)),this, SLOT(handle_fixedTempCB_changed(int)));
    connect(ui->control_fastMode, SIGNAL(stateChanged(int)),this, SLOT(handle_fastMode_changed(int)));
    handle_resizeImgLabel();

    connect(&bworker, SIGNAL(ErrorMessageFF(QString)), ui->errorMessage, SLOT(setText(QString)));
    connect(&bworker,SIGNAL(connectedPort()), this, SLOT(handle_com_opened()));
    connect(&bworker, SIGNAL(closedPort()), this, SLOT(handle_com_closed()));

    connect(&socket, SIGNAL(errorMsg(QString)), ui->errorMessage, SLOT(setText(QString)));
    connect(&socket,SIGNAL(socketConnected()), this, SLOT(handle_ip_opened()));
    connect(&socket, SIGNAL(socketClosed()), this, SLOT(handle_ip_closed()));
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    handle_resizeImgLabel();
}

void MainWindow::writeSettings()
{
    QSettings settings(m_settings);
    QString ip = ui->ip_ip->text();
    QString port = ui->ip_port->text();
    settings.beginGroup("mainwindow");
    settings.setValue("ip",ip);
    settings.setValue("port",port);
    settings.endGroup(); //mainwindow
}

void MainWindow::loadSettings()
{
    QSettings settings(m_settings);
    settings.beginGroup("mainwindow");
    QString ip = settings.value("ip","").toString();
    QString port = settings.value("port","").toString();
    settings.endGroup(); //mainwindow
    ui->ip_ip->setText(ip);
    ui->ip_port->setText(port);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
}

void MainWindow::on_com_refresh_clicked()
{
    ui->com_Comports->clear();
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

void MainWindow::on_ip_connect_clicked()
{
    bool connected = socket.getCon_state();
    if(connected == false)
    {
        QString ip = ui->ip_ip->text();
        QString port = ui->ip_port->text();
        socket.connectToServer(true, ip, port);
    }
    else
    {
        socket.connectToServer(false);
    }
}

void MainWindow::on_control_saveImg_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "home/timg.png",
                               tr("Images (*.png *.xpm *.jpg)"));
    QImage img_scaled = tCoHa->getTImgP()->getImg()->scaledToHeight(1000);
    img_scaled.save(fileName);
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
    connect(&bworker,SIGNAL(newRXMessageComS(QByteArray)), tCoHa,SLOT(handleCommandCallback(QByteArray)));
    connect(tCoHa, SIGNAL(sendCommandMessage(QByteArray)), &bworker,SIGNAL(sendMessageCom(QByteArray)));
    bworker.setConnected(true);
    ui->com_connect->setText("Disconnect");
    ui->control_start->setEnabled(true);
}

void MainWindow::handle_com_closed()
{
    disconnect(&bworker,SIGNAL(newRXMessageComS(QByteArray)), tCoHa,SLOT(handleCommandCallback(QByteArray)));
    disconnect(tCoHa, SIGNAL(sendCommandMessage(QByteArray)), &bworker,SIGNAL(sendMessageCom(QByteArray)));
    bworker.setConnected(false);
    ui->com_connect->setText("Connect");
    ui->control_start->setEnabled(false);
}

void MainWindow::handle_ip_opened()
{
    connect(&socket,SIGNAL(data_received(QByteArray)), tCoHa,SLOT(handleCommandCallback(QByteArray)));
    connect(tCoHa, SIGNAL(sendCommandMessage(QByteArray)), &socket,SLOT(handle_sendData(QByteArray)));
    ui->com_connect->setEnabled(false);
    ui->ip_connect->setText("Disconnect");
    ui->control_start->setEnabled(true);
}

void MainWindow::handle_ip_closed()
{
    disconnect(&socket,SIGNAL(data_received(QByteArray)), tCoHa,SLOT(handleCommandCallback(QByteArray)));
    disconnect(tCoHa, SIGNAL(sendCommandMessage(QByteArray)), &socket,SLOT(handle_sendData(QByteArray)));
    ui->com_connect->setEnabled(true);
    ui->ip_connect->setText("Connect");
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

void MainWindow::handle_newMinMaxT()
{
    ThermalImage* tImg = tCoHa->getTImgP();
    QString minTstr = QString::number(tImg->getMin());
    QString maxTstr = QString::number(tImg->getMax());
    ui->control_minTempLE->setText(minTstr);
    ui->control_maxTempLE->setText(maxTstr);
}

void MainWindow::handle_fixedTempCB_changed(int state)
{
    if(state == Qt::Unchecked)
    {
        tCoHa->getTImgP()->setFixedMinMax(false);
        ui->control_maxTempLE->setReadOnly(true);
        ui->control_minTempLE->setReadOnly(true);
        disconnect(this, SIGNAL(newMinMaxTemps(int,int)),tCoHa->getTImgP(),SLOT(handle_newFixedTemps(int, int)));
        disconnect(ui->control_minTempLE, SIGNAL(textChanged(QString)), this, SLOT(handle_newFixedTemp()));
        disconnect(ui->control_maxTempLE, SIGNAL(textChanged(QString)), this, SLOT(handle_newFixedTemp()));
    }
    else if(state == Qt::Checked)
    {
        tCoHa->getTImgP()->setFixedMinMax(true);
        ui->control_maxTempLE->setReadOnly(false);
        ui->control_minTempLE->setReadOnly(false);
        connect(this, SIGNAL(newMinMaxTemps(float,float)),tCoHa->getTImgP(),SLOT(handle_newFixedTemps(float, float)));
        connect(ui->control_minTempLE, SIGNAL(editingFinished()), this, SLOT(handle_newFixedTemp()));
        connect(ui->control_maxTempLE, SIGNAL(editingFinished()), this, SLOT(handle_newFixedTemp()));
    }

}

void MainWindow::handle_fastMode_changed(int state)
{
    tCoHa->setFastMode(static_cast<bool>(state));
}

void MainWindow::handle_newFixedTemp()
{
    float max = ui->control_maxTempLE->text().toFloat();
    float min = ui->control_minTempLE->text().toFloat();
    emit newMinMaxTemps(min,max);
}

void MainWindow::handle_connectionError(QString msg)
{
    ui->errorMessage->setText(msg);
    ui->control_start->setText("Start");
}
