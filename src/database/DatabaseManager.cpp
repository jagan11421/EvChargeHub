
#include "DatabaseManager.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>


QString DatabaseManager::databasePath()
{
    QString directory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
        );

    return directory + "/evchargehub.db";
}

bool DatabaseManager::initialize(QString *error)
{
    QString directory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppDataLocation
        );

    if (!QDir().mkpath(directory))
    {
        if (error)
            *error = "Unable to create database directory.";

        return false;
    }

    QSqlDatabase db;

    if (QSqlDatabase::contains("evchargehub_connection"))
    {
        db = QSqlDatabase::database(
            "evchargehub_connection"
        );
    }
    else
    {
        db = QSqlDatabase::addDatabase(
            "QSQLITE",
            "evchargehub_connection"
        );
    }

    db.setDatabaseName(databasePath());

    if (!db.open())
    {
        if (error)
            *error = db.lastError().text();

        return false;
    }

    QSqlQuery query(db);

    if (!query.exec("PRAGMA foreign_keys = ON"))
    {
        if (error)
            *error = query.lastError().text();

        return false;
    }

    // Create stations table
    QString createStations = R"(
        CREATE TABLE IF NOT EXISTS stations (
            station_id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            address TEXT NOT NULL,
            total_power REAL NOT NULL CHECK(total_power > 0),
            status TEXT NOT NULL DEFAULT 'ACTIVE'
                CHECK(status IN ('ACTIVE', 'INACTIVE'))
        )
    )";

    if (!query.exec(createStations))
    {
        if (error)
            *error = query.lastError().text();

        return false;
    }

    // Create charging points table
    QString createChargingPoints = R"(
        CREATE TABLE IF NOT EXISTS charging_points (
            point_id INTEGER PRIMARY KEY AUTOINCREMENT,

            station_id INTEGER NOT NULL,

            charger_type TEXT NOT NULL
                CHECK(charger_type IN ('AC', 'DC')),

            power_kw REAL NOT NULL
                CHECK(power_kw > 0),

            price_per_kwh REAL NOT NULL
                CHECK(price_per_kwh >= 0),

            status TEXT NOT NULL DEFAULT 'AVAILABLE'
                CHECK(status IN (
                    'AVAILABLE',
                    'OCCUPIED',
                    'MAINTENANCE',
                    'INACTIVE'
                )),

            FOREIGN KEY(station_id)
                REFERENCES stations(station_id)
                ON DELETE RESTRICT
        )
    )";

    if (!query.exec(createChargingPoints))
    {
        if (error)
            *error = query.lastError().text();

        return false;
    }
    QSqlQuery vehicleQuery(db);
    if (!vehicleQuery.exec(R"(
        CREATE TABLE IF NOT EXISTS vehicles (
            vehicle_id INTEGER PRIMARY KEY AUTOINCREMENT,
            owner_name TEXT NOT NULL,
            registration_number TEXT NOT NULL UNIQUE,
            brand TEXT NOT NULL,
            model TEXT NOT NULL,
            battery_capacity REAL NOT NULL
                CHECK(battery_capacity > 0),
            connector_type TEXT NOT NULL
                CHECK(connector_type IN ('TYPE1', 'TYPE2', 'CCS', 'CHADEMO'))
        )
    )")) {
        if (error)
            *error = vehicleQuery.lastError().text();
        return false;
    }
    if (!query.exec(R"(
        CREATE TABLE IF NOT EXISTS vehicles (
            vehicle_id INTEGER PRIMARY KEY AUTOINCREMENT,
            registration_number TEXT NOT NULL UNIQUE,
            brand TEXT NOT NULL,
            model TEXT NOT NULL,
            battery_capacity REAL NOT NULL
                CHECK(battery_capacity > 0),
            connector_type TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT 'ACTIVE'
                CHECK(status IN ('ACTIVE', 'INACTIVE'))
        )
    )")) {
        if (error) {
            *error = query.lastError().text();
        }
        return false;
    }
    // Migration: Add status column to existing vehicles table

    bool hasStatusColumn = false;

    if (query.exec("PRAGMA table_info(vehicles)")) {

        while (query.next()) {
            if (query.value(1).toString() == "status") {
                hasStatusColumn = true;
                break;
            }
        }

    } else {
        if (error) {
            *error = query.lastError().text();
        }
        return false;
    }

    if (!hasStatusColumn) {

        if (!query.exec(
            "ALTER TABLE vehicles "
            "ADD COLUMN status TEXT NOT NULL DEFAULT 'ACTIVE' "
            "CHECK(status IN ('ACTIVE', 'INACTIVE'))"
        )) {
            if (error) {
                *error = query.lastError().text();
            }
            return false;
        }
    }
    return true;
}
