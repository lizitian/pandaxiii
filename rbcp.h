#ifndef RBCP_H
#define RBCP_H
#include <QTime>
#include <QtEndian>
#include <QUdpSocket>
#include "ui_rbcp.h"
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xc0
typedef struct {
    quint8 type;
    quint8 command;
    quint8 id;
    quint8 length;
    quint32 address;
} rbcp_header;
bool rbcp_com(const QHostAddress &, quint16, quint8, quint8, quint32, void *);
void msg_handler(QtMsgType, const char *);
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
private slots:
    void on_write_clicked();
    void on_read_clicked();
    void on_CFigPLL_clicked();
private:
    Ui::MainWindow ui;
};
#endif
