
#ifndef STATION_H
#define STATION_H

#include <QString>

struct Station
{
    int stationId = 0;

    QString name;
    QString address;

    double totalPower = 0.0;

    QString status = "ACTIVE";
};

#endif
