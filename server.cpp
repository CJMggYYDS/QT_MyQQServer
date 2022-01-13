#include "server.h"
#include <QDataStream>
#include <QtSql>
#include <QDebug>

Server::Server()
{
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    TcpClientSocket* socket= new TcpClientSocket();
    socket->setSocketDescriptor(socketDescriptor);
    socketlist.append(socket);
    socket->location=socketlist.count()-1;
    connect(socket,SIGNAL(updateClient(QByteArray,int)),this,SLOT(updateClient(QByteArray,int )));
    connect(socket,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));
}

//处理接受的信号信息
void Server::updateClient(QByteArray m,int n)
{
    QDataStream in(&m,QIODevice::ReadOnly);
    quint8 type;
    in>>type;           //客户端发来的信号，根据信号来判断客户端要进行的操作

    switch(type){
    case 'L':login(m,n);break;      //收到'L'信号，服务器进行登录处理
    case 'R':regedit(m,n);break;    //收到'R'信号，服务器进行注册处理
    case 'A':addfriend(m);break;    //收到'A'信号，服务器进行添加好友处理
    case 'M':message(m,n);break;    //收到'M'信号，服务器进行收发信息处理
    case 'Q':quit(m,n);break;       //收到'Q'信号，服务器进行退出处理
    case 'O':online(m,n);break;     //收到'O'信号，服务器进行上线处理
    case 'U':unline(m,n);break;     //收到'U'信号，服务器进行下线处理
    case 'N':sendtoall(m,n);break;  //收到'N'信号，服务器进行群发消息处理
    }
}

//登录
void Server::login(QByteArray m,int n){
    QString username,passwd,ipAddress,port;
    bool bRet;
    quint8 type;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);
    in>>type>>username>>passwd>>ipAddress>>port;
    bRet = query1.exec("select username, passwd from user_info;");
    if (!bRet){
        out  << quint8('D');
        out.device()->seek(0);
         socketlist[n]->write(block);
        return;
    }
    while(query1.next()){
        if(username == query1.value(0).toString()){
            if(passwd == query1.value(1).toString()){
                QString strUpdateSqlFormat("Update user_info Set state = '%1', ipAddress = '%2', port = '%3'"
                                           "where username = '%4';");
                QString strUpdateSql = strUpdateSqlFormat.arg(QString("online"))
                                       .arg(ipAddress).arg(port).arg(username);
                bRet = query1.exec(strUpdateSql);
                if (!bRet){
                    out  << quint8('D');     //D 表示数据库出错
                    out.device()->seek(0);
                    socketlist[n]->write(block);;
                    return;
                }
                out  << quint8('P'); //P 表示登录通过
                out.device()->seek(0);
                socketlist[n]->write(block);
                return;
            }
            else{
                out  << quint8('E'); //E 表示密码错误
                out.device()->seek(0);
                socketlist[n]->write(block);
                return;
            }
        }
    }
    out<< quint8('N')<<username; //N 表示用户名不存在
    out.device()->seek(0);
    socketlist[n]->write(block);
    return;
}

//注册
void Server::regedit(QByteArray m,  int n){
    QString username,passwd,ipAddress,port;
    bool bRet;
    quint8 type;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);
    in>>type>>username>>passwd>>ipAddress>>port;
    bRet = query1.exec("select username from user_info;");
    if (!bRet){
        out << quint8('D');      //‘D’表示数据库错误
        out.device()->seek(0);
        socketlist[n]->write(block);
        return;
    }
    while(query1.next()){
        if(username == query1.value(0).toString()){
            out << quint8('F')<<username;           //‘F’说明用户已存在
            out.device()->seek(0);
            socketlist[n]->write(block);
            return;
        }
    }
    QSqlQuery query2;
    QString friendlist=QString("create table '%1' (username text);").arg(username);
    bRet =query2.exec(friendlist);
    if(!bRet){
        out<<quint8('D');
        out.device()->seek(0);
        socketlist[n]->write(block);
        return;
    }
    QString strInsertSqlFormat("insert into user_info(username, passwd, state, ipAddress, port)"
                                                    " values('%1', '%2', '%3', '%4','%5');" );
    QString strInsertSql = strInsertSqlFormat.arg(username).arg(passwd)
                           .arg(QString("left")).arg(ipAddress).arg(port);
    bRet = query1.exec(strInsertSql);
    if (!bRet){
        out  << quint8('D');         //‘D’表示数据库错误
        out.device()->seek(0);
        socketlist[n]->write(block);
        return;
    }
    out << quint8('S');            //‘S’表示注册成功
    out.device()->seek(0);
    socketlist[n]->write(block);
    return;
}

//添加好友
void Server::addfriend(QByteArray m){
    QString user,username;
    quint8 type;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);
    in>>type>>username>>user;
    QString add=QString("select username from user_info;");
    query1.exec(add);

    while(query1.next()){
        QString insert=QString("insert into '%1' values('%2');").arg(user).arg(username);
        QString insert1=QString("insert into '%1' values('%2');").arg(username).arg(user);

        QSqlQuery query2;
        query2.exec(insert);
        query2.exec(insert1);
    }

}

//发送消息
void Server::message(QByteArray m, int n){
    quint8 type;
    QString msg,msgfrom,msgto,str;
    QDataStream in(&m,QIODevice::ReadOnly);
    in >>type>> msgfrom >> msgto >> msg;
    QHash<QString,int>::const_iterator get;
    get=usernamelist.find(msgto);
    n=get.value();
    socketlist[n]->write(m);
}

//退出
void Server::quit(QByteArray m, int n){
    QString username;
    quint8 type;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);
    in>>type>>username;
    QString strUpdateSqlFormat("update user_info set state = '%1' where username = '%2';");
    QString strUpdateSql = strUpdateSqlFormat.arg(QString("left")).arg(username);
    query1.exec(strUpdateSql);
    usernamelist.remove(username);
    socketlist.removeAt(n);
    for(int i=0;i<socketlist.count();i++){
        socketlist[i]->location-=1;
    }
}

//显示在线好友
void Server::online(QByteArray m, int n){
    quint8 type;
    QString username;
    bool bRet;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);
    in>>type>>username;
    usernamelist.insert(username,n);
    QString online=QString("select username from user_info where state = 'online' and username in (select username from '%1' );").arg(username);
     bRet = query1.exec(online);
      if (!bRet) return;
      out<<quint8('G');
      while( query1.next() ){
              out<<query1.value(0).toString();
      }
      out<<QString("over");
      socketlist[n]->write(block);
}

//显示离线好友
void Server::unline(QByteArray m, int n){
    QString username;
    quint8 type;
    bool bRet;
    QSqlQuery query1;
    QByteArray block;
    QDataStream in(&m,QIODevice::ReadOnly);
    QDataStream out(&block,QIODevice::WriteOnly);

    in>>type>>username;
    QString unline=QString("select username from user_info where state = 'left' and username in (select username from '%1' );").arg(username);
     bRet = query1.exec(unline);
      if (!bRet) return;

      out<<quint8('G');
      while( query1.next() ){
              out<<query1.value(0).toString();
      }
      out<<QString("over");
      socketlist[n]->write(block);
}

//群发消息
void Server::sendtoall(QByteArray m, int n)
{
    QDataStream in(&m,QIODevice::ReadOnly);
    QString username;
    quint8 type;

    in>>type>>username;
    QString select=QString("select username from user_info where state = 'online' and username in (select username from '%1');").arg(username);
    QSqlQuery query1;
    query1.exec(select);

    while(query1.next())
    {
        QHash<QString,int>::const_iterator it;
        it=usernamelist.find(query1.value(0).toString());
        n=it.value();
        socketlist[n]->write(m);

    }
}

//离线
void Server::slotDisconnected(int n){
    socketlist.removeAt(n);
}

