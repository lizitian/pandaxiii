#ifndef RBCP_H
#define RBCP_H
#include "ui_rbcp.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
private:
    Ui::MainWindow ui;
};
#endif
