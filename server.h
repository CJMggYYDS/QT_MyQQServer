#ifndef SERVER_H
#define SERVER_H

#include<QTcpServer>
#include<QObject>
#include"tcpclientsocket.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    QList<TcpClientSocket*> socketlist;       //套接字链表，存放链接
    QHash<QString,int> usernamelist;          //用户表，存放用户名及其在套接字链表中的位置

protected:
    void incomingConnection(qintptr socketDescriptor);   //处理发送来的连接

public slots:
    void updateClient(QByteArray,int);        //处理接收客户端发来的信息
    void slotDisconnected(int);               //断开连接

private:
    void login(QByteArray,int);  //登录处理

    void regedit(QByteArray,int); //注册处理

    void addfriend(QByteArray);   //添加好友处理

    void message(QByteArray,int);  //发送消息处理

    void quit(QByteArray,int);     //退出

    void online(QByteArray,int);   //上线

    void unline(QByteArray,int);   //离线

    void sendtoall(QByteArray,int); //群发消息
};
#endif // SERVER_H
