
#include "VehicleRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

QList<Vehicle> VehicleRepository::getAllVehicles()
{
    QList<Vehicle> vehicles;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT vehicle_id, owner_name, registration_number, "
        "brand, model, battery_capacity, connector_type, status, customer_id "
        "FROM vehicles ORDER BY vehicle_id DESC")) {

        m_lastError = query.lastError().text();
        return vehicles;
    }

    while (query.next()) {
        Vehicle vehicle;

        vehicle.vehicleId = query.value(0).toInt();
        vehicle.ownerName = query.value(1).toString();
        vehicle.registrationNumber = query.value(2).toString();
        vehicle.brand = query.value(3).toString();
        vehicle.model = query.value(4).toString();
        vehicle.batteryCapacity = query.value(5).toDouble();
        vehicle.connectorType = query.value(6).toString();
        vehicle.status = query.value(7).toString();
        vehicle.customerId = query.value(8).toInt();

        vehicles.append(vehicle);
    }

    m_lastError.clear();
    return vehicles;
}


bool VehicleRepository::addVehicle(const Vehicle &vehicle)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO vehicles "
        "(owner_name, registration_number, brand, model, "
        "battery_capacity, connector_type, status, customer_id) "
        "VALUES (:owner, :reg, :brand, :model, "
        ":battery, :connector, :status, :customer)"
    );

    query.bindValue(":owner", vehicle.ownerName);
    query.bindValue(":reg", vehicle.registrationNumber);
    query.bindValue(":brand", vehicle.brand);
    query.bindValue(":model", vehicle.model);
    query.bindValue(":battery", vehicle.batteryCapacity);
    query.bindValue(":connector", vehicle.connectorType);
    query.bindValue(":status", vehicle.status);
    query.bindValue(":customer", vehicle.customerId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool VehicleRepository::updateVehicle(const Vehicle &vehicle)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "UPDATE vehicles SET "
        "owner_name = :owner, "
        "registration_number = :reg, "
        "brand = :brand, "
        "model = :model, "
        "battery_capacity = :battery, "
        "connector_type = :connector, "
        "status = :status, "
        "customer_id = :customer "
        "WHERE vehicle_id = :id"
    );

    query.bindValue(":owner", vehicle.ownerName);
    query.bindValue(":reg", vehicle.registrationNumber);
    query.bindValue(":brand", vehicle.brand);
    query.bindValue(":model", vehicle.model);
    query.bindValue(":battery", vehicle.batteryCapacity);
    query.bindValue(":connector", vehicle.connectorType);
    query.bindValue(":status", vehicle.status);
    query.bindValue(":customer", vehicle.customerId);
    query.bindValue(":id", vehicle.vehicleId);
    

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool VehicleRepository::deleteVehicle(int vehicleId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "DELETE FROM vehicles WHERE vehicle_id = :id"
    );

    query.bindValue(":id", vehicleId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


QString VehicleRepository::lastError() const
{
    return m_lastError;
}
