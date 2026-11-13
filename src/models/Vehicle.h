#ifndef VEHICLE_H
#define VEHICLE_H

#include <QString>

struct Vehicle {
    int vehicleId = 0;
    int customerId = 0;

    QString ownerName;
    QString registrationNumber;
    QString brand;
    QString model;

    double batteryCapacity = 0.0;

    QString connectorType;
    QString status = "ACTIVE";
};

#endif