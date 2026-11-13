
#ifndef VEHICLEREPOSITORY_H
#define VEHICLEREPOSITORY_H

#include <QList>
#include <QString>
#include "../models/Vehicle.h"

class VehicleRepository
{
public:
    QList<Vehicle> getAllVehicles();

    bool addVehicle(const Vehicle &vehicle);
    bool updateVehicle(const Vehicle &vehicle);
    bool deleteVehicle(int vehicleId);

    QString lastError() const;

private:
    QString m_lastError;
};

#endif
