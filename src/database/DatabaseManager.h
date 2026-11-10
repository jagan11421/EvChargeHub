
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>

class DatabaseManager
{
public:
    static bool initialize(QString *error = nullptr);

    static QString databasePath();
};

#endif
