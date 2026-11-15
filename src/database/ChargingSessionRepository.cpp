
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


// =====================================
// START CHARGING SESSION
// =====================================

bool ChargingSessionRepository::startSession(int bookingId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.transaction()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. Get booking information
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

        m_lastError =
            "This booking cannot start a charging session.";

        db.rollback();
        return false;
    }

    // 2. Check whether session already exists
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
        m_lastError =
            "A session already exists for this booking.";

        db.rollback();
        return false;
    }

    // 3. Check charging point availability
    query.prepare(
        "SELECT status FROM charging_points "
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

    QString pointStatus = query.value(0).toString();

    if (pointStatus != "AVAILABLE") {
        m_lastError =
            "This charging point is not available.";

        db.rollback();
        return false;
    }

    // 4. Check for another active session on this point
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
        m_lastError =
            "This charging point is already in use.";

        db.rollback();
        return false;
    }

    // 5. Reserve the charging point
    query.prepare(
        "UPDATE charging_points "
        "SET status = 'OCCUPIED' "
        "WHERE point_id = :point "
        "AND status = 'AVAILABLE'"
    );

    query.bindValue(":point", pointId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to reserve charging point."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 6. Insert charging session
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

    // 7. Confirm booking
    query.prepare(
        "UPDATE bookings "
        "SET status = 'CONFIRMED' "
        "WHERE booking_id = :booking"
    );

    query.bindValue(":booking", bookingId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to confirm booking."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 8. Commit all changes
    if (!db.commit()) {
        m_lastError = db.lastError().text();
        db.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}


// =====================================
// COMPLETE CHARGING SESSION
// =====================================

bool ChargingSessionRepository::completeSession(
    int sessionId,
    double energyConsumed)
{
    if (energyConsumed < 0) {
        m_lastError =
            "Energy consumed cannot be negative.";
        return false;
    }

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.transaction()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. Get session information
    query.prepare(
        "SELECT point_id, booking_id, status "
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
    int bookingId = query.value(1).toInt();

    QString status = query.value(2).toString();

    if (status != "IN_PROGRESS") {
        m_lastError =
            "Only an in-progress session can be completed.";

        db.rollback();
        return false;
    }

    // 2. Get charging point price
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

    // 3. Complete charging session
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

    // 4. Complete booking
    query.prepare(
        "UPDATE bookings "
        "SET status = 'COMPLETED' "
        "WHERE booking_id = :booking"
    );

    query.bindValue(":booking", bookingId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to complete booking."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 5. Release charging point
    query.prepare(
        "UPDATE charging_points "
        "SET status = 'AVAILABLE' "
        "WHERE point_id = :point"
    );

    query.bindValue(":point", pointId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to release charging point."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 6. Commit transaction
    if (!db.commit()) {
        m_lastError = db.lastError().text();
        db.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}


// =====================================
// CANCEL CHARGING SESSION
// =====================================

bool ChargingSessionRepository::cancelSession(int sessionId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.transaction()) {
        m_lastError = db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. Get session information
    query.prepare(
        "SELECT point_id, booking_id, status "
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
    int bookingId = query.value(1).toInt();

    QString status = query.value(2).toString();

    if (status != "IN_PROGRESS") {
        m_lastError =
            "Only an in-progress session can be cancelled.";

        db.rollback();
        return false;
    }

    // 2. Cancel charging session
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

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to cancel charging session."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 3. Cancel booking
    query.prepare(
        "UPDATE bookings "
        "SET status = 'CANCELLED' "
        "WHERE booking_id = :booking"
    );

    query.bindValue(":booking", bookingId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to cancel booking."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 4. Release charging point
    query.prepare(
        "UPDATE charging_points "
        "SET status = 'AVAILABLE' "
        "WHERE point_id = :point"
    );

    query.bindValue(":point", pointId);

    if (!query.exec() || query.numRowsAffected() != 1) {
        m_lastError = query.lastError().text().isEmpty()
            ? "Unable to release charging point."
            : query.lastError().text();

        db.rollback();
        return false;
    }

    // 5. Commit transaction
    if (!db.commit()) {
        m_lastError = db.lastError().text();
        db.rollback();
        return false;
    }

    m_lastError.clear();
    return true;
}


// =====================================
// LAST ERROR
// =====================================

QString ChargingSessionRepository::lastError() const
{
    return m_lastError;
}
