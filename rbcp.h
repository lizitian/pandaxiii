#ifndef RBCP_H
#define RBCP_H
#define RBCP_VER 0xFF
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xC0
#define UDP_BUF_SIZE 2048
#include <arpa/inet.h>
struct rbcp_header {
    unsigned char type;
    unsigned char command;
    unsigned char id;
    unsigned char length;
    unsigned int address;
};
#include <QApplication>
#include <QUdpSocket>
#include <QTime>
#include "ui_rbcp.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
private slots:
    void on_write_clicked(bool checked);
    void on_read_clicked(bool checked);
private:
    Ui::MainWindow ui;
    QUdpSocket sock;
    void log(QString s);
    void rbcp_com(QString ipaddr, unsigned int port, struct rbcp_header *header, const void *data, char *buffer);
};
#endif
