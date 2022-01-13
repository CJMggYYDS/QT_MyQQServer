#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H

#include <QObject>
#include<QTcpSocket>

class TcpClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
    TcpClientSocket();
    int location;             //记录server中套接字链表中当前套接字对象的下标
signals:
    void updateClient(QByteArray,int);  //发来信号处理
    void disconnected(int);              //连接断开

private slots:
    void dateReceived();       //数据接收
    void slotdisconnect();     //断开连接
};

#endif // TCPCLIENTSOCKET_H
