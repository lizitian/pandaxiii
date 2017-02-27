#ifndef RBCP_H
#define RBCP_H
#include <QtGui>
#include <QtNetwork>
namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void tcp_show(const QString &);
    void tcp_set_enabled(bool);
    void tcp_set_connected(bool);
private slots:
    void on_write_clicked();
    void on_read_clicked();
    void on_CFigPLL_clicked();
    void on_AGET_test_clicked();
    void on_StartSCA_clicked();
    void on_TriggerEn_clicked(bool);
    void on_CFigDAC_clicked();
    void on_connect_clicked(bool);
private:
    Ui::MainWindow *ui;
    QHostAddress rbcp_ipaddr();
    quint16 rbcp_port();
    quint32 rbcp_address();
    quint8 rbcp_data();
    quint8 rbcp_length();
    quint8 rbcp_samplerate();
    quint8 rbcp_vicm();
    quint8 rbcp_gaincsa();
    quint8 rbcp_agetthres();
    quint8 rbcp_testcap();
    quint8 rbcp_modesel();
    quint8 rbcp_scachannel();
    quint8 rbcp_chthres();
    quint8 rbcp_trigselec();
    quint8 rbcp_trigdelay();
    quint8 rbcp_dacthres();
    void rbcp_show(const QString &);
    QHostAddress tcp_ipaddr();
    quint16 tcp_port();
};
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
class TcpCom : public QTcpSocket
{
    Q_OBJECT
public:
    TcpCom(MainWindow *, QObject *parent = 0);
    virtual ~TcpCom();
private slots:
    void on_connected();
    void on_disconnected();
    void on_readyRead();
private:
    static const qint64 bufsize = 1024 * 1024;
    MainWindow *window;
    QFile *file;
    QTime t;
    qint64 stat;
    quint8 *data;
};
#endif
