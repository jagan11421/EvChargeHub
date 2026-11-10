

#include "ChargingPointRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

QList<ChargingPoint>
ChargingPointRepository::getAllPoints()
{
    QList<ChargingPoint> points;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    QString sql = R"(
        SELECT
            cp.point_id,
            cp.station_id,
            s.name,
            cp.charger_type,
            cp.power_kw,
            cp.price_per_kwh,
            cp.status

        FROM charging_points cp

        JOIN stations s
        ON cp.station_id = s.station_id

        ORDER BY cp.point_id
    )";

    if (!query.exec(sql))
    {
        errorMessage = query.lastError().text();
        return points;
    }

    while (query.next())
    {
        ChargingPoint point;

        point.pointId = query.value(0).toInt();
        point.stationId = query.value(1).toInt();
        point.stationName = query.value(2).toString();
        point.chargerType = query.value(3).toString();
        point.powerKW = query.value(4).toDouble();
        point.pricePerKwh = query.value(5).toDouble();
        point.status = query.value(6).toString();

        points.append(point);
    }

    errorMessage.clear();

    return points;
}

bool ChargingPointRepository::addPoint(
    const ChargingPoint &point
)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(R"(
        INSERT INTO charging_points
        (
            station_id,
            charger_type,
            power_kw,
            price_per_kwh,
            status
        )
        VALUES (?, ?, ?, ?, ?)
    )");

    query.addBindValue(point.stationId);
    query.addBindValue(point.chargerType);
    query.addBindValue(point.powerKW);
    query.addBindValue(point.pricePerKwh);
    query.addBindValue(point.status);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();

    return true;
}

bool ChargingPointRepository::updatePoint(
    const ChargingPoint &point
)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(R"(
        UPDATE charging_points

        SET station_id = ?,
            charger_type = ?,
            power_kw = ?,
            price_per_kwh = ?,
            status = ?

        WHERE point_id = ?
    )");

    query.addBindValue(point.stationId);
    query.addBindValue(point.chargerType);
    query.addBindValue(point.powerKW);
    query.addBindValue(point.pricePerKwh);
    query.addBindValue(point.status);
    query.addBindValue(point.pointId);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();

    return true;
}

bool ChargingPointRepository::deletePoint(int pointId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "DELETE FROM charging_points WHERE point_id = ?"
    );

    query.addBindValue(pointId);

    if (!query.exec())
    {
        errorMessage = query.lastError().text();
        return false;
    }

    errorMessage.clear();

    return true;
}

QString ChargingPointRepository::lastError() const
{
    return errorMessage;
}
