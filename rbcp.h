#ifndef RBCP_H
#define RBCP_H
struct rbcp_header {
    unsigned char type;
    unsigned char command;
    unsigned char id;
    unsigned char length;
    unsigned int address;
};
#include <QTime>
#include <QtEndian>
#include <QUdpSocket>
#include "ui_rbcp.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = Q_NULLPTR);
private slots:
    void on_write_clicked();
    void on_read_clicked();
private:
    Ui::MainWindow ui;
    QUdpSocket sock;
    void log(QString s);
    void rbcp_com1(QString ipaddr, unsigned int port, struct rbcp_header *header, const void *data, char *buffer);
};
#endif
