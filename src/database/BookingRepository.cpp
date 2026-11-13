#include "BookingRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

QList<Booking> BookingRepository::getAllBookings()
{
    QList<Booking> bookings;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT booking_id, customer_id, vehicle_id, point_id, "
        "start_time, end_time, status "
        "FROM bookings ORDER BY booking_id DESC")) {

        m_lastError = query.lastError().text();
        return bookings;
    }

    while (query.next()) {
        Booking booking;

        booking.bookingId = query.value(0).toInt();
        booking.customerId = query.value(1).toInt();
        booking.vehicleId = query.value(2).toInt();
        booking.chargingPointId = query.value(3).toInt();

        booking.startTime = QDateTime::fromString(
            query.value(4).toString(), Qt::ISODate);

        booking.endTime = QDateTime::fromString(
            query.value(5).toString(), Qt::ISODate);

        booking.status = query.value(6).toString();

        bookings.append(booking);
    }

    m_lastError.clear();
    return bookings;
}


bool BookingRepository::addBooking(const Booking &booking)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO bookings "
        "(customer_id, vehicle_id, point_id, "
        "start_time, end_time, status) "
        "VALUES (:customer, :vehicle, :point, "
        ":start, :end, :status)"
    );

    query.bindValue(":customer", booking.customerId);
    query.bindValue(":vehicle", booking.vehicleId);
    query.bindValue(":point", booking.chargingPointId);

    query.bindValue(
        ":start", booking.startTime.toString(Qt::ISODate));

    query.bindValue(
        ":end", booking.endTime.toString(Qt::ISODate));

    query.bindValue(":status", booking.status);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool BookingRepository::updateBooking(const Booking &booking)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "UPDATE bookings SET "
        "customer_id = :customer, "
        "vehicle_id = :vehicle, "
        "point_id = :point, "
        "start_time = :start, "
        "end_time = :end, "
        "status = :status "
        "WHERE booking_id = :id"
    );

    query.bindValue(":customer", booking.customerId);
    query.bindValue(":vehicle", booking.vehicleId);
    query.bindValue(":point", booking.chargingPointId);

    query.bindValue(
        ":start", booking.startTime.toString(Qt::ISODate));

    query.bindValue(
        ":end", booking.endTime.toString(Qt::ISODate));

    query.bindValue(":status", booking.status);
    query.bindValue(":id", booking.bookingId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool BookingRepository::cancelBooking(int bookingId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "UPDATE bookings SET status = 'CANCELLED' "
        "WHERE booking_id = :id"
    );

    query.bindValue(":id", bookingId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

bool BookingRepository::isTimeSlotAvailable(
    int pointId,
    const QDateTime &startTime,
    const QDateTime &endTime,
    int excludeBookingId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "SELECT COUNT(*) FROM bookings "
        "WHERE point_id = :point "
        "AND status != 'CANCELLED' "
        "AND start_time < :end "
        "AND end_time > :start "
        "AND booking_id != :excludeId"
    );

    query.bindValue(":point", pointId);
    query.bindValue(":start", startTime.toString(Qt::ISODate));
    query.bindValue(":end", endTime.toString(Qt::ISODate));
    query.bindValue(":excludeId", excludeBookingId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.next()) {
        int conflictingBookings = query.value(0).toInt();

        m_lastError.clear();
        return conflictingBookings == 0;
    }

    m_lastError = "Unable to check charging point availability.";
    return false;
}

QString BookingRepository::lastError() const
{
    return m_lastError;
}
