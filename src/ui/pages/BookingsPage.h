#ifndef BOOKINGSPAGE_H
#define BOOKINGSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "../../database/BookingRepository.h"
#include "../../database/CustomerRepository.h"
#include "../../database/VehicleRepository.h"
#include "../../database/ChargingPointRepository.h"

class BookingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit BookingsPage(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QLineEdit *searchInput;

    BookingRepository repository;
    CustomerRepository customerRepository;
    VehicleRepository vehicleRepository;
    ChargingPointRepository pointRepository;

    void setupUI();
    void loadBookings();
    void addBooking();
    void editBooking();
    void cancelBooking();
    void searchBookings();
};

#endif