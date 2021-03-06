#ifndef PANDAXIII_H
#define PANDAXIII_H
#include <QtGui>
#include <QtNetwork>
namespace Ui
{
    class MainWindow;
}
class TcpWorker;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
private slots:
    void on_write_clicked();
    void on_read_clicked();
    void on_errortest_clicked(bool);
    void errortest_tick();
    void on_CFigPLL_clicked();
    void on_AGET_test_clicked();
    void on_StartSCA_clicked();
    void on_TriggerEn_clicked(bool);
    void on_CFigDAC_clicked();
    bool fec_configure(const quint8 *, qint64, const QString &);
    void on_connect_clicked(bool);
    void tcp_receive_data(quint8 *);
    void write_data_tick();
    void tcp_disconnected();
    void on_draw_clicked();
    void on_baselinebutton_clicked();
    void TriggerEn_set_checked(bool);
    void tcp_set_enabled(bool);
    void tcp_set_connected(bool);
    void errortest_set_checked(bool);
private:
    Ui::MainWindow *ui;
    TcpWorker *tcp_worker;
    QString filename;
    QTime *t;
    quint8 sfp_status_mask(qint64);
    quint8 fec_samplerate();
    quint8 fec_vicm();
    quint8 fec_gaincsa();
    quint8 fec_agetthres();
    quint8 fec_testcap();
    quint8 fec_modesel();
    quint8 fec_scachannel();
    quint8 fec_chthres();
    quint8 fec_trigselec();
    quint8 fec_trigdelay();
    quint8 fec_dacthres();
    QHostAddress ipaddr();
    quint16 rbcp_port();
    quint16 tcp_port();
    quint32 rbcp_address();
    quint8 rbcp_length();
    QString rbcp_data();
    qint64 tcp_packet();
    qint64 tcp_chip();
    qint64 tcp_channel();
    qint64 baseline_mode();
    void tcp_canvas_set_picture(const QPicture &);
    qreal tcp_canvas_get_aspect_ratio();
    void baseline_canvas_set_picture(const QPicture &);
signals:
    void tcp_user_disconnect();
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
bool rbcp_read(const QHostAddress &, quint16, quint8, quint32, void *);
bool rbcp_write(const QHostAddress &, quint16, quint32, quint8);
class TcpData
{
public:
    TcpData(const QString &);
    ~TcpData();
    static const quint16 datamask = 0x0fff;
    static const quint16 chipbits[4];
    static const qint64 chips = sizeof(chipbits) / sizeof(chipbits[0]), channels = 64, units = 512;
    bool read(quint16 packet);
    bool get_data(qint64, qint64, quint16 *);
    quint32 get_trigger();
private:
    static const quint16 header = 0xeeee, footer = 0xffff;
    quint32 trigger;
    quint16 data[chips][channels][units];
    QFile *file;
    bool read16(quint16 *);
};
class TcpWorker : public QThread
{
    Q_OBJECT
public:
    TcpWorker(const QHostAddress &, quint16, MainWindow *);
protected:
    virtual void run();
private:
    QHostAddress ipaddr;
    quint16 port;
    MainWindow *window;
};
class TcpCom : public QTcpSocket
{
    Q_OBJECT
public:
    TcpCom(QObject *parent = 0);
private slots:
    void on_connected();
    void on_disconnected();
    void on_readyRead();
    void user_disconnect();
private:
    quint8 *queue;
signals:
    void send_data(quint8 *);
    void ui_status(QString);
    void ui_tcp_connected(bool);
    void ui_tcp_enabled(bool);
    void quit();
};
#endif
