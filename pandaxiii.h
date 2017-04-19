#ifndef RBCP_H
#define RBCP_H
#include <QtGui>
#include <QtNetwork>
namespace Ui
{
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
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
    void on_draw_clicked();
    void on_baselinebutton_clicked();
private:
    Ui::MainWindow *ui;
    QHostAddress ipaddr();
    quint16 rbcp_port();
    quint32 rbcp_address();
    QString rbcp_data();
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
    quint16 tcp_port();
    qint64 tcp_chip();
    qint64 tcp_channel();
    void tcp_canvas_set_picture(const QPicture &);
    qreal tcp_canvas_get_aspect_ratio();
    void baseline_canvas_set_picture(const QPicture &);
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
class TcpData
{
public:
    TcpData(const QString &);
    ~TcpData();
    static const quint16 datamask = 0x0fff;
    static const qint64 channels = 64, units = 512;
    bool read();
    bool get_data(qint64, qint64, quint16 *);
private:
    static const quint32 header = 0x55aa55aa, footer = 0xffffffff, gap1 = 0x8002c000, gap2 = 0x8002c000;
    static const quint16 chip1bits = 0x8000, chip2bits = 0xc000;
    quint32 options;
    quint16 chip1[channels][units];
    quint16 chip2[channels][units];
    QFile *file;
    bool read32(quint32 *);
};
#endif
