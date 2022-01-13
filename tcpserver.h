#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include"server.h"


namespace Ui {
class tcpserver;
}

class tcpserver : public QWidget
{
    Q_OBJECT

public:
    explicit tcpserver(QWidget *parent = nullptr);
    ~tcpserver();
    Server* server;

private:
    Ui::tcpserver *ui;

};

#endif // TCPSERVER_H
