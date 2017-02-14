#ifndef RBCP_H
#define RBCP_H
#include "ui_rbcp.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
private slots:
    void on_write_clicked(bool checked);
    void on_read_clicked(bool checked);
private:
    Ui::MainWindow ui;
};
#endif
