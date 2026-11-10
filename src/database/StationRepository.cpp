
#include "StationRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

QList<Station> StationRepository::getAllStations()
{
    QList<Station> stations;

    QSqlDatabase db =
        QSqlDatabase::database(
            "evchargehub_connection"
        );

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT station_id, name, address, "
        "total_power, status FROM stations"
    ))
    {
        errorMessage = query.lastError().text();
        return stations;
    }

    while (query.next())
    {
        Station station;

        station.stationId = query.value(0).toInt();
        station.name = query.value(1).toString();
        station.address = query.value(2).toString();
        station.totalPower = query.value(3).toDouble();
        station.status = query.value(4).toString();

        stations.append(station);
    }

    errorMessage.clear();

    return stations;
}

bool StationRepository::addStation(
    const Station &station
)
{
    QSqlDatabase db =
        QSqlDatabase::database(
            "evchargehub_connection"
        );

    QSqlQuery query(db);

    query.prepare(R"(
        INSERT INTO stations
        (name, address, total_power, status)
        VALUES (?, ?, ?, ?)
    )");

    query.addBindValue(station.name);
    query.addBindValue(station.address);
    query.addBindValue(station.totalPower);
    query.addBindValue(station.status);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();
    return true;
}

bool StationRepository::updateStation(
    const Station &station
)
{
    QSqlDatabase db =
        QSqlDatabase::database(
            "evchargehub_connection"
        );

    QSqlQuery query(db);

    query.prepare(R"(
        UPDATE stations
        SET name = ?,
            address = ?,
            total_power = ?,
            status = ?
        WHERE station_id = ?
    )");

    query.addBindValue(station.name);
    query.addBindValue(station.address);
    query.addBindValue(station.totalPower);
    query.addBindValue(station.status);
    query.addBindValue(station.stationId);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();
    return true;
}

bool StationRepository::deleteStation(int stationId)
{
    QSqlDatabase db =
        QSqlDatabase::database(
            "evchargehub_connection"
        );

    QSqlQuery query(db);

    query.prepare(
        "DELETE FROM stations WHERE station_id = ?"
    );

    query.addBindValue(stationId);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();
    return true;
}

QString StationRepository::lastError() const
{
    return errorMessage;
}
