
#ifndef CHARGINGPOINT_H
#define CHARGINGPOINT_H

#include <QString>

struct ChargingPoint
{
    int pointId = 0;
    int stationId = 0;

    QString stationName;
    QString chargerType; // AC or DC

    double powerKW = 0.0;
    double pricePerKwh = 0.0;

    QString status = "AVAILABLE";
};

#endif
