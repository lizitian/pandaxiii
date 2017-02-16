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
}

void MainWindow::on_write_clicked()
{
    quint8 data = ui.data->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    if(rbcp_com(QHostAddress(ui.ipaddr->text()), ui.port->text().toUInt(0, 0), RBCP_CMD_WR, 1, address, &data)) {
        ui.output->moveCursor(QTextCursor::End);
        ui.output->insertPlainText("OK!\n");
    } else {
        ui.output->moveCursor(QTextCursor::End);
        ui.output->insertPlainText("Error!\n");
    }
}

void MainWindow::on_read_clicked()
{
    quint8 length = ui.length->text().toUInt(0, 0);
    quint32 address = ui.address->text().toUInt(0, 0);
    quint8 *data = new quint8[length];
    if(rbcp_com(QHostAddress(ui.ipaddr->text()), ui.port->text().toUInt(0, 0), RBCP_CMD_RD, length, address, data)) {
        ui.output->moveCursor(QTextCursor::End);
        ui.output->insertPlainText("Data:\n");
        // to fix
        char buffer[100];
        for(quint8 i = 0; i < length; i++) {
            if(i % 8 == 0)
                sprintf(buffer, "[0x%08x]:%02x ", address + i, data[i]);
            else if(i % 8 == 7)
                sprintf(buffer, "%02x\n", data[i]);
            else if(i % 8 == 3)
                sprintf(buffer, "%02x - ", data[i]);
            else
                sprintf(buffer, "%02x ", data[i]);
            ui.output->moveCursor(QTextCursor::End);
            ui.output->insertPlainText(buffer);
        }
        ui.output->moveCursor(QTextCursor::End);
        ui.output->insertPlainText("\n");
    } else {
        ui.output->moveCursor(QTextCursor::End);
        ui.output->insertPlainText("Error!\n");
    }
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
    qint64 i;
    for(i = 0; i < size; i++) {
        if(i % 4 == 0)
            qDebug("Info: [%03x]:%02x ", (quint32)i, data[i]);
        else if(i % 4 == 3)
            qDebug("%02x\n", data[i]);
        else
            qDebug("%02x ", data[i]);
    }
    if(i % 4 != 0)
        qDebug("\n");
}

bool rbcp_com(const QHostAddress &ipaddr, quint16 port, quint8 command, quint8 length, quint32 address, void *data)
{
    const quint8 type = 0xff;
    static rbcp_header header = { type, 0x00, 0x00, 0x00, 0x00000000 };
    QUdpSocket sock;
    if(ipaddr.isNull()) {
        qWarning("ERROR: Invalid IP Address.\n");
        return false;
    }
    if(port == 0) {
        qWarning("ERROR: Invalid Port.\n");
        return false;
    }
    if((command != RBCP_CMD_WR) && (command != RBCP_CMD_RD)) {
        qWarning("ERROR: Internal Error - Unknown Command.\n");
        return false;
    }
    if(length == 0) {
        qWarning("ERROR: Invalid Length.\n");
        return false;
    }
    if(data == NULL) {
        qWarning("ERROR: Internal Error - Data is Null.\n");
        return false;
    }
    if(!sock.bind()) {
        qWarning("ERROR: Internal Error - Cannot Bind a UDP Port.\n");
        return false;
    }
    header.command = command;
    header.id++;
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
            qDebug("Info: Sent Packet:\n");
            log_packet(send_size, send_buffer);
            qDebug("Info: Wait to receive the ACK packet...\n");
        } else
            qWarning("ERROR: Timeout!\n");
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
            qWarning("ERROR: No Packet Received.\n");
        else
            qWarning("ERROR: Packet Length Error.\n");
        return false;
    }
    quint8 *recv_buffer = new quint8[recv_size];
    QHostAddress recv_ipaddr;
    quint16 recv_port;
    sock.readDatagram((char *)recv_buffer, recv_size, &recv_ipaddr, &recv_port);
    if((recv_ipaddr != ipaddr) || (recv_port != port)) {
        qWarning("ERROR: Received a Packet From %s:%u, Expected %s:%u.\n",
            recv_ipaddr.toString().toLocal8Bit().constData(), recv_port, ipaddr.toString().toLocal8Bit().constData(), port);
        delete []recv_buffer;
        return false;
    }
    qDebug("Info: Received:\n");
    log_packet(recv_size, recv_buffer);
    rbcp_header recv_header;
    memcpy(&recv_header, recv_buffer, sizeof(rbcp_header));
    if((recv_header.type != type) || (recv_header.command != (command | 0x08)) ||
        (recv_header.id != header.id) || (recv_header.length != length) || (recv_header.address != address)) {
        qWarning("ERROR: Received Packet Header Error.\n");
        delete []recv_buffer;
        return false;
    }
    if(command == RBCP_CMD_WR) {
        quint8 *recv_data = new quint8[length];
        memcpy(recv_data, recv_buffer + sizeof(rbcp_header), length);
        if(memcmp(recv_data, data, length)) {
            qWarning("ERROR: Error ACK Packet.\n");
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
    if((type == QtDebugMsg) || (type == QtWarningMsg))
        fputs(msg, stdout);
    else
        fputs(msg, stderr);
}
