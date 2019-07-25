/*
配置管理类 单实例类
*/
#ifndef CCONFIGMANAGER_H
#define CCONFIGMANAGER_H

#include <QString>

class CConfigManager
{
private:
    CConfigManager();
    static CConfigManager* Instance;
public:
    ~CConfigManager();
    static CConfigManager* getInstance();
    QString csvRight = "";
    QString calibxmlRight = "";
    QString csvLeft = "";
    QString calibxmlLeft = "";
};

#endif // CCONFIGMANAGER_H
