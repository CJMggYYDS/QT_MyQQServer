#include "tcpclientsocket.h"

TcpClientSocket::TcpClientSocket()
{
    connect(this,SIGNAL(readyRead()),this,SLOT(dateReceived()));
    connect(this,SIGNAL(disconnected()),this,SLOT(slotdisconnect()));
}

void TcpClientSocket::dateReceived(){
    while(bytesAvailable()>0){
        QByteArray datagram;
        datagram=readAll();
        emit updateClient(datagram,this->location);
    }
}

void TcpClientSocket::slotdisconnect(){
    emit disconnected(this->location);
}




