#include "rbcp.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    qInstallMsgHandler(msg_handler);
    window.show();
    return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.samplerate->addItem("50MHz", 0);
    ui.samplerate->addItem("25MHz", 1);
    ui.samplerate->addItem("12.5MHz", 3);
    ui.samplerate->addItem("10MHz", 4);
    ui.samplerate->addItem("5MHz", 9);
    ui.samplerate->addItem("3.75MHz", 15);
    ui.vicm->addItem("1.25V", 0);
    ui.vicm->addItem("1.35V", 1);
    ui.vicm->addItem("1.55V", 2);
    ui.vicm->addItem("1.65V", 3);
    ui.gaincsa->addItem("120fc", 0);
    ui.gaincsa->addItem("240fc", 1);
    ui.gaincsa->addItem("1pc", 2);
    ui.gaincsa->addItem("10pc", 3);
    ui.agetthres->addItem("000", 0);
    ui.agetthres->addItem("2%", 1);
    ui.agetthres->addItem("4%", 2);
    ui.agetthres->addItem("5.5%", 3);
    ui.agetthres->addItem("7.5%(middle value)", 4);
    ui.agetthres->addItem("9%", 5);
    ui.agetthres->addItem("11%", 6);
    ui.agetthres->addItem("12%(max value)", 7);
    ui.testcap->addItem("120fF", 0);
    ui.testcap->addItem("240fF", 1);
    ui.testcap->addItem("1pF", 2);
    ui.testcap->addItem("10pF", 3);
    ui.modesel->addItem("nomal", 0);
    ui.modesel->addItem("calibration", 1);
    ui.modesel->addItem("test", 2);
    ui.modesel->addItem("function", 3);
    ui.scachannel->addItem("1-34", 5);
    ui.scachannel->addItem("ch1", 6);
    ui.scachannel->addItem("1-68", 7);
    ui.chthres->addItem("0000", 0);
    ui.chthres->addItem("0001", 1);
    ui.chthres->addItem("0010", 2);
    ui.chthres->addItem("0011", 3);
    ui.chthres->addItem("0100", 4);
    ui.chthres->addItem("0101", 5);
    ui.chthres->addItem("0110", 6);
    ui.chthres->addItem("0111", 7);
    ui.chthres->addItem("1000(middle value)", 8);
    ui.chthres->addItem("1001", 9);
    ui.chthres->addItem("1010", 10);
    ui.chthres->addItem("1011", 11);
    ui.chthres->addItem("1100", 12);
    ui.chthres->addItem("1101", 13);
    ui.chthres->addItem("1110", 14);
    ui.chthres->addItem("1111(max value)", 15);
    ui.trigselec->addItem("Self Trig", 0);
    ui.trigselec->addItem("Ext Trig", 1);
    ui.trigselec->addItem("Hit Trig", 3);
    ui.trigdelay->addItem("7.5us", 3);
    ui.trigdelay->addItem("10us", 4);
    ui.trigdelay->addItem("15us", 6);
    ui.trigdelay->addItem("19us", 7);
    ui.trigdelay->addItem("20us", 8);
    ui.trigdelay->addItem("25us", 10);
    ui.trigdelay->addItem("40us", 16);
    ui.trigdelay->addItem("50us", 20);
    ui.trigdelay->addItem("60us", 24);
    ui.trigdelay->addItem("80us", 32);
    ui.dacthres->addItem("0", 0);
    ui.dacthres->addItem("1V", 5);
}

#define uilog(x) do { ui.output->moveCursor(QTextCursor::End); ui.output->insertPlainText(x); } while(0)

void MainWindow::on_write_clicked()
{
    quint8 data = ui.data->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    if(rbcp_com(QHostAddress(ui.ipaddr->text()), ui.port->text().toUInt(0, 0), RBCP_CMD_WR, 1, address, &data))
        uilog("OK!\n");
    else
        uilog("Error!\n");
}

void MainWindow::on_read_clicked()
{
    quint8 length = ui.length->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    quint8 *data = new quint8[length];
    if(rbcp_com(QHostAddress(ui.ipaddr->text()), ui.port->text().toUInt(0, 0), RBCP_CMD_RD, length, address, data)) {
        uilog("Received Data:\n");
        QString buffer;
        for(quint8 i = 0; i < length; i++) {
            if(i % 8 == 0)
                buffer.sprintf("[0x%08x]:", address + i);
            else if(i % 8 == 4)
                buffer.append("- ");
            buffer.append(QString().sprintf("%02x", data[i]));
            if((++i) != length) {
                if(i % 8 == 0) {
                    buffer.append('\n');
                    uilog(buffer);
                } else
                    buffer.append(' ');
            }
        }
        buffer.append('\n');
        uilog(buffer);
    } else
        uilog("Error!\n");
    delete []data;
}

void MainWindow::on_CFigPLL_clicked()
{
    quint8 sample_rate = ui.samplerate->itemData(ui.samplerate->currentIndex()).toUInt();
    quint8 dat1 = (sample_rate & 0x0f) | ((sample_rate & 0x0f) << 4);
    quint8 data[] = {
        0x19, 0x00, 0x18, 0x06,
        0x11, 0x00, 0x18, 0x06,
        0x19, 0x02, 0x32, 0x01,
        0x11, 0x02, 0x32, 0x01,
        0x19, 0x00, 0x10, 0x7c,
        0x11, 0x00, 0x10, 0x7c,
        0x19, 0x00, 0x1c, 0x01,
        0x11, 0x00, 0x1c, 0x01,
        0x19, 0x00, 0x1e, 0x02,
        0x11, 0x00, 0x1e, 0x02,
        0x19, 0x00, 0xf6, 0xe2,
        0x11, 0x00, 0xf6, 0xe2,
        0x19, 0x00, 0xf7, 0xe2,
        0x11, 0x00, 0xf7, 0xe2,
        0x19, 0x00, 0xf8, 0xe2,
        0x11, 0x00, 0xf8, 0xe2,
        0x19, 0x01, 0x90, dat1,
        0x11, 0x01, 0x90, dat1,
        0x19, 0x01, 0x91, 0x00,
        0x11, 0x01, 0x91, 0x00,
        0x19, 0x01, 0x93, dat1,
        0x11, 0x01, 0x93, dat1,
        0x19, 0x01, 0x96, 0x11,
        0x11, 0x01, 0x96, 0x11,
        0x19, 0x01, 0x97, 0x10,
        0x11, 0x01, 0x97, 0x10,
        0x19, 0x01, 0x99, 0x11,
        0x11, 0x01, 0x99, 0x11,
      //0x18, 0x01, 0x9a, 0x00,
      //0x18, 0x81, 0x9a, 0x00,
        0x19, 0x00, 0x18, 0x07,
        0x11, 0x00, 0x18, 0x07,
        0x19, 0x02, 0x32, 0x01,
        0x11, 0x02, 0x32, 0x01,
        0x12, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00,
    };
    QHostAddress ipaddr(ui.ipaddr->text());
    quint16 port = ui.port->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    bool ok = true;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok &= rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        uilog("PLL Configure Success.\n");
    else
        uilog("PLL Configure Fail.\n");
}

void MainWindow::on_AGET_test_clicked()
{
    quint8 modesel = ui.modesel->itemData(ui.modesel->currentIndex()).toUInt();
    quint8 testcap = ui.testcap->itemData(ui.testcap->currentIndex()).toUInt();
    quint8 vicm = ui.vicm->itemData(ui.vicm->currentIndex()).toUInt();
    quint8 agetthres = ui.agetthres->itemData(ui.agetthres->currentIndex()).toUInt();
    quint8 gaincsa = ui.gaincsa->itemData(ui.gaincsa->currentIndex()).toUInt();
    quint8 chthres = ui.chthres->itemData(ui.chthres->currentIndex()).toUInt();
    quint8 dat1 = (gaincsa & 0x03) | ((gaincsa & 0x03) << 2) | ((gaincsa & 0x03) << 4) | ((gaincsa & 0x03) << 6);
    quint8 dat2 = (chthres & 0x0f) | ((chthres & 0x0f) << 4);
    quint8 data[] = {
        0x40, 0x01, (quint8)(modesel >> 1), (quint8)(((testcap & 0x03) << 1) | ((modesel & 0x03) << 7) | 0x78),
        0x43, 0x01, 0x00, (quint8)(((vicm & 0x03) << 1) | ((agetthres & 0x07) << 3)),
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x40, 0x00, 0x00,
        0x40, 0x02, 0xa8, 0x53,
        0x43, 0x02, 0x00, 0x60,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x40, 0x00, 0x00,
        0x40, 0x03, 0x00, 0x00,
        0x42, 0x03, 0x00, 0x00,
        0x45, 0x03, 0x00, 0x02,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x40, 0x00, 0x00,
        0x40, 0x06, dat1, dat1,
        0x42, 0x06, dat1, dat1,
        0x44, 0x06, dat1, dat1,
        0x47, 0x06, dat1, dat1,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x40, 0x00, 0x00,
        0x40, 0x07, dat1, dat1,
        0x42, 0x07, dat1, dat1,
        0x44, 0x07, dat1, dat1,
        0x47, 0x07, dat1, dat1,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x40, 0x00, 0x00,
        0x40, 0x08, dat2, dat2,
        0x42, 0x08, dat2, dat2,
        0x44, 0x08, dat2, dat2,
        0x46, 0x08, dat2, dat2,
        0x48, 0x08, dat2, dat2,
        0x4a, 0x08, dat2, dat2,
        0x4c, 0x08, dat2, dat2,
        0x4f, 0x08, dat2, dat2,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x00, 0x00, 0x00,
        0x48, 0x00, 0x00, 0x00,
        0x4a, 0x00, 0x00, 0x00,
        0x4c, 0x00, 0x00, 0x00,
        0x4e, 0x40, 0x00, 0x00,
        0x40, 0x09, dat2, dat2,
        0x42, 0x09, dat2, dat2,
        0x44, 0x09, dat2, dat2,
        0x46, 0x09, dat2, dat2,
        0x48, 0x09, dat2, dat2,
        0x4a, 0x09, dat2, dat2,
        0x4c, 0x09, dat2, dat2,
        0x4f, 0x09, dat2, dat2,
        0x40, 0x00, 0x00, 0x00,
        0x42, 0x00, 0x00, 0x00,
        0x44, 0x00, 0x00, 0x00,
        0x46, 0x00, 0x00, 0x00,
        0x48, 0x00, 0x00, 0x00,
        0x4a, 0x00, 0x00, 0x00,
        0x4c, 0x00, 0x00, 0x00,
        0x4e, 0x40, 0x00, 0x00,
        0x41, 0x0a, 0x00, 0x00,
        0x40, 0x40, 0x00, 0x00,
    };
    QHostAddress ipaddr(ui.ipaddr->text());
    quint16 port = ui.port->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    bool ok = true;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok &= rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        uilog("AGET test mode Configure Success.\n");
    else
        uilog("AGET test mode Configure Fail.\n");
}

void MainWindow::on_StartSCA_clicked()
{
    quint8 scachannel = ui.scachannel->itemData(ui.scachannel->currentIndex()).toUInt();
    quint8 data[] = {
        (quint8)(((scachannel & 0x07) << 1) | 0x51), 0x08, 0x00, 0x00,
    };
    QHostAddress ipaddr(ui.ipaddr->text());
    quint16 port = ui.port->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    bool ok = true;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok &= rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        uilog("SCA Configure Success.\n");
    else
        uilog("SCA Configure Fail.\n");
}

void MainWindow::on_TriggerEn_clicked(bool checked)
{
    quint8 trigselec = ui.trigselec->itemData(ui.trigselec->currentIndex()).toUInt();
    quint8 trigdelay = ui.trigdelay->itemData(ui.trigdelay->currentIndex()).toUInt();
    quint8 data[] = {
        0x60, 0x06, 0x44, 0x00,
    };
    if(checked) {
        data[1] = 0x0f;
        data[2] = ((trigselec << 6) & 0xc0) | (trigdelay & 0x3f);
    }
    QHostAddress ipaddr(ui.ipaddr->text());
    quint16 port = ui.port->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    bool ok = true;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok &= rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        uilog("Send msb and trig Command Success.\n");
    else
        uilog("Send msb and trig Command Fail.\n");
}

void MainWindow::on_CFigDAC_clicked()
{
    quint8 dacthres = ui.dacthres->itemData(ui.dacthres->currentIndex()).toUInt() & 0x0f;
    quint8 data[] = {
        0x38, 0x26, 0x10, 0x00,
        0x28, 0x80, dacthres, 0x00,
        0x28, 0x00, dacthres, 0x00,
      //0xc1, 0x00, 0x31, 0x10,
      //0xc1, 0x00, 0x31, 0x00,
    };
    QHostAddress ipaddr(ui.ipaddr->text());
    quint16 port = ui.port->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    bool ok = true;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok &= rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        uilog("DAC Configure Success.\n");
    else
        uilog("DAC Configure Fail.\n");
}

static inline void construct_packet(void *buffer, const rbcp_header *header, const void *data)
{
    memcpy(buffer, header, sizeof(rbcp_header));
    if(header->command == RBCP_CMD_WR)
        memcpy((quint8 *)buffer + sizeof(rbcp_header), data, header->length);
}

static inline void log_packet(qint64 size, const void *buffer)
{
    quint8 *data = (quint8 *)buffer;
    QString line;
    for(qint64 i = 0; i < size;) {
        if(i % 4 == 0)
            line.sprintf("[%03x]:", (quint32)i);
        line.append(QString().sprintf("%02x", data[i]));
        if((++i) != size) {
            if(i % 4 == 0)
                qDebug() << qPrintable(line);
            else
                line.append(' ');
        }
    }
    qDebug() << qPrintable(line);
}

bool rbcp_com(const QHostAddress &ipaddr, quint16 port, quint8 command, quint8 length, quint32 address, void *data)
{
    static quint8 id = 0x00;
    QUdpSocket sock;
    if(ipaddr.isNull()) {
        qWarning("Invalid IP Address.");
        return false;
    }
    if(port == 0) {
        qWarning("Invalid Port.");
        return false;
    }
    if((command != RBCP_CMD_WR) && (command != RBCP_CMD_RD)) {
        qWarning("Internal Error - Unknown Command.");
        return false;
    }
    if(length == 0) {
        qWarning("Invalid Length.");
        return false;
    }
    if(data == NULL) {
        qWarning("Internal Error - Data is Null.");
        return false;
    }
    if(!sock.bind()) {
        qWarning("Internal Error - Cannot Bind a UDP Port.");
        return false;
    }
    rbcp_header header;
    header.type = 0xff;
    header.command = command;
    header.id = id++;
    header.length = length;
    header.address = qToBigEndian(address);
    qint64 send_size = sizeof(rbcp_header);
    if(command == RBCP_CMD_WR)
        send_size += length;
    quint8 *send_buffer = new quint8[send_size];
    for(qint8 i = 0; i < 3; i++) {
        QTime t;
        construct_packet(send_buffer, &header, data);
        sock.writeDatagram((char *)send_buffer, send_size, ipaddr, port);
        if(i == 0) {
            qDebug("Sent Packet:");
            log_packet(send_size, send_buffer);
            qDebug("Wait to receive the ACK packet...");
        } else
            qWarning("Timeout!");
        t.start();
        while((t.elapsed() < 1000) && !sock.hasPendingDatagrams());
        //QCoreApplication::processEvents();
        if(sock.hasPendingDatagrams())
            break;
    }
    qint64 recv_size = sock.pendingDatagramSize();
    delete []send_buffer;
    if(recv_size != (qint64)(sizeof(rbcp_header) + length)) {
        if(recv_size == -1)
            qWarning("No Packet Received.");
        else
            qWarning("Packet Length Error.");
        return false;
    }
    quint8 *recv_buffer = new quint8[recv_size];
    QHostAddress recv_ipaddr;
    quint16 recv_port;
    sock.readDatagram((char *)recv_buffer, recv_size, &recv_ipaddr, &recv_port);
    if((recv_ipaddr != ipaddr) || (recv_port != port)) {
        qWarning().nospace() << "Received a Packet From " << qPrintable(recv_ipaddr.toString()) << ':' << recv_port
            << ", Expect " << qPrintable(ipaddr.toString()) << ':' << port << '.';
        delete []recv_buffer;
        return false;
    }
    qDebug("Received:");
    log_packet(recv_size, recv_buffer);
    rbcp_header recv_header;
    memcpy(&recv_header, recv_buffer, sizeof(rbcp_header));
    if((recv_header.type != header.type) || (recv_header.command != (header.command | 0x08)) ||
        (recv_header.id != header.id) || (recv_header.length != header.length) || (recv_header.address != header.address)) {
        qWarning("Received Packet Header Error.");
        delete []recv_buffer;
        return false;
    }
    if(command == RBCP_CMD_WR) {
        quint8 *recv_data = new quint8[length];
        memcpy(recv_data, recv_buffer + sizeof(rbcp_header), length);
        if(memcmp(recv_data, data, length)) {
            qWarning("Error ACK Packet.");
            delete []recv_data;
            delete []recv_buffer;
            return false;
        }
        delete []recv_data;
    }
    if(command == RBCP_CMD_RD)
        memcpy(data, recv_buffer + sizeof(rbcp_header), length);
    delete []recv_buffer;
    return true;
}

void msg_handler(QtMsgType type, const char *msg)
{
    QTextStream out(stdout);
    if(type == QtDebugMsg)
        out << "[Info]:";
    else if(type == QtWarningMsg)
        out << "[Warning]:";
    else
        out << "[ERROR]:";
    out << msg << endl;
}
