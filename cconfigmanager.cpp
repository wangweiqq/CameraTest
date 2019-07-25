#pragma execution_character_set("utf-8")
#include <QDebug>
#include <QSettings>
#include "cconfigmanager.h"
CConfigManager* CConfigManager::Instance = NULL;
CConfigManager::CConfigManager()
{
    QSettings setting("./config.ini",QSettings::IniFormat);
    csvRight = setting.value("Main/csvRight").toString();
    calibxmlRight = setting.value("Main/calibxmlRight").toString();
    csvLeft = setting.value("Main/csvLeft").toString();
    calibxmlLeft = setting.value("Main/calibxmlLeft").toString();
    qDebug()<<setting.fileName();
    qDebug() << "csvRight = " << csvRight;
    qDebug() << "calibxmlRight = " << calibxmlRight;
    qDebug() << "csvLeft = " << csvLeft;
    qDebug() << "calibxmlLeft = " << calibxmlLeft;
//	qDebug() << "mDbIp : " << mDbIp.toStdString().c_str() << "mDbPort : " << mDbPort<<"mDbUser : "<< mDbUser.toStdString().c_str()<<" mDbPwd : "<< mDbPwd.toStdString().c_str()<<" mDbName : "<< mDbName.toStdString().c_str();
}
CConfigManager::~CConfigManager(){

}
CConfigManager* CConfigManager::getInstance(){
    if(Instance){
        return Instance;
    }
    Instance = new CConfigManager();
    return Instance;
}

