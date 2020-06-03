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
private:
    ThermalImage* tImg;
    thermalComHandler* tCoHa;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    backroundworker bworker;

public slots:
    void on_com_refresh_clicked();
};
#endif // MAINWINDOW_H
