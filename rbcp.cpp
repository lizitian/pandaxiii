#include <QApplication>
#include "rbcp.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.show();
    return app.exec();
}

void MainWindow::on_write_clicked(bool checked)
{
    ui.output->moveCursor(QTextCursor::End);
    ui.output->insertPlainText("write clicked\n");
}

void MainWindow::on_read_clicked(bool checked)
{
    ui.output->moveCursor(QTextCursor::End);
    ui.output->insertPlainText("read clicked\n");
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);
}
