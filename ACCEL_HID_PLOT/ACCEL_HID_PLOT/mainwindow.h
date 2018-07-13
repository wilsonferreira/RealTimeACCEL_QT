#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <hidapi.h>
#include <QTimer>
#include "qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setupRealtimeDataDemo(QCustomPlot *customPlot);


public slots:
    void find_devices();
    void connect_devices();
    void disconnect_device();
    void get_data();

private:
    Ui::MainWindow *ui;
    struct hid_device_info *devices;
    hid_device *connected_device;
    bool on_timer_to_get_data;

    QString demoName;
    QTimer dataTimer;
    QCPItemTracer *itemDemoPhaseTracer;
    int currentDemoIndex;
    unsigned char buf[256];
    int res;
    double value0;
    double value1;
    double value2;

private slots:

    void realtimeDataSlot();



};

#endif // MAINWINDOW_H
