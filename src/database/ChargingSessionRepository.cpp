#include "ChargingSessionRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>

QList<ChargingSession>
ChargingSessionRepository::getAllSessions()
{
    QList<ChargingSession> sessions;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT session_id, booking_id, customer_id, "
        "vehicle_id, point_id, start_time, end_time, "
        "energy_consumed, cost, status "
        "FROM charging_sessions "
        "ORDER BY session_id DESC")) {

        m_lastError = query.lastError().text();
        return sessions;
    }

    while (query.next()) {
        ChargingSession session;

        session.sessionId = query.value(0).toInt();
        session.bookingId = query.value(1).toInt();
        session.customerId = query.value(2).toInt();
        session.vehicleId = query.value(3).toInt();
        session.chargingPointId = query.value(4).toInt();

        session.startTime = QDateTime::fromString(
            query.value(5).toString(), Qt::ISODate
        );

        if (!query.value(6).isNull()) {
            session.endTime = QDateTime::fromString(
                query.value(6).toString(), Qt::ISODate
            );
        }

        session.energyConsumed = query.value(7).toDouble();
        session.cost = query.value(8).toDouble();
        session.status = query.value(9).toString();

        sessions.append(session);
    }

    m_lastError.clear();
    return sessions;
}


bool ChargingSessionRepository::startSession(int bookingId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!db.transaction()) {
        m_lastError = db.lastError().text();
        return false;
    }

    query.prepare(
        "SELECT customer_id, vehicle_id, point_id, status "
        "FROM bookings "
        "WHERE booking_id = :id"
    );

    query.bindValue(":id", bookingId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Booking not found."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    int customerId = query.value(0).toInt();
    int vehicleId = query.value(1).toInt();
    int pointId = query.value(2).toInt();
    QString bookingStatus = query.value(3).toString();

    if (bookingStatus != "PENDING" &&
        bookingStatus != "CONFIRMED") {

        m_lastError = "This booking cannot start a charging session.";
        db.rollback();
        return false;
    }

    query.prepare(
        "SELECT COUNT(*) FROM charging_sessions "
        "WHERE booking_id = :booking"
    );

    query.bindValue(":booking", bookingId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        db.rollback();
        return false;
    }

    if (query.value(0).toInt() > 0) {
        m_lastError = "A session already exists for this booking.";
        db.rollback();
        return false;
    }

    query.prepare(
        "SELECT COUNT(*) FROM charging_sessions "
        "WHERE point_id = :point "
        "AND status = 'IN_PROGRESS'"
    );

    query.bindValue(":point", pointId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text();
        db.rollback();
        return false;
    }

    if (query.value(0).toInt() > 0) {
        m_lastError = "This charging point is already in use.";
        db.rollback();
        return false;
    }

    query.prepare(
        "INSERT INTO charging_sessions "
        "(booking_id, customer_id, vehicle_id, point_id, "
        "start_time, status) "
        "VALUES (:booking, :customer, :vehicle, :point, "
        ":start, 'IN_PROGRESS')"
    );

    query.bindValue(":booking", bookingId);
    query.bindValue(":customer", customerId);
    query.bindValue(":vehicle", vehicleId);
    query.bindValue(":point", pointId);
    query.bindValue(
        ":start",
        QDateTime::currentDateTime().toString(Qt::ISODate)
    );

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        m_lastError = db.lastError().text();
        db.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool ChargingSessionRepository::completeSession(
    int sessionId,
    double energyConsumed)
{
    if (energyConsumed < 0) {
        m_lastError = "Energy consumed cannot be negative.";
        return false;
    }

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.transaction()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    query.prepare(
        "SELECT point_id, status "
        "FROM charging_sessions "
        "WHERE session_id = :id"
    );

    query.bindValue(":id", sessionId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Charging session not found."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    int pointId = query.value(0).toInt();
    QString status = query.value(1).toString();

    if (status != "IN_PROGRESS") {
        m_lastError = "Only an in-progress session can be completed.";
        db.rollback();
        return false;
    }

    query.prepare(
        "SELECT price_per_kwh "
        "FROM charging_points "
        "WHERE point_id = :point"
    );

    query.bindValue(":point", pointId);

    if (!query.exec() || !query.next()) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Charging point not found."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    double pricePerKwh = query.value(0).toDouble();
    double totalCost = energyConsumed * pricePerKwh;

    query.prepare(
        "UPDATE charging_sessions SET "
        "end_time = :end, "
        "energy_consumed = :energy, "
        "cost = :cost, "
        "status = 'COMPLETED' "
        "WHERE session_id = :id "
        "AND status = 'IN_PROGRESS'"
    );

    query.bindValue(
        ":end",
        QDateTime::currentDateTime().toString(Qt::ISODate)
    );

    query.bindValue(":energy", energyConsumed);
    query.bindValue(":cost", totalCost);
    query.bindValue(":id", sessionId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to complete charging session."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    if (!db.commit()) {
        m_lastError = db.lastError().text();
        db.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool ChargingSessionRepository::cancelSession(int sessionId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "UPDATE charging_sessions "
        "SET status = 'CANCELLED', "
        "end_time = :end "
        "WHERE session_id = :id "
        "AND status = 'IN_PROGRESS'"
    );

    query.bindValue(
        ":end",
        QDateTime::currentDateTime().toString(Qt::ISODate)
    );

    query.bindValue(":id", sessionId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() != 1) {
        m_lastError = "Session not found or is no longer in progress.";
        return false;
    }

    m_lastError.clear();
    return true;
}


QString ChargingSessionRepository::lastError() const
{
    return m_lastError;
}