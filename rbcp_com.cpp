#include "rbcp.h"
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
void MainWindow::rbcp_com(QString ipaddr, unsigned int port, struct rbcp_header *header, const void *data, char *buffer)
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
                sprintf(logbuf, "[0x%.8x] %.2x ", ntohl(header->address) + i - 8, (unsigned char)buffer[i]);
            else if(i % 8 == 7)
                sprintf(logbuf, "%.2x\n", (unsigned char)buffer[i]);
            else if(i % 8 == 3)
                sprintf(logbuf, "%.2x - ", (unsigned char)buffer[i]);
            else
                sprintf(logbuf, "%.2x ", (unsigned char)buffer[i]);
            log(QString(logbuf));
        }
    else {
        sprintf(logbuf, "0x%x: OK", ntohl(header->address));
        log(QString(logbuf));
    }
    log(QString("\n"));
}
