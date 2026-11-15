#ifndef CHARGINGSESSION_H
#define CHARGINGSESSION_H

#include <QString>
#include <QDateTime>

struct ChargingSession
{
    int sessionId = 0;

    int bookingId = 0;
    int customerId = 0;
    int vehicleId = 0;
    int chargingPointId = 0;

    QDateTime startTime;
    QDateTime endTime;

    double energyConsumed = 0.0;
    double cost = 0.0;

    QString status = "IN_PROGRESS";
};

#endif