#include "rbcp.h"
#include "ui_rbcp.h"
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui = new Ui::MainWindow;
    ui->setupUi(this);
    ui->canvas->setScaledContents(true);
    ui->samplerate->addItem("50MHz", 0);
    ui->samplerate->addItem("25MHz", 1);
    ui->samplerate->addItem("12.5MHz", 3);
    ui->samplerate->addItem("10MHz", 4);
    ui->samplerate->addItem("5MHz", 9);
    ui->samplerate->addItem("3.75MHz", 15);
    ui->vicm->addItem("1.25V", 0);
    ui->vicm->addItem("1.35V", 1);
    ui->vicm->addItem("1.55V", 2);
    ui->vicm->addItem("1.65V", 3);
    ui->gaincsa->addItem("120fc", 0);
    ui->gaincsa->addItem("240fc", 1);
    ui->gaincsa->addItem("1pc", 2);
    ui->gaincsa->addItem("10pc", 3);
    ui->agetthres->addItem("000", 0);
    ui->agetthres->addItem("2%", 1);
    ui->agetthres->addItem("4%", 2);
    ui->agetthres->addItem("5.5%", 3);
    ui->agetthres->addItem("7.5%(middle value)", 4);
    ui->agetthres->addItem("9%", 5);
    ui->agetthres->addItem("11%", 6);
    ui->agetthres->addItem("12%(max value)", 7);
    ui->testcap->addItem("120fF", 0);
    ui->testcap->addItem("240fF", 1);
    ui->testcap->addItem("1pF", 2);
    ui->testcap->addItem("10pF", 3);
    ui->modesel->addItem("nomal", 0);
    ui->modesel->addItem("calibration", 1);
    ui->modesel->addItem("test", 2);
    ui->modesel->addItem("function", 3);
    ui->scachannel->addItem("1-34", 5);
    ui->scachannel->addItem("ch1", 6);
    ui->scachannel->addItem("1-68", 7);
    ui->chthres->addItem("0000", 0);
    ui->chthres->addItem("0001", 1);
    ui->chthres->addItem("0010", 2);
    ui->chthres->addItem("0011", 3);
    ui->chthres->addItem("0100", 4);
    ui->chthres->addItem("0101", 5);
    ui->chthres->addItem("0110", 6);
    ui->chthres->addItem("0111", 7);
    ui->chthres->addItem("1000(middle value)", 8);
    ui->chthres->addItem("1001", 9);
    ui->chthres->addItem("1010", 10);
    ui->chthres->addItem("1011", 11);
    ui->chthres->addItem("1100", 12);
    ui->chthres->addItem("1101", 13);
    ui->chthres->addItem("1110", 14);
    ui->chthres->addItem("1111(max value)", 15);
    ui->trigselec->addItem("Self Trig", 0);
    ui->trigselec->addItem("Ext Trig", 1);
    ui->trigselec->addItem("Hit Trig", 3);
    ui->trigdelay->addItem("7.5us", 3);
    ui->trigdelay->addItem("10us", 4);
    ui->trigdelay->addItem("15us", 6);
    ui->trigdelay->addItem("19us", 7);
    ui->trigdelay->addItem("20us", 8);
    ui->trigdelay->addItem("25us", 10);
    ui->trigdelay->addItem("40us", 16);
    ui->trigdelay->addItem("50us", 20);
    ui->trigdelay->addItem("60us", 24);
    ui->trigdelay->addItem("80us", 32);
    ui->dacthres->addItem("0", 0);
    ui->dacthres->addItem("1V", 5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QHostAddress MainWindow::rbcp_ipaddr()
{
    return QHostAddress(ui->rbcpipaddr->text());
}

quint16 MainWindow::rbcp_port()
{
    return ui->rbcpport->text().toUInt(0, 0);
}

quint32 MainWindow::rbcp_address()
{
    return ui->address->text().toUInt(0, 0);
}

quint8 MainWindow::rbcp_data()
{
    return ui->data->text().toUInt(0, 0);
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

void MainWindow::rbcp_show(const QString &text)
{
    ui->output->moveCursor(QTextCursor::End);
    ui->output->insertPlainText(text);
}

QHostAddress MainWindow::tcp_ipaddr()
{
    return QHostAddress(ui->tcpipaddr->text());
}

quint16 MainWindow::tcp_port()
{
    return ui->tcpport->text().toUInt(0, 0);
}

void MainWindow::tcp_canvas_set_picture(const QPicture &picture)
{
    ui->canvas->setPicture(picture);
}

qreal MainWindow::tcp_canvas_get_aspect_ratio()
{
    return (qreal)ui->canvas->width() / ui->canvas->height();
}

void MainWindow::tcp_show(const QString &text)
{
    ui->status->setText(text);
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
