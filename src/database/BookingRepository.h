#ifndef BOOKINGREPOSITORY_H
#define BOOKINGREPOSITORY_H

#include "../models/Booking.h"
#include <QList>

class BookingRepository
{
public:
    QList<Booking> getAllBookings();

    bool addBooking(const Booking &booking);
    bool updateBooking(const Booking &booking);
    bool cancelBooking(int bookingId);
    bool isTimeSlotAvailable(
        int pointId,
        const QDateTime &startTime,
        const QDateTime &endTime,
        int excludeBookingId = 0
    );

    QString lastError() const;

private:
    QString m_lastError;
};

#endif