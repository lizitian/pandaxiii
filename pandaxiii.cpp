#include "pandaxiii.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

void MainWindow::on_write_clicked()
{
    QString data_str = rbcp_data();
    QRegExp regex("0[xX][0-9a-fA-F]+");
    if(data_str.size() <= 2 || !regex.exactMatch(data_str)) {
        qWarning("Data Error.");
        statusBar()->showMessage("Data Error!");
        return;
    }
    if(data_str.size() % 2)
        data_str.insert(2, '0');
    for(qint64 i = 1; i < data_str.size() / 2; i++) {
        quint8 data = data_str.mid(i * 2, 2).toUInt(0, 16);
        if(!rbcp_com(rbcp_ipaddr(), rbcp_port(), RBCP_CMD_WR, 1, rbcp_address(), &data)) {
            qWarning("Send Data Error.");
            statusBar()->showMessage("Send Data Error!");
            return;
        }
    }
    statusBar()->showMessage("Send Data OK.");
}

void MainWindow::on_read_clicked()
{
    quint8 length = rbcp_length();
    quint32 address = rbcp_address();
    quint8 *data = new quint8[length];
    if(rbcp_com(rbcp_ipaddr(), rbcp_port(), RBCP_CMD_RD, length, address, data)) {
        QString buffer;
        buffer.sprintf("Received Data: [0x%08x]:", address);
        for(quint8 i = 0; i < length; i++) {
            if((i % 4 == 0) && (i != 0))
                buffer.append("- ");
            buffer.append(QString().sprintf("%02x ", data[i]));
        }
        statusBar()->showMessage(buffer);
    } else
        statusBar()->showMessage("Error!");
    delete []data;
}

void MainWindow::on_CFigPLL_clicked()
{
    quint8 samplerate = rbcp_samplerate() & 0x0f;
    quint8 dat1 = samplerate | (samplerate << 4);
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
    QHostAddress ipaddr = rbcp_ipaddr();
    quint16 port = rbcp_port();
    quint32 address = rbcp_address();
    bool ok;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok = rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        statusBar()->showMessage("PLL Configure Success.");
    else
        statusBar()->showMessage("PLL Configure Fail.");
}

void MainWindow::on_AGET_test_clicked()
{
    quint8 modesel = rbcp_modesel() & 0x03;
    quint8 testcap = rbcp_testcap() & 0x03;
    quint8 vicm = rbcp_vicm() & 0x03;
    quint8 agetthres = rbcp_agetthres() & 0x07;
    quint8 gaincsa = rbcp_gaincsa() & 0x03;
    quint8 chthres = rbcp_chthres() & 0x0f;
    quint8 dat1 = gaincsa | (gaincsa << 2) | (gaincsa << 4) | (gaincsa << 6);
    quint8 dat2 = chthres | (chthres << 4);
    quint8 data[] = {
        0x40, 0x01, (quint8)(modesel >> 1), (quint8)((testcap << 1) | (modesel << 7) | 0x78),
        0x43, 0x01, 0x00, (quint8)((vicm << 1) | (agetthres << 3)),
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
    QHostAddress ipaddr = rbcp_ipaddr();
    quint16 port = rbcp_port();
    quint32 address = rbcp_address();
    bool ok;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok = rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        statusBar()->showMessage("AGET test mode Configure Success.");
    else
        statusBar()->showMessage("AGET test mode Configure Fail.");
}

void MainWindow::on_StartSCA_clicked()
{
    quint8 scachannel = rbcp_scachannel() & 0x07;
    quint8 data[] = {
        (quint8)((scachannel << 1) | 0x51), 0x08, 0x00, 0x00,
    };
    QHostAddress ipaddr = rbcp_ipaddr();
    quint16 port = rbcp_port();
    quint32 address = rbcp_address();
    bool ok;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok = rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        statusBar()->showMessage("SCA Configure Success.");
    else
        statusBar()->showMessage("SCA Configure Fail.");
}

void MainWindow::on_TriggerEn_clicked(bool checked)
{
    quint8 data[] = {
        0x60, 0x06, 0x44, 0x00,
    };
    if(checked) {
        data[1] = 0x0f;
        data[2] = ((rbcp_trigselec() << 6) & 0xc0) | (rbcp_trigdelay() & 0x3f);
    }
    QHostAddress ipaddr = rbcp_ipaddr();
    quint16 port = rbcp_port();
    quint32 address = rbcp_address();
    bool ok;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok = rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        statusBar()->showMessage("Send msb and trig Command Success.");
    else
        statusBar()->showMessage("Send msb and trig Command Fail.");
}

void MainWindow::on_CFigDAC_clicked()
{
    quint8 dacthres = rbcp_dacthres() & 0x0f;
    quint8 data[] = {
        0x38, 0x26, 0x10, 0x00,
        0x28, 0x80, dacthres, 0x00,
        0x28, 0x00, dacthres, 0x00,
      //0xc1, 0x00, 0x31, 0x10,
      //0xc1, 0x00, 0x31, 0x00,
    };
    QHostAddress ipaddr = rbcp_ipaddr();
    quint16 port = rbcp_port();
    quint32 address = rbcp_address();
    bool ok;
    for(quint16 i = 0; i < sizeof(data); i++) {
        ok = rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data[i]);
        if(!ok)
            break;
    }
    if(ok)
        statusBar()->showMessage("DAC Configure Success.");
    else
        statusBar()->showMessage("DAC Configure Fail.");
}

void MainWindow::on_connect_clicked(bool checked)
{
    static TcpCom *sock = NULL;
    if((sock != NULL) == checked) {
        qWarning("Internal Error - Socket Status Error.");
        statusBar()->showMessage("Connect Error.");
        return;
    }
    tcp_set_enabled(false);
    if(checked) {
        sock = new TcpCom(this);
        sock->connectToHost(tcp_ipaddr(), tcp_port());
    } else {
        sock->disconnectFromHost();
        sock = NULL;
    }
}

static QPicture background_picture(qreal aspect_ratio)
{
    QPicture picture;
    QPainter painter;
    qint16 h = 30, v = h * aspect_ratio;
    painter.begin(&picture);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkCyan);
    painter.drawRect(0, 0, 1, 1);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::NoBrush);
    for(qint16 i = 0; i <= h; i++)
        painter.drawLine(QPointF(0, (qreal)i / h), QPointF(1, (qreal)i / h));
    for(qint16 i = 0; i <= v; i++)
        painter.drawLine(QPointF((qreal)i / v, 0), QPointF((qreal)i / v, 1));
    painter.end();
    return picture;
}

void MainWindow::on_draw_clicked()
{
    QPicture picture;
    QPainter painter;
    QPainterPath path;
    TcpData *tcp_data = new TcpData("data.dat");
    quint16 data[TcpData::units];
    if(!(tcp_data->read() && tcp_data->get_data(tcp_chip(), tcp_channel(), data)))
        return;
    delete tcp_data;
    path.moveTo(0, (qreal)(TcpData::datamask - data[0]) / TcpData::datamask);
    for(qint64 i = 1; i < TcpData::units; i++)
        path.lineTo((qreal)i / (TcpData::units - 1), (qreal)(TcpData::datamask - data[i]) / TcpData::datamask);
    painter.begin(&picture);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPicture(0, 0, background_picture(tcp_canvas_get_aspect_ratio()));
    painter.setPen(QPen(Qt::green, 0.005));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
    painter.end();
    tcp_canvas_set_picture(picture);
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

TcpCom::TcpCom(MainWindow *window, QObject *parent) : QTcpSocket(parent)
{
    this->window = window;
    connect(this, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(this, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
    stat = 0;
    file = NULL;
    data = new quint8[bufsize];
}

TcpCom::~TcpCom()
{
    delete []data;
    if(file != NULL)
        delete file;
}

void TcpCom::on_connected()
{
    window->statusBar()->showMessage("Connected.");
    window->tcp_set_connected(true);
    window->tcp_set_enabled(true);
    file = new QFile(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz'.dat'"));
    file->open(QIODevice::WriteOnly);
    t.start();
}

void TcpCom::on_disconnected()
{
    window->statusBar()->showMessage("Disconnected.");
    window->tcp_set_connected(false);
    window->tcp_set_enabled(true);
    file->close();
    this->deleteLater();
}

void TcpCom::on_readyRead()
{
    qint64 size = bytesAvailable(), readed;
    if(size <= 0) {
        qWarning("No Data to Read.");
        return;
    }
    quint8 *data = this->data;
    if(size > bufsize)
        data = new quint8[size];
    readed = read((char *)data, size);
    if(readed != size)
        qWarning("Data Length Mismatch!");
    file->write((char *)data, readed);
    if(size > bufsize)
        delete []data;
    stat += readed;
    if(t.elapsed() > 1000) {
        window->statusBar()->showMessage(QString("Speed: %1 Mbps").arg((double)stat * 8000 / t.elapsed() / 1024 / 1024));
        stat = 0;
        t.restart();
    }
}

TcpData::TcpData(const QString &name)
{
    file = new QFile(name);
}

TcpData::~TcpData()
{
    delete file;
}

bool TcpData::read()
{
    quint32 data;
    file->open(QIODevice::ReadOnly);
    if(!read32(&data) || qFromBigEndian(data) != header) {
        qWarning("Wrong Header.");
        file->close();
        return false;
    }
    if(!read32(&data)) {
        file->close();
        return false;
    }
    options = qFromBigEndian(data);
    for(qint64 i = 0; i < units; i++) {
        if(!read32(&data) || qFromBigEndian(data) != gap1) {
            qWarning() << "Wrong Gap, Unit:" << i << '.';
            file->close();
            return false;
        }
        if(!read32(&data) || qFromBigEndian(data) != gap2) {
            qWarning() << "Wrong Gap, Unit:" << i << '.';
            file->close();
            return false;
        }
        for(qint64 j = 0; j < channels; j++) {
            if(!read32(&data)) {
                file->close();
                return false;
            }
            chip1[j][i] = qFromBigEndian(data) >> 16;
            if((chip1[j][i] & ~datamask) != chip1bits) {
                qWarning() << "Wrong Data, Chip: 1 , Channel:" << j << ", Unit:" << i << '.' << chip1[j][i];
                file->close();
                return false;
            }
            chip1[j][i] &= datamask;
            chip2[j][i] = qFromBigEndian(data);
            if((chip2[j][i] & ~datamask) != chip2bits) {
                qWarning() << "Wrong Data, Chip: 2 , Channel:" << j << ", Unit:" << i << '.';
                file->close();
                return false;
            }
            chip2[j][i] &= datamask;
        }
    }
    if(!read32(&data) || qFromBigEndian(data) != footer) {
        qWarning("Wrong Footer.");
        file->close();
        return false;
    }
    file->close();
    return true;
}

bool TcpData::get_data(qint64 chip, qint64 channel, quint16 *data)
{
    if(channel < 0 || channel >= channels) {
        qWarning("Wrong Channel.");
        return false;
    }
    for(qint64 i = 0; i < units; i++)
        if(chip == 1)
            data[i] = chip1[channel][i];
        else if(chip == 2)
            data[i] = chip2[channel][i];
        else {
            qWarning("Wrong Chip");
            return false;
        }
    return true;
}

bool TcpData::read32(quint32 *data)
{
    qint64 readed;
    readed = file->read((char *)data, sizeof(quint32));
    if(readed != sizeof(quint32)) {
        qWarning("Read file error.");
        return false;
    }
    return true;
}
