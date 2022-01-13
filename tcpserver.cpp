#include "tcpserver.h"
#include "ui_tcpserver.h"
#include<QtSql>

tcpserver::tcpserver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tcpserver)
{
    ui->setupUi(this);
    this->setWindowTitle(" My QQChart Server");
    //创建数据库
    const char* myQQDBFilePath = "MyQQChart.db";
    const char* CreateUserInfoTable = "create table user_info(username text, passwd text, state text, ipAddress text, port text);";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(myQQDBFilePath);
    bool bRet = db.open();
    if (!bRet){
             printf("QtSql driver <QSQLITE> cannot open %s\n", myQQDBFilePath);
             return ;
     }
     QSqlQuery query;
     query.exec(CreateUserInfoTable);
     server = new Server;


     if(server->listen(QHostAddress::Any,8888))
     {
         ui->textEdit->append("QQChart服务器正在运行！");
     }
}

tcpserver::~tcpserver()
{
    delete ui;
}
