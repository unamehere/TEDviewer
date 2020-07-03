#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "thermalimage.h"
#include "thermalcomhandler.h"
#include "sockethandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    backroundworker bworker;
    thermalComHandler* tCoHa;
    socketHandler socket;


    void resizeEvent(QResizeEvent*);

signals:
    void startStopS(bool state, QString res);
    void newMinMaxTemps(float min, float max);
public slots:
    void on_com_refresh_clicked();
    void on_com_connect_clicked();
    void on_control_start_clicked();
    void on_ip_connect_clicked();
    void on_control_saveImg_clicked();

    //handlers
    void handle_resizeImgLabel();
    void handle_com_opened();
    void handle_com_closed();
    void handle_ip_opened();
    void handle_ip_closed();
    void handle_NewImageData();
    void handle_newMinMaxT();
    void handle_fixedTempCB_changed(int state);
    void handle_newFixedTemp();
    void handle_connectionError(QString msg);
};
#endif // MAINWINDOW_H
