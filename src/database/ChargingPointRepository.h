
#ifndef CHARGINGPOINTREPOSITORY_H
#define CHARGINGPOINTREPOSITORY_H

#include "../models/ChargingPoint.h"

#include <QList>
#include <QString>

class ChargingPointRepository
{
public:
    QList<ChargingPoint> getAllPoints();

    bool addPoint(const ChargingPoint &point);

    bool updatePoint(const ChargingPoint &point);

    bool deletePoint(int pointId);

    QString lastError() const;

private:
    QString errorMessage;
};

#endif
