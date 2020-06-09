#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "thermalimage.h"
#include "thermalcomhandler.h"

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


    void resizeEvent(QResizeEvent*);

signals:
    void startStopS(bool state, QString res);
public slots:
    void on_com_refresh_clicked();
    void on_com_connect_clicked();
    void on_control_start_clicked();

    //handlers
    void handle_resizeImgLabel();
    void handle_com_opened();
    void handle_com_closed();
    void handle_NewImageData();
};
#endif // MAINWINDOW_H
