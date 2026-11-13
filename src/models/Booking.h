#ifndef BOOKING_H
#define BOOKING_H

#include <QString>
#include <QDateTime>

struct Booking
{
    int bookingId = 0;

    int customerId = 0;
    int vehicleId = 0;
    int chargingPointId = 0;

    QDateTime startTime;
    QDateTime endTime;

    QString status = "PENDING";
};

#endif