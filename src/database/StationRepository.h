
#ifndef STATIONREPOSITORY_H
#define STATIONREPOSITORY_H

#include "../models/Station.h"

#include <QList>
#include <QString>

class StationRepository
{
public:
    QList<Station> getAllStations();

    bool addStation(const Station &station);

    bool updateStation(const Station &station);

    bool deleteStation(int stationId);

    QString lastError() const;

private:
    QString errorMessage;
};

#endif
