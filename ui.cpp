#include "pandaxiii.h"
#include "ui_pandaxiii.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    setFixedSize(size());
    ui->canvas->setScaledContents(true);
    ui->baselinecanvas->setScaledContents(true);
    ui->samplerate->addItem(QString::fromUtf8("50MHz"), 0);
    ui->samplerate->addItem(QString::fromUtf8("25MHz"), 1);
    ui->samplerate->addItem(QString::fromUtf8("12.5MHz"), 3);
    ui->samplerate->addItem(QString::fromUtf8("10MHz"), 4);
    ui->samplerate->addItem(QString::fromUtf8("5MHz"), 9);
    ui->samplerate->addItem(QString::fromUtf8("3.75MHz"), 15);
    ui->vicm->addItem(QString::fromUtf8("1.25V"), 0);
    ui->vicm->addItem(QString::fromUtf8("1.35V"), 1);
    ui->vicm->addItem(QString::fromUtf8("1.55V"), 2);
    ui->vicm->addItem(QString::fromUtf8("1.65V"), 3);
    ui->gaincsa->addItem(QString::fromUtf8("120fc"), 0);
    ui->gaincsa->addItem(QString::fromUtf8("240fc"), 1);
    ui->gaincsa->addItem(QString::fromUtf8("1pc"), 2);
    ui->gaincsa->addItem(QString::fromUtf8("10pc"), 3);
    ui->agetthres->addItem(QString::fromUtf8("000"), 0);
    ui->agetthres->addItem(QString::fromUtf8("2%"), 1);
    ui->agetthres->addItem(QString::fromUtf8("4%"), 2);
    ui->agetthres->addItem(QString::fromUtf8("5.5%"), 3);
    ui->agetthres->addItem(QString::fromUtf8("7.5%(中间值)"), 4);
    ui->agetthres->addItem(QString::fromUtf8("9%"), 5);
    ui->agetthres->addItem(QString::fromUtf8("11%"), 6);
    ui->agetthres->addItem(QString::fromUtf8("12%(最大值)"), 7);
    ui->testcap->addItem(QString::fromUtf8("120fF"), 0);
    ui->testcap->addItem(QString::fromUtf8("240fF"), 1);
    ui->testcap->addItem(QString::fromUtf8("1pF"), 2);
    ui->testcap->addItem(QString::fromUtf8("10pF"), 3);
    ui->modesel->addItem(QString::fromUtf8("nomal"), 0);
    ui->modesel->addItem(QString::fromUtf8("calibration"), 1);
    ui->modesel->addItem(QString::fromUtf8("test"), 2);
    ui->modesel->addItem(QString::fromUtf8("function"), 3);
    ui->scachannel->addItem(QString::fromUtf8("1-34"), 5);
    ui->scachannel->addItem(QString::fromUtf8("ch1"), 6);
    ui->scachannel->addItem(QString::fromUtf8("1-68"), 7);
    ui->chthres->addItem(QString::fromUtf8("0000"), 0);
    ui->chthres->addItem(QString::fromUtf8("0001"), 1);
    ui->chthres->addItem(QString::fromUtf8("0010"), 2);
    ui->chthres->addItem(QString::fromUtf8("0011"), 3);
    ui->chthres->addItem(QString::fromUtf8("0100"), 4);
    ui->chthres->addItem(QString::fromUtf8("0101"), 5);
    ui->chthres->addItem(QString::fromUtf8("0110"), 6);
    ui->chthres->addItem(QString::fromUtf8("0111"), 7);
    ui->chthres->addItem(QString::fromUtf8("1000(中间值)"), 8);
    ui->chthres->addItem(QString::fromUtf8("1001"), 9);
    ui->chthres->addItem(QString::fromUtf8("1010"), 10);
    ui->chthres->addItem(QString::fromUtf8("1011"), 11);
    ui->chthres->addItem(QString::fromUtf8("1100"), 12);
    ui->chthres->addItem(QString::fromUtf8("1101"), 13);
    ui->chthres->addItem(QString::fromUtf8("1110"), 14);
    ui->chthres->addItem(QString::fromUtf8("1111(最大值)"), 15);
    ui->trigselec->addItem(QString::fromUtf8("Self Trig"), 0);
    ui->trigselec->addItem(QString::fromUtf8("Ext Trig"), 1);
    ui->trigselec->addItem(QString::fromUtf8("Hit Trig"), 3);
    ui->trigdelay->addItem(QString::fromUtf8("7.5us"), 3);
    ui->trigdelay->addItem(QString::fromUtf8("10us"), 4);
    ui->trigdelay->addItem(QString::fromUtf8("15us"), 6);
    ui->trigdelay->addItem(QString::fromUtf8("19us"), 7);
    ui->trigdelay->addItem(QString::fromUtf8("20us"), 8);
    ui->trigdelay->addItem(QString::fromUtf8("25us"), 10);
    ui->trigdelay->addItem(QString::fromUtf8("40us"), 16);
    ui->trigdelay->addItem(QString::fromUtf8("50us"), 20);
    ui->trigdelay->addItem(QString::fromUtf8("60us"), 24);
    ui->trigdelay->addItem(QString::fromUtf8("80us"), 32);
    ui->dacthres->addItem(QString::fromUtf8("0"), 0);
    ui->dacthres->addItem(QString::fromUtf8("1V"), 5);
    ui->chip->addItem(QString::fromUtf8("Chip a"), 1);
    ui->chip->addItem(QString::fromUtf8("Chip b"), 2);
    ui->chip->addItem(QString::fromUtf8("Chip c"), 3);
    ui->chip->addItem(QString::fromUtf8("Chip d"), 4);
    filename = "data.dat";
    QLabel *label; // memory
    for(qint32 i = 0; i <= 15; i++) {
        label = new QLabel(ui->wavedisplay);
        label->setGeometry(QRect(60 + 670 * i / 15, 510, 30, 20));
        label->setText(QString().sprintf("%d", i * 511 / 15));
    }
    for(qint32 i = 0; i <= 8; i++) {
        label = new QLabel(ui->wavedisplay);
        label->setGeometry(QRect(20, 490 - 350 * i / 8, 30, 20));
        label->setText(QString().sprintf("%4d", i * 4095 / 8));
    }
    for(qint32 i = 0; i <= 15; i++) {
        label = new QLabel(ui->baselinetest);
        label->setGeometry(QRect(60 + 670 * i / 15, 510, 30, 20));
        label->setText(QString().sprintf("%d", i * 63 / 15));
    }
    for(qint32 i = 0; i <= 8; i++) {
        label = new QLabel(ui->baselinetest);
        label->setGeometry(QRect(20, 490 - 350 * i / 8, 30, 20));
        label->setText(QString().sprintf("%4d", i * 4095 / 8));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QHostAddress MainWindow::ipaddr()
{
    return QHostAddress(ui->ipaddr->text());
}

quint16 MainWindow::rbcp_port()
{
    return ui->rbcpport->text().toUInt(0, 0);
}

quint32 MainWindow::rbcp_address()
{
    return ui->address->text().toUInt(0, 0);
}

QString MainWindow::rbcp_data()
{
    return ui->data->text();
}

quint8 MainWindow::rbcp_length()
{
    return ui->length->text().toUInt(0, 0);
}

quint8 MainWindow::rbcp_samplerate()
{
    return ui->samplerate->itemData(ui->samplerate->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_vicm()
{
    return ui->vicm->itemData(ui->vicm->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_gaincsa()
{
    return ui->gaincsa->itemData(ui->gaincsa->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_agetthres()
{
    return ui->agetthres->itemData(ui->agetthres->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_testcap()
{
    return ui->testcap->itemData(ui->testcap->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_modesel()
{
    return ui->modesel->itemData(ui->modesel->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_scachannel()
{
    return ui->scachannel->itemData(ui->scachannel->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_chthres()
{
    return ui->chthres->itemData(ui->chthres->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_trigselec()
{
    return ui->trigselec->itemData(ui->trigselec->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_trigdelay()
{
    return ui->trigdelay->itemData(ui->trigdelay->currentIndex()).toUInt();
}

quint8 MainWindow::rbcp_dacthres()
{
    return ui->dacthres->itemData(ui->dacthres->currentIndex()).toUInt();
}

quint16 MainWindow::tcp_port()
{
    return ui->tcpport->text().toUInt(0, 0);
}

qint64 MainWindow::tcp_packet()
{
    return ui->packet->text().toUInt(0, 0);
}

qint64 MainWindow::tcp_chip()
{
    return ui->chip->itemData(ui->chip->currentIndex()).toInt();
}

qint64 MainWindow::tcp_channel()
{
    return ui->channel->text().toUInt(0, 0);
}

void MainWindow::tcp_canvas_set_picture(const QPicture &picture)
{
    ui->canvas->setPicture(picture);
}

qreal MainWindow::tcp_canvas_get_aspect_ratio()
{
    return (qreal)ui->canvas->width() / ui->canvas->height();
}

void MainWindow::baseline_canvas_set_picture(const QPicture &picture)
{
    ui->baselinecanvas->setPicture(picture);
}

void MainWindow::tcp_set_enabled(bool enabled)
{
    ui->connect->setEnabled(enabled);
}

void MainWindow::tcp_set_connected(bool connected)
{
    ui->connect->setChecked(connected);
    ui->connect->setText(connected ? "disconnect" : "connect");
}
