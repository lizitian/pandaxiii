#include "rbcp.h"
static struct rbcp_header header = {
    .type = RBCP_VER,
    .command = 0,
    .id = 0,
    .length = 0,
    .address = 0
};
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

void MainWindow::on_write_clicked(bool checked)
{
    QByteArray ipaddr = ui.ipaddr->text().toUtf8();
    int port = ui.port->text().toInt(0, 0);
    header.command = RBCP_CMD_WR;
    header.length = 1;
    header.address = qToBigEndian(ui.address->text().toUInt(0, 0));
    unsigned char data = ui.data->text().toInt(0, 0);
    char buffer[UDP_BUF_SIZE];
    rbcp_com(ipaddr.data(), port, &header, &data, buffer);
}

void MainWindow::on_read_clicked(bool checked)
{
    QByteArray ipaddr = ui.ipaddr->text().toUtf8();
    int port = ui.port->text().toInt(0, 0);
    header.command = RBCP_CMD_RD;
    header.length = ui.length->text().toInt(0, 0);
    header.address = qToBigEndian(ui.address->text().toUInt(0, 0));
    char buffer[UDP_BUF_SIZE];
    rbcp_com(ipaddr.data(), port, &header, NULL, buffer);
}

void MainWindow::log(QString s)
{
    ui.output->moveCursor(QTextCursor::End);
    ui.output->insertPlainText(s);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
}
