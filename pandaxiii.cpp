#include "pandaxiii.h"
static void msg_handler(QtMsgType type, const char *msg)
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

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    qInstallMsgHandler(msg_handler);
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
        if(!rbcp_write(ipaddr(), rbcp_port(), rbcp_address(), data)) {
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
    if(rbcp_read(ipaddr(), rbcp_port(), length, address, data)) {
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

void MainWindow::on_errortest_clicked(bool checked)
{
    static QTimer *t = NULL;
    QHostAddress ip_address = ipaddr();
    quint16 port = rbcp_port();
    quint8 mask = sfp_status_mask(3);
    if(checked) {
        if(!rbcp_write(ip_address, port, 0xfffe002c, mask) || !rbcp_write(ip_address, port, 0xfffe002b, mask)) {
            statusBar()->showMessage("Error rate test error.");
            errortest_set_checked(false);
            return;
        }
        this->t = new QTime();
        this->t->start();
        t = new QTimer();
        connect(t, SIGNAL(timeout()), this, SLOT(errortest_tick()));
        t->start(5000);
        errortest_set_checked(true);
    } else {
        if(!rbcp_write(ip_address, port, 0xfffe002c, 0x00))
            statusBar()->showMessage("Error rate test error.");
        delete t;
        t = NULL;
        delete this->t;
        this->t = NULL;
        errortest_set_checked(false);
    }
}

void MainWindow::errortest_tick()
{
    static const qreal datarate = 800000; // bits per millisecond
    QHostAddress ip_address = ipaddr();
    quint16 port = rbcp_port();
    quint8 mask = sfp_status_mask(3);
    quint64 errorrate = 0;
    QString msg = "Error Rate: ";
    for(qint64 i = 0; i < 5; i++) {
        quint8 data;
        if(!(mask & (0x01 << i)))
            continue;
        if(!rbcp_read(ip_address, port, 1, 0xfffe0026, &data)) {
            msg = "Error.";
            return;
        }
        if(!rbcp_read(ip_address, port, 1, 0xfffe0036 + i, &data)) {
            msg = "Error.";
            return;
        }
        errorrate = data << 24;
        if(!rbcp_read(ip_address, port, 1, 0xfffe0036 + i, &data)) {
            msg = "Error.";
            return;
        }
        errorrate |= data << 16;
        if(!rbcp_read(ip_address, port, 1, 0xfffe0036 + i, &data)) {
            msg = "Error.";
            return;
        }
        errorrate |= data << 8;
        if(!rbcp_read(ip_address, port, 1, 0xfffe0036 + i, &data)) {
            msg = "Error.";
            return;
        }
        errorrate |= data;
        msg.append(QString("SFP%1 %2%3 ").arg(i).arg(data == 0 ? "<" : "").arg((data == 0 ? 1 : errorrate) / datarate / t->elapsed()));
    }
    statusBar()->showMessage(msg);
}

void MainWindow::on_CFigPLL_clicked()
{
    quint8 samplerate = fec_samplerate() & 0x0f;
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
    fec_configure(data, sizeof(data), "PLL Configure");
}

void MainWindow::on_AGET_test_clicked()
{
    quint8 modesel = fec_modesel() & 0x03;
    quint8 testcap = fec_testcap() & 0x03;
    quint8 vicm = fec_vicm() & 0x03;
    quint8 agetthres = fec_agetthres() & 0x07;
    quint8 gaincsa = fec_gaincsa() & 0x03;
    quint8 chthres = fec_chthres() & 0x0f;
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
    fec_configure(data, sizeof(data), "AGET test mode Configure");
}

void MainWindow::on_StartSCA_clicked()
{
    quint8 scachannel = fec_scachannel() & 0x07;
    quint8 data[] = {
        (quint8)((scachannel << 1) | 0x51), 0x08, 0x00, 0x00,
    };
    fec_configure(data, sizeof(data), "SCA Configure");
}

void MainWindow::on_TriggerEn_clicked(bool checked)
{
    quint8 data[] = {
        0x60, 0x06, 0x44, 0x00,
    };
    if(checked) {
        data[1] = 0x0f;
        data[2] = ((fec_trigselec() << 6) & 0xc0) | (fec_trigdelay() & 0x3f);
    }
    if(!fec_configure(data, sizeof(data), "Send msb and trig Command"))
        TriggerEn_set_checked(!checked);
}

void MainWindow::on_CFigDAC_clicked()
{
    quint8 dacthres = fec_dacthres() & 0x0f;
    quint8 data[] = {
        0x38, 0x26, 0x10, 0x00,
        0x28, 0x80, dacthres, 0x00,
        0x28, 0x00, dacthres, 0x00,
      //0xc1, 0x00, 0x31, 0x10,
      //0xc1, 0x00, 0x31, 0x00,
    };
    fec_configure(data, sizeof(data), "DAC Configure");
}

bool MainWindow::fec_configure(const quint8 *data, qint64 length, const QString &str)
{
    QHostAddress ip_address = ipaddr();
    quint16 port = rbcp_port();
    quint32 address_base = 0xfffe0020;
    quint8 mask = sfp_status_mask(1);
    QString msg = str;
    bool ok = true;
    for(qint64 i = 0; i < 5; i++) {
        if(!(mask & (0x01 << i)))
            continue;
        for(quint16 j = 0; j < length; j++) {
            ok = rbcp_write(ip_address, port, address_base + i, data[j]);
            if(!ok)
                break;
        }
    }
    if(ok)
        statusBar()->showMessage(msg.append(" Success."));
    else
        statusBar()->showMessage(msg.append(" Fail."));
    return ok;
}

void MainWindow::on_connect_clicked(bool checked)
{
    if((tcp_worker != NULL) == checked) {
        qWarning("Internal Error - Socket Status Error.");
        statusBar()->showMessage("Connect Error.");
        return;
    }
    tcp_set_enabled(false);
    if(checked) {
        bool ok = rbcp_write(ipaddr(), rbcp_port(), 0xfffe0026, 0x07);
        if(!ok) {
            statusBar()->showMessage("Cannot send clear command.");
            tcp_set_connected(false);
            tcp_set_enabled(true);
            return;
        }
        tcp_worker = new TcpWorker(ipaddr(), tcp_port(), this);
        tcp_worker->start();
    } else
        emit tcp_user_disconnect();
}

void MainWindow::tcp_receive_data(quint8 *data)
{
    static QFile *file = NULL;
    static qint64 stat = 0, sum = 0;
    static QTime *t = NULL;
    static QTimer *timer = NULL;
    static quint8 *queue;
    if(data != NULL) {
        filename = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz'.dat'");
        file = new QFile(filename);
        file->open(QIODevice::WriteOnly);
        t = new QTime();
        t->start();
        timer = new QTimer();
        timer->start(5);
        connect(timer, SIGNAL(timeout()), this, SLOT(write_data_tick()));
        queue = data;
    }
    while(*((quint8 **)queue) != NULL) {
        qint64 size = *((qint64 *)(queue + sizeof(quint8 *)));
        quint8 *old = queue;
        if(size > 0) {
            sum += size;
            if(sum > 1024 * 1024 * 1024) {
                file->close();
                delete file;
                filename = QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz'.dat'");
                file = new QFile(filename);
                file->open(QIODevice::WriteOnly);
                sum = size;
            }
            file->write((char *)(queue + sizeof(quint8 *) + sizeof(qint64)), size);
            stat += size;
            if(t->elapsed() > 1000) {
                statusBar()->showMessage(QString("Speed: %1 Mbps").arg((qreal)stat * 8000 / t->elapsed() / 1024 / 1024));
                stat = 0;
                t->restart();
            }
        } else if(size != -1)
            qWarning("Wrong Tcp Queue");
        queue = *((quint8 **)queue);
        delete []old;
    }
    if(*((qint64 *)(queue + sizeof(quint8 *))) == 0) {
        file->close();
        delete file;
        delete t;
        delete timer;
        file = NULL;
        stat = 0;
        sum = 0;
        t = NULL;
        timer = NULL;
        delete []queue;
        queue = NULL;
    }
}

void MainWindow::write_data_tick()
{
    tcp_receive_data(NULL);
}

void MainWindow::tcp_disconnected()
{
    tcp_worker->quit();
    tcp_worker->wait();
    delete tcp_worker;
    tcp_worker = NULL;
    statusBar()->showMessage("Disconnected.");
    tcp_set_connected(false);
    tcp_set_enabled(true);
}

static QPicture background_picture(qreal aspect_ratio)
{
    QPicture picture;
    QPainter painter;
    qint16 h = 8, v = h * aspect_ratio;
    painter.begin(&picture);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, 1, 1);
    painter.setPen(Qt::darkGreen);
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
    TcpData *tcp_data = new TcpData(filename);
    quint16 data[TcpData::units];
    qreal avg = 0, sum_square = 0;
    if(!(tcp_data->read(tcp_packet()) && tcp_data->get_data(tcp_chip(), tcp_channel(), data))) {
        delete tcp_data;
        return;
    }
    for(qint64 i = 0; i < TcpData::units; i++)
        avg += data[i];
    avg /= TcpData::units;
    for(qint64 i = 0; i < TcpData::units; i++)
        sum_square += (data[i] - avg) * (data[i] - avg);
    statusBar()->showMessage(QString().sprintf("Trigger ID: 0x%08x, ", tcp_data->get_trigger()).append(QString("avg = %1, rms = %2").arg(avg).arg(sqrt(sum_square / (TcpData::units - 1)))));
    delete tcp_data;
    path.moveTo(0, (qreal)(TcpData::datamask - data[0]) / TcpData::datamask);
    for(qint64 i = 1; i < TcpData::units; i++)
        path.lineTo((qreal)i / (TcpData::units - 1), (qreal)(TcpData::datamask - data[i]) / TcpData::datamask);
    painter.begin(&picture);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPicture(0, 0, background_picture(tcp_canvas_get_aspect_ratio()));
    painter.setPen(QPen(Qt::white, 0.005));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
    painter.end();
    tcp_canvas_set_picture(picture);
}

void MainWindow::on_baselinebutton_clicked()
{
    QPicture picture;
    QPainter painter;
    TcpData *tcp_data = new TcpData(filename);
    if(!tcp_data->read(tcp_packet())) {
        delete tcp_data;
        return;
    }
    painter.begin(&picture);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPicture(0, 0, background_picture(tcp_canvas_get_aspect_ratio())); // bug
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    for(qint64 chip = 0; chip < TcpData::chips; chip++)
        for(qint64 i = 0; i < TcpData::channels; i++) {
            qreal avg = 0, height;
            quint16 data[TcpData::units];
            if(!tcp_data->get_data(chip + 1, i, data)) {
                delete tcp_data;
                return;
            }
            for(qint64 j = 0; j < TcpData::units; j++)
                avg += data[j];
            avg /= TcpData::units;
            if(baseline_mode() == 0)
                height = avg / TcpData::datamask;
            else {
                qreal sum_square = 0;
                for(qint64 j = 0; j < TcpData::units; j++)
                    sum_square += (data[j] - avg) * (data[j] - avg);
                height = sqrt(sum_square / (TcpData::units - 1)) / TcpData::datamask;
            }
            painter.drawRect(QRectF((qreal)(chip * TcpData::channels + i) / (TcpData::chips * TcpData::channels), 1 - height, 1.0 / (TcpData::chips * TcpData::channels), height));
        }
    delete tcp_data;
    painter.end();
    baseline_canvas_set_picture(picture);
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

static bool rbcp_com(const QHostAddress &ipaddr, quint16 port, quint8 command, quint8 length, quint32 address, void *data)
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
    construct_packet(send_buffer, &header, data);
    sock.writeDatagram((char *)send_buffer, send_size, ipaddr, port);
    qDebug("Sent Packet:");
    log_packet(send_size, send_buffer);
    qDebug("Wait to receive the ACK packet...");
    QTime t;
    t.start();
    while((t.elapsed() < 200) && !sock.hasPendingDatagrams());
    //QCoreApplication::processEvents();
    qint64 recv_size = sock.pendingDatagramSize();
    delete []send_buffer;
    if(recv_size != (qint64)(sizeof(rbcp_header) + length)) {
        if(recv_size == -1)
            qWarning("Timeout, No Packet Received.");
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

bool rbcp_read(const QHostAddress &ipaddr, quint16 port, quint8 length, quint32 address, void *data)
{
    for(qint8 i = 0; i < 3; i++) {
        if(i != 0)
            qDebug("Resending...");
        if(rbcp_com(ipaddr, port, RBCP_CMD_RD, length, address, data))
            return true;
    }
    return false;
}

bool rbcp_write(const QHostAddress &ipaddr, quint16 port, quint32 address, quint8 data)
{
    static const quint32 count_addr = 0xfffe0000;
    static const quint32 data_addr = 0xfffe0001;
    static const quint32 address_addr = 0xfffe0002;
    static quint8 count;
    static bool initialized = false;
    quint8 current_count;
    quint8 current_data;
    quint8 current_address;
    if(!initialized) {
        if(!rbcp_read(ipaddr, port, 1, count_addr, &count)) {
            qWarning("Cannot read current count register.");
            return false;
        }
        initialized = true;
    }
    for(qint8 i = 0; i < 3; i++) {
        if(i != 0)
            qDebug("Resending...");
        if(rbcp_com(ipaddr, port, RBCP_CMD_WR, 1, address, &data)) {
            count++;
            return true;
        }
        if(!rbcp_read(ipaddr, port, 1, count_addr, &current_count)) {
            qWarning("Cannot read current count register.");
            initialized = false;
            return false;
        }
        if(current_count == (quint8)(count + 1)) {
            count++;
            if(!rbcp_read(ipaddr, port, 1, data_addr, &current_data)) {
                qWarning("Cannot read current data register.");
                return false;
            }
            if(current_data != data) {
                qWarning("Current Data Error.");
                return false;
            }
            if(!rbcp_read(ipaddr, port, 1, address_addr, &current_address)) {
                qWarning("Cannot read current address register.");
                return false;
            }
            if(current_address != (address & 0xff)) {
                qWarning("Current Address Error.");
                return false;
            }
            return true;
        }
        if(current_count != count) {
            qWarning().nospace() << "Current Count is " << current_count << ", Count is " << count << ".";
            count = current_count;
            return false;
        }
    }
    return false;
}

const quint16 TcpData::chipbits[] = { 0xa000, 0xb000, 0xc000, 0xd000 };

TcpData::TcpData(const QString &name)
{
    file = new QFile(name);
}

TcpData::~TcpData()
{
    delete file;
}

bool TcpData::read(quint16 packet)
{
    quint16 data;
    quint16 flags = 0;
    file->open(QIODevice::ReadOnly);
    while(flags != 0xffff >> (16 - chips)) {
        qint64 chip = -1;
        if(!read16(&data) || qFromBigEndian(data) != header) {
            qWarning("Wrong Header.");
            file->close();
            return false;
        }
        if(!read16(&data)) {
            qWarning("Wrong Header.");
            file->close();
            return false;
        }
        if(qFromBigEndian(data) != packet)
            chip = -2;
        if(!read16(&data)) {
            qWarning("Wrong Header.");
            file->close();
            return false;
        }
        trigger = qFromBigEndian(data) << 16;
        if(!read16(&data)) {
            qWarning("Wrong Header.");
            file->close();
            return false;
        }
        trigger |= qFromBigEndian(data);
        if(!read16(&data)) {
            qWarning("Wrong Header.");
            file->close();
            return false;
        }
        if(chip != -2) {
            for(qint64 i = 0; i < chips; i++)
                if((qFromBigEndian(data) & ~datamask) == chipbits[i]) {
                    chip = i;
                    break;
                }
            if(chip != -1) {
                if(flags & (0x1 << chip))
                    chip = -1;
                else
                    flags |= 0x1 << chip;
            }
        }
        for(qint64 i = 0; i < units; i++) {
            if(i != 0 && (!read16(&data) || ((chip >= 0) && ((qFromBigEndian(data) & ~datamask) != chipbits[chip])))) {
                qWarning("Wrong Header.");
                file->close();
                return false;
            }
            if(!read16(&data) || ((chip >= 0) && ((qFromBigEndian(data) & ~datamask) != chipbits[chip]))) {
                qWarning("Wrong Header.");
                file->close();
                return false;
            }
            for(qint64 j = 0; j < channels; j++) {
                if(!read16(&data) || ((chip >= 0) && ((qFromBigEndian(data) & ~datamask) != chipbits[chip]))) {
                    qWarning() << "Wrong Data, Chip:" << chip + 1 << ", Channel:" << j << ", Unit:" << i << '.';
                    file->close();
                    return false;
                }
                if(chip >= 0)
                    this->data[chip][j][i] = qFromBigEndian(data) & datamask;
            }
        }
        if(!read16(&data) || qFromBigEndian(data) != footer) {
            qWarning("Wrong Footer.");
            file->close();
            return false;
        }
    }
    file->close();
    return true;
}

bool TcpData::get_data(qint64 chip, qint64 channel, quint16 *data)
{
    if(chip <= 0 || chip > chips || channel < 0 || channel >= channels) {
        qWarning("Wrong Channel Or Chip.");
        return false;
    }
    for(qint64 i = 0; i < units; i++)
        data[i] = this->data[chip - 1][channel][i];
    return true;
}

quint32 TcpData::get_trigger()
{
    return trigger;
}

bool TcpData::read16(quint16 *data)
{
    qint64 readed;
    readed = file->read((char *)data, sizeof(quint16));
    if(readed != sizeof(quint16)) {
        qWarning("Read file error.");
        return false;
    }
    return true;
}

TcpWorker::TcpWorker(const QHostAddress &ipaddr, quint16 port, MainWindow *window)
{
    this->ipaddr = ipaddr;
    this->port = port;
    this->window = window;
}

void TcpWorker::run()
{
    TcpCom *sock = new TcpCom();
    connect(window, SIGNAL(tcp_user_disconnect()), sock, SLOT(user_disconnect()));
    connect(sock, SIGNAL(send_data(quint8 *)), window, SLOT(tcp_receive_data(quint8 *)));
    connect(sock, SIGNAL(ui_status(QString)), window->statusBar(), SLOT(showMessage(QString)));
    connect(sock, SIGNAL(ui_tcp_connected(bool)), window, SLOT(tcp_set_connected(bool)));
    connect(sock, SIGNAL(ui_tcp_enabled(bool)), window, SLOT(tcp_set_enabled(bool)));
    connect(sock, SIGNAL(quit()), window, SLOT(tcp_disconnected()));
    sock->connectToHost(ipaddr, port);
    exec();
}

TcpCom::TcpCom(QObject *parent) : QTcpSocket(parent)
{
    connect(this, SIGNAL(connected()), this, SLOT(on_connected()));
    connect(this, SIGNAL(disconnected()), this, SLOT(on_disconnected()));
    connect(this, SIGNAL(readyRead()), this, SLOT(on_readyRead()));
}

void TcpCom::on_connected()
{
    queue = new quint8[sizeof(quint8 *) + sizeof(qint64)];
    emit ui_status("Connected.");
    emit ui_tcp_connected(true);
    emit ui_tcp_enabled(true);
    *((quint8 **)queue) = NULL;
    *((qint64 *)(queue + sizeof(quint8 *))) = -1;
    emit send_data(queue);
}

void TcpCom::on_disconnected()
{
    quint8 *data = new quint8[sizeof(quint8 *) + sizeof(qint64)];
    *((quint8 **)queue) = data;
    queue = data;
    *((quint8 **)queue) = NULL;
    *((qint64 *)(queue + sizeof(quint8 *))) = 0;
    deleteLater();
    emit quit();
}

void TcpCom::on_readyRead()
{
    qint64 size = bytesAvailable();
    if(size <= 0)
        return;
    quint8 *data = new quint8[size + sizeof(quint8 *) + sizeof(qint64)];
    size = read((char *)(data + sizeof(quint8 *) + sizeof(qint64)), size);
    if(size <= 0) {
        delete []data;
        return;
    }
    *((quint8 **)queue) = data;
    queue = data;
    *((quint8 **)queue) = NULL;
    *((qint64 *)(queue + sizeof(quint8 *))) = size;
}

void TcpCom::user_disconnect()
{
    disconnectFromHost();
}
