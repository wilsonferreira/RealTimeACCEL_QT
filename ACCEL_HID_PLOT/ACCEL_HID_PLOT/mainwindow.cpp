#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "hidapi.h"
#include "string.h"
#include "stdlib.h"
#include "limits.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect( ui->find_devices_pushbutton, SIGNAL( clicked() ), this, SLOT(find_devices()));
    connect( ui->connect_pushButton, SIGNAL( clicked() ), this, SLOT(connect_devices()));
    connect( ui->disconnect_pushButton, SIGNAL( clicked() ), this, SLOT(disconnect_device()));
    connect( ui->getdata_pushButton, SIGNAL( clicked() ), this, SLOT(get_data()));

}

MainWindow::~MainWindow()
{
    delete ui;
    //hid_device *connected_device;
    connected_device = NULL;
    on_timer_to_get_data = false;
    //devices = NULL;
    if (connected_device)
        hid_close(connected_device);


}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, the realtime data demo needs functions that are available with Qt 4.7 to work properly");
#endif
  demoName = "Real Time Data Demo";

  // include this section to fully disable antialiasing for higher performance:

  customPlot->setNotAntialiasedElements(QCP::aeAll);
  QFont font;
  font.setStyleStrategy(QFont::NoAntialias);
  customPlot->xAxis->setTickLabelFont(font);
  customPlot->yAxis->setTickLabelFont(font);
  customPlot->legend->setFont(font);

  customPlot->addGraph(); // blue line
  customPlot->graph(0)->setPen(QPen(Qt::blue));
  //customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
  //customPlot->graph(0)->setAntialiasedFill(false);
  customPlot->addGraph(); // red line
  customPlot->graph(1)->setPen(QPen(Qt::red));
  //customPlot->graph(0)->setChannelFillGraph(customPlot->graph(1));





  customPlot->addGraph(); // blue dot
  customPlot->graph(2)->setPen(QPen(Qt::blue));
  customPlot->graph(2)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
  customPlot->addGraph(); // red dot
  customPlot->graph(3)->setPen(QPen(Qt::red));
  customPlot->graph(3)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);

  customPlot->addGraph(); // green line
  customPlot->graph(4)->setPen(QPen(Qt::green));
  customPlot->addGraph(); // red dot
  customPlot->graph(5)->setPen(QPen(Qt::green));
  customPlot->graph(5)->setLineStyle(QCPGraph::lsNone);
  customPlot->graph(5)->setScatterStyle(QCPScatterStyle::ssDisc);

  customPlot->addGraph(); // green line
  customPlot->graph(6)->setPen(QPen(Qt::black));
  customPlot->addGraph(); // green line
  customPlot->graph(7)->setPen(QPen(Qt::black));


  customPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
  customPlot->xAxis->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setAutoTickStep(false);
  customPlot->xAxis->setTickStep(2);
  customPlot->axisRect()->setupFullAxesBox();

  // make left and bottom axes transfer their ranges to right and top axes:
  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));

  // setup a timer that repeatedly calls MainWindow::realtimeDataSlot:
  //connect(&dataTimer, SIGNAL(timeout()), this, SLOT(realtimeDataSlot()));
  //dataTimer.start(0); // Interval 0 means to refresh as fast as possible
}
void MainWindow::realtimeDataSlot()
{
  // calculate two new data points:
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  double key = 0;
#else
  double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
#endif
  static double lastPointKey = 0;
//  double value0 = 0;
//  double value1 = 0;
  if (key-lastPointKey > 0.01) // at most add point every 10 ms
  {
      if (res == 6)
      {
          value0 = (int)buf[0] << 8 | (int)buf[1];
          value1 = (int)buf[2] << 8 | (int)buf[3];
          value2 = (int)buf[4] << 8 | (int)buf[5];
      }
      if (value0 > 32768)
          value0 = value0 - 65535;
      if (value1 > 32768)
          value1 = value1 - 65535;
      if (value2 > 32768)
          value2 = value2 - 65535;
    //double value0 = qSin(key); //qSin(key*1.6+qCos(key*1.7)*2)*10 + qSin(key*1.2+0.56)*20 + 26;
    //double value1 = qCos(key); //qSin(key*1.3+qCos(key*1.2)*1.2)*7 + qSin(key*0.9+0.26)*24 + 26;
    // add data to lines:
    ui->customPlot->graph(0)->addData(key, value0);
    ui->customPlot->graph(1)->addData(key, value1);
    ui->customPlot->graph(4)->addData(key, value2);
    ui->customPlot->graph(6)->addData(key, 250);
    ui->customPlot->graph(7)->addData(key, -250);
    // set data of dots:
    //ui->customPlot->graph(2)->clearData();
    //ui->customPlot->graph(2)->addData(key, value0);
    //ui->customPlot->graph(3)->clearData();
    //ui->customPlot->graph(3)->addData(key, value1);
    //ui->customPlot->graph(5)->clearData();
    //ui->customPlot->graph(5)->addData(key, value2);
    // remove data of lines that's outside visible range:
    ui->customPlot->graph(0)->removeDataBefore(key-8);
    ui->customPlot->graph(1)->removeDataBefore(key-8);
    ui->customPlot->graph(4)->removeDataBefore(key-8);
    ui->customPlot->graph(6)->removeDataBefore(key-8);
    ui->customPlot->graph(7)->removeDataBefore(key-8);
    // rescale value (vertical) axis to fit the current data:
    ui->customPlot->graph(0)->rescaleValueAxis();
    ui->customPlot->graph(1)->rescaleValueAxis(true);
    ui->customPlot->graph(4)->rescaleValueAxis(true);
    ui->customPlot->graph(6)->rescaleValueAxis(true);
    ui->customPlot->graph(7)->rescaleValueAxis(true);
    lastPointKey = key;
  }
  // make key axis range scroll with the data (at a constant range size of 8):
  ui->customPlot->xAxis->setRange(key+0.25, 8, Qt::AlignRight);
  ui->customPlot->replot();

  // calculate frames per second:
  static double lastFpsKey;
  static int frameCount;
  ++frameCount;
  if (key-lastFpsKey > 2) // average fps over 2 seconds
  {
    ui->statusBar->showMessage(
          QString("%1 FPS, Total Data points: %2")
          .arg(frameCount/(key-lastFpsKey), 0, 'f', 0)
          .arg(ui->customPlot->graph(0)->data()->count()+ui->customPlot->graph(1)->data()->count()+ui->customPlot->graph(4)->data()->count())
          , 0);
    lastFpsKey = key;
    frameCount = 0;
  }
}

void MainWindow::get_data()
{
    qDebug() << "get_Data started";
//    unsigned char buf[256];
//    int res;
    if(connected_device != NULL)
        res = hid_read(connected_device, buf, sizeof(buf));
    if (res > 0) {
        QString s;
        s = QString("Received %1 bytes:\n").arg(res,4,10);
        for (int i = 0; i < res; i++) {
            QString t;
            t = QString("%1 ").arg(buf[i],2,16,QLatin1Char( '0' ));
            //qDebug() << (int16_t)buf[i];
            s += t;
            if ((i+1) % 4 == 0)
                s += " ";
            if ((i+1) % 16 == 0)
                s += "\n";
        }
        s += "\n";
        ui->received_data_textBrowser->append(s);
        //input_text->setBottomLine(INT_MAX);
    }
    if (res < 0) {
        ui->received_data_textBrowser->append("hid_read() returned error\n");
        //input_text->appendText("hid_read() returned error\n");
        //input_text->setBottomLine(INT_MAX);
    }
    realtimeDataSlot();
    if (on_timer_to_get_data)
        QTimer::singleShot(5, this, SLOT(get_data()));
}

void MainWindow::disconnect_device()
{
    qDebug() << "connected device" << connected_device;
    hid_close(connected_device);
    connected_device = NULL;
    qDebug() << "connected device" << connected_device;
    on_timer_to_get_data = false;
}

void MainWindow::connect_devices()
{
    //devices = NULL;
    connected_device = NULL;
    if (connected_device)
        hid_close(connected_device);
    /* //Working
    QString cur_item;
    //devices->interface_number;
    cur_item = devices->path;
    qDebug() << cur_item;
    QByteArray ba = cur_item.toLocal8Bit();
    const char *c_str2 = ba.data();
    qDebug() << c_str2;
    connected_device = hid_open_path("/dev/hidraw0");
    //connected_device = hid_open(4660,6,NULL);
    //connected_device = hid_open_path();
    if(connected_device)
        qDebug() << "Device Connected";
    else qDebug() << "not connected";

    //qDebug() << connected_device;
    //hid_set_nonblocking(connected_device, 1);
    if (connected_device != NULL)
        hid_close(connected_device);
    */

    //int cur_item = ui->find_devices_listWidget->currentItem();
    //int cur_item = ui->find_devices_listWidget->currentIndex();
    //qDebug() << cur_item;
    //QListWidgetItem *item = ui->find_devices_listWidget->item(cur_item);
    QListWidgetItem *item = ui->find_devices_listWidget->currentItem();
    qDebug()<<"item"<<item->text();
    QRegularExpression re("(/dev/hidraw\\d{1,3})");
    QRegularExpressionMatch match = re.match(item->text());
    QString match_string;
    const char *device_path;
    QByteArray ba;

    if (match.hasMatch()) {
        match_string = match.captured(1);
        ba = match_string.toLocal8Bit();
        device_path = ba.data();
    }
    else return;
    qDebug( )<< "device_path" << device_path;
    //struct hid_device_info *device_info = (struct hid_device_info*) item->clone();
    //QString cur_item;
    //cur_item = device_info->path;
    //qDebug() << cur_item;
    //QByteArray ba = cur_item.toLocal8Bit();
    //const char *c_str2 = ba.data();
    //qDebug() << c_str2;
    connected_device = hid_open_path(device_path);
    //connected_device = hid_open_path("/dev/hidraw4");
    //qDebug() << "path" << device_info->path ;
    //connected_device = hid_open_path(device_info->path);
    if (connected_device)
    {
        qDebug() << "connected";
        on_timer_to_get_data = true;
    }
    else
    {
        qDebug() << "not connected";
        on_timer_to_get_data = false;
    }
    qDebug() << connected_device;
    hid_set_nonblocking(connected_device, 1);

    //QThread::sleep(1000);
    //if (connected_device != NULL)
    //    hid_close(connected_device);
    //qDebug() << connected_device;
    if (on_timer_to_get_data)
        QTimer::singleShot(5, this, SLOT(get_data()));
    setupRealtimeDataDemo(ui->customPlot);
}

void MainWindow::find_devices()
{
    struct hid_device_info *cur_dev;


    ui->find_devices_listWidget->clear();
    devices = NULL;
    cur_dev = NULL;

    // List the Devices
    hid_free_enumeration(devices);
    //hid_device *device;
    //device = hid_open(0x1234, 0x0006, NULL);
    //qDebug() << "Device" << device;
    devices = hid_enumerate(0x0, 0x0);
    qDebug() << "Devices" << devices;
    cur_dev = devices;
    qDebug() << "cur_dev" << cur_dev;
    while (cur_dev) {
        // Add it to the List Box.
        QString s;
        //QString vendorid;
        //QString productid;
        QString usage_str;
        //s = QString("%04hx:%04hx").arg(cur_dev->vendor_id, cur_dev->product_id);
        //s = QString("%1 : %2").arg(cur_dev->vendor_id, cur_dev->product_id);
        //s = QString("%1:%2").arg("vendor", "product_id");
        //vendorid = cur_dev->vendor_id;
        //productid = cur_dev->product_id;
        //vendorid.setNum(cur_dev->vendor_id,16);
        //productid.setNum(cur_dev->product_id, 16);
        s = QString("%1:%2").arg(cur_dev->vendor_id,4,16,QLatin1Char( '0' )).arg(cur_dev->product_id,4,16,QLatin1Char( '0' ));
        //QString manufacturer_String;
        //if(cur_dev->manufacturer_string != NULL)
        //    manufacturer_String = QString::fromStdWString(std::wstring(cur_dev->manufacturer_string));
        //else manufacturer_String =" ";
        //qDebug() << manufacturer_String;
        if(cur_dev->manufacturer_string != NULL)
            s += QString(" ") + QString::fromStdWString(std::wstring(cur_dev->manufacturer_string));
        if(cur_dev->product_string != NULL)
            s += QString(" ") + QString::fromStdWString(std::wstring(cur_dev->product_string));
        else s +="  ";
        usage_str = QString("(usage: %1:%2)").arg(cur_dev->usage_page,4,10,QLatin1Char( '0' )).arg(cur_dev->usage,4,10,QLatin1Char( '0' ));
        //usage_str = QString(" ") + QString("(usage: %1:").arg(cur_dev->usage_page,4,16,QLatin1Char( '0' ));
        s+= QString(" ") + usage_str;
        QString cur_item;
        cur_item = cur_dev->path;
        //qDebug() << cur_item;
        QByteArray ba = cur_item.toLocal8Bit();
        const char *c_str2 = ba.data();
        qDebug() << c_str2;
        s += QString(" ") + c_str2;
        //qDebug() << "usage_str" << usage_str;
        //s += QString(" ") + QString::fromStdWString(std::wstring(cur_dev->manufacturer_string));
        //QString str;
        //str = QString("Decimal 63 is %1 in hexadecimal").arg(63, 4, 16, QLatin1Char( '0' ));
        //qDebug() << "str" << str;
        //qDebug() << " string s" << s;
        //qDebug() << "vendor" << cur_dev->vendor_id;
        //qDebug() << "Produc_id" << cur_dev->product_id;
        //qDebug() << "vendor" << vendorid;
        //qDebug() << "Produc_id" << productid;
        qDebug() << "s" << s;
       // s += QString(" ") + cur_dev->manufacturer_string;
       // s += QString(" ") + cur_dev->product_string;
       // qDebug() << cur_dev->manufacturer_string;
       // qDebug() << cur_dev->product_string;
       // qDebug() << cur_dev->usage_page;
       // qDebug() << cur_dev->usage;
       // qDebug() << "s" << s;


        //s.format("%04hx:%04hx -", cur_dev->vendor_id, cur_dev->product_id);
        //s += QTString(" ") + cur_dev->manufacturer_string;
        //s += QTString(" ") + cur_dev->product_string;
        //usage_str.format(" (usage: %04hx:%04hx) ", cur_dev->usage_page, cur_dev->usage);
        //s += usage_str;
        //QLis
        //QTListItem *li = new QTListItem(s, NULL, cur_dev);

        //device_list->appendItem(li);
        //QListWidgetItem *li;
        //li->setText(s);
        //li->
        ui->find_devices_listWidget->addItem(s);
        //ui->find_devices_listWidget->addItem(li);
        cur_dev = cur_dev->next;
        //QThread::sleep(1);
    }


   if (ui->find_devices_listWidget->count() == 0)
      ui->find_devices_listWidget->addItem("*** No Devices Connected ***");
   else {
        ui->find_devices_listWidget->setCurrentItem(0);
   }

    //return 1;

    /*
     * int device_right = 0;
    int device_left = 1;
    captureThread1 = new CaptureThread(device_right);
    captureThread2 = new CaptureThread(device_left);
    connect(captureThread1, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame_right(QImage)));
    connect(captureThread2, SIGNAL(newFrame(QImage)), this, SLOT(updateFrame_left(QImage)));
    captureThread1->start();
    captureThread2->start();
    */
}
