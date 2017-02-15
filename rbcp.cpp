#include "rbcp.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
}

#define RBCP_VER 0xFF
#define RBCP_CMD_WR 0x80
#define RBCP_CMD_RD 0xC0
#define UDP_BUF_SIZE 2048
static struct rbcp_header header = {
    .type = RBCP_VER,
    .command = 0,
    .id = 0,
    .length = 0,
    .address = 0
};
void MainWindow::on_write_clicked()
{
    QByteArray ipaddr = ui.ipaddr->text().toUtf8();
    int port = ui.port->text().toInt(0, 0);
    header.command = RBCP_CMD_WR;
    header.length = 1;
    header.address = qToBigEndian(ui.address->text().toUInt(0, 0));
    unsigned char data = ui.data->text().toInt(0, 0);
    char buffer[UDP_BUF_SIZE];
    rbcp_com1(ipaddr.data(), port, &header, &data, buffer);
}

void MainWindow::on_read_clicked()
{
    QByteArray ipaddr = ui.ipaddr->text().toUtf8();
    int port = ui.port->text().toInt(0, 0);
    header.command = RBCP_CMD_RD;
    header.length = ui.length->text().toInt(0, 0);
    header.address = qToBigEndian(ui.address->text().toUInt(0, 0));
    char buffer[UDP_BUF_SIZE];
    rbcp_com1(ipaddr.data(), port, &header, NULL, buffer);
}

void MainWindow::log(QString s)
{
    ui.output->moveCursor(QTextCursor::End);
    ui.output->insertPlainText(s);
}

static inline int construct_packet(char *buffer, struct rbcp_header *header, const void *data)
{
    int length = sizeof(struct rbcp_header);
    memcpy(buffer, header, sizeof(struct rbcp_header));
    header->id++;
    if(header->command == RBCP_CMD_WR) {
        memcpy(buffer + sizeof(struct rbcp_header), data, header->length);
        length += header->length;
    }
    return length;
}

void MainWindow::rbcp_com1(QString ipaddr, unsigned int port, struct rbcp_header *header, const void *data, char *buffer)
{
    int length, recv_length;
    char logbuf[100];
    length = construct_packet(buffer, header, data);
    for(int i = 0; i < length; i++) {
        if(i % 4 == 0)
            sprintf(logbuf, "[%.3x]:%.2x ", i, (unsigned char)buffer[i]);
        else if(i % 4 == 3)
            sprintf(logbuf, "%.2x\n", (unsigned char)buffer[i]);
        else
            sprintf(logbuf, "%.2x ", (unsigned char)buffer[i]);
        log(QString(logbuf));
    }
    log(QString("\n"));
    sock.writeDatagram(buffer, length, QHostAddress(ipaddr), port);
    log(QString("The packet have been sent!\nWait to receive the ACK packet...\n"));
    QTime t;
    for(int i = 0; i < 3; i++) {
        QTime t;
        t.start();
        while(t.elapsed() < 1000) {
            QCoreApplication::processEvents();
            recv_length = sock.readDatagram(buffer, UDP_BUF_SIZE);
            if(recv_length != -1)
                break;
        }
        if(recv_length != -1)
            break;
        else {
            log(QString("***** Timeout ! *****\n"));
            length = construct_packet(buffer, header, data);
            sock.writeDatagram(buffer, length, QHostAddress(ipaddr), port);
        }
    }
    if(recv_length == -1)
        return;
    if(recv_length < sizeof(struct rbcp_header)) {
        log(QString("ERROR: ACK packet is too short\n"));
        return;
    }
    log(QString("***** A pacekt is received ! *****.\nReceived data:\n"));
    for(int i = 0; i < recv_length; i++) {
        if(i % 4 == 0)
            sprintf(logbuf, "[%.3x]:%.2x ", i, (unsigned char)buffer[i]);
        else if(i % 4 == 3)
            sprintf(logbuf, "%.2x\n", (unsigned char)buffer[i]);
        else
            sprintf(logbuf, "%.2x ", (unsigned char)buffer[i]);
        log(QString(logbuf));
    }
    log(QString("\n"));
    if(header->command == RBCP_CMD_RD)
        for(int i = 8; i < recv_length; i++) {
            if(i % 8 == 0)
                sprintf(logbuf, "[0x%.8x] %.2x ", qFromBigEndian(header->address) + i - 8, (unsigned char)buffer[i]);
            else if(i % 8 == 7)
                sprintf(logbuf, "%.2x\n", (unsigned char)buffer[i]);
            else if(i % 8 == 3)
                sprintf(logbuf, "%.2x - ", (unsigned char)buffer[i]);
            else
                sprintf(logbuf, "%.2x ", (unsigned char)buffer[i]);
            log(QString(logbuf));
        }
    else {
        sprintf(logbuf, "0x%x: OK", qFromBigEndian(header->address));
        log(QString(logbuf));
    }
    log(QString("\n"));
}
