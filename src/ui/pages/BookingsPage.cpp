#include "BookingsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTimeEdit>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QDateTime>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

BookingsPage::BookingsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadBookings();
}

void BookingsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Booking Management");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #ffffff;"
    );

    QLabel *subtitle = new QLabel(
        "Manage EV charging reservations"
    );
    subtitle->setStyleSheet("color: #aaaaaa;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search by customer name...");

    QPushButton *searchButton = new QPushButton("Search");
    QPushButton *refreshButton = new QPushButton("Refresh");

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(refreshButton);

    mainLayout->addLayout(searchLayout);

    table = new QTableWidget();
    table->setColumnCount(7);

    table->setHorizontalHeaderLabels({
        "ID",
        "Customer",
        "Vehicle",
        "Charging Point",
        "Start Time",
        "End Time",
        "Status"
    });

    table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch
    );

    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    table->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    mainLayout->addWidget(table);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addButton = new QPushButton("Add Booking");
    QPushButton *editButton = new QPushButton("Edit Booking");
    QPushButton *cancelButton = new QPushButton("Cancel Booking");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked,
            this, &BookingsPage::addBooking);

    connect(editButton, &QPushButton::clicked,
            this, &BookingsPage::editBooking);

    connect(cancelButton, &QPushButton::clicked,
            this, &BookingsPage::cancelBooking);

    connect(searchButton, &QPushButton::clicked,
            this, &BookingsPage::searchBookings);

    connect(refreshButton, &QPushButton::clicked,
            this, &BookingsPage::loadBookings);

    connect(searchInput, &QLineEdit::returnPressed,
            this, &BookingsPage::searchBookings);
}

void BookingsPage::loadBookings()
{
    searchInput->clear();

    QList<Booking> bookings = repository.getAllBookings();

    table->setRowCount(0);

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    for (const Booking &booking : bookings) {

        QString customerName;
        QString registration;
        QString pointName;

        QSqlQuery query(db);

        query.prepare(
            "SELECT full_name FROM customers "
            "WHERE customer_id = :id"
        );
        query.bindValue(":id", booking.customerId);

        if (query.exec() && query.next()) {
            customerName = query.value(0).toString();
        }

        query.prepare(
            "SELECT registration_number FROM vehicles "
            "WHERE vehicle_id = :id"
        );
        query.bindValue(":id", booking.vehicleId);

        if (query.exec() && query.next()) {
            registration = query.value(0).toString();
        }

        query.prepare(
            "SELECT point_id FROM charging_points "
            "WHERE point_id = :id"
        );
        query.bindValue(":id", booking.chargingPointId);

        if (query.exec() && query.next()) {
            pointName = "Point " + query.value(0).toString();
        }

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::number(booking.bookingId)));

        table->setItem(row, 1,
            new QTableWidgetItem(customerName));

        table->setItem(row, 2,
            new QTableWidgetItem(registration));

        table->setItem(row, 3,
            new QTableWidgetItem(pointName));

        table->setItem(row, 4,
            new QTableWidgetItem(
                booking.startTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 5,
            new QTableWidgetItem(
                booking.endTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 6,
            new QTableWidgetItem(booking.status));
    }

    if (!repository.lastError().isEmpty()) {
        QMessageBox::warning(
            this, "Database Error", repository.lastError());
    }
}

void BookingsPage::addBooking()
{
    QList<Customer> customers =
        customerRepository.getAllCustomers();

    QList<Vehicle> vehicles =
        vehicleRepository.getAllVehicles();

    QList<ChargingPoint> points =
        pointRepository.getAllPoints();

    if (customers.isEmpty() || vehicles.isEmpty() || points.isEmpty()) {
        QMessageBox::warning(
            this,
            "Missing Data",
            "Please add customers, vehicles and charging points first."
        );
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Add Booking");

    QFormLayout *form = new QFormLayout(&dialog);

    QComboBox *customerCombo = new QComboBox();
    QComboBox *vehicleCombo = new QComboBox();
    QComboBox *pointCombo = new QComboBox();

    for (const Customer &customer : customers) {
        customerCombo->addItem(
            customer.fullName,
            customer.customerId
        );
    }

    auto updateVehicles = [vehicleCombo, customers, vehicles](int index) {

        vehicleCombo->clear();

        int customerId = customers[index].customerId;

        for (const Vehicle &vehicle : vehicles) {

            if (vehicle.customerId == customerId) {

                vehicleCombo->addItem(
                    vehicle.registrationNumber,
                    vehicle.vehicleId
                );
            }
        }
    };

    updateVehicles(0);

    connect(
        customerCombo,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        updateVehicles
    );

    for (const ChargingPoint &point : points) {
        if (point.status == "AVAILABLE") {
            pointCombo->addItem(
                "Point " + QString::number(point.pointId) +
                " (" + point.chargerType + ")",
                point.pointId
            );
        }
    }

    if (pointCombo->count() == 0) {
        QMessageBox::warning(
            this, "No Available Points",
            "No available charging points found."
        );
        return;
    }

    QDateTimeEdit *startEdit = new QDateTimeEdit(
        QDateTime::currentDateTime()
    );

    startEdit->setCalendarPopup(true);
    startEdit->setDisplayFormat("dd-MM-yyyy hh:mm AP");

    QDateTimeEdit *endEdit = new QDateTimeEdit(
        QDateTime::currentDateTime().addSecs(3600)
    );

    endEdit->setCalendarPopup(true);
    endEdit->setDisplayFormat("dd-MM-yyyy hh:mm AP");

    form->addRow("Customer:", customerCombo);
    form->addRow("Vehicle:", vehicleCombo);
    form->addRow("Charging Point:", pointCombo);
    form->addRow("Start Time:", startEdit);
    form->addRow("End Time:", endEdit);

    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel
    );

    form->addRow(buttons);

    connect(buttons, &QDialogButtonBox::accepted,
            &dialog, &QDialog::accept);

    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Booking booking;

    booking.customerId = customerCombo->currentData().toInt();
    booking.vehicleId = vehicleCombo->currentData().toInt();
    booking.chargingPointId = pointCombo->currentData().toInt();

    booking.startTime = startEdit->dateTime();
    booking.endTime = endEdit->dateTime();

    booking.status = "PENDING";

    if (booking.endTime <= booking.startTime ||
        booking.startTime < QDateTime::currentDateTime()) {

        QMessageBox::warning(
            this,
            "Invalid Time",
            "Select a future start time and an end time after the start."
        );
        return;
    }

    // Verify that the selected vehicle belongs to the selected customer.
    bool vehicleMatches = false;

    for (const Vehicle &vehicle : vehicles) {
        if (vehicle.vehicleId == booking.vehicleId &&
            vehicle.customerId == booking.customerId) {
            vehicleMatches = true;
            break;
        }
    }

    if (!vehicleMatches) {
        QMessageBox::warning(
            this,
            "Invalid Vehicle",
            "The selected vehicle is not registered to this customer."
        );
        return;
    }

    if (!repository.isTimeSlotAvailable(
            booking.chargingPointId,
            booking.startTime,
            booking.endTime)) {

        QMessageBox::warning(
            this,
            "Charging Point Unavailable",
            repository.lastError().isEmpty()
                ? "This charging point is already booked during the selected time."
                : repository.lastError()
        );

        return;
    }

    if (repository.addBooking(booking)) {

        QMessageBox::information(
            this,
            "Success",
            "Booking created successfully."
        );

        loadBookings();

    } else {

        QMessageBox::critical(
            this,
            "Database Error",
            repository.lastError()
        );
    }
}


void BookingsPage::editBooking()
{
    QMessageBox::information(
        this,
        "Coming Next",
        "Booking editing will be implemented after availability validation."
    );
}

void BookingsPage::cancelBooking()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a booking first."
        );
        return;
    }

    int bookingId = table->item(row, 0)->text().toInt();

    if (table->item(row, 6)->text() == "CANCELLED") {
        QMessageBox::information(
            this, "Already Cancelled",
            "This booking is already cancelled."
        );
        return;
    }

    auto reply = QMessageBox::question(
        this,
        "Cancel Booking",
        "Are you sure you want to cancel this booking?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) return;

    if (repository.cancelBooking(bookingId)) {
        QMessageBox::information(
            this, "Success", "Booking cancelled successfully."
        );
        loadBookings();
    } else {
        QMessageBox::critical(
            this, "Database Error", repository.lastError()
        );
    }
}

void BookingsPage::searchBookings()
{
    QString searchText = searchInput->text().trimmed();

    if (searchText.isEmpty()) {
        loadBookings();
        return;
    }

    QList<Booking> bookings = repository.getAllBookings();

    table->setRowCount(0);

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    for (const Booking &booking : bookings) {

        QSqlQuery query(db);

        query.prepare(
            "SELECT full_name FROM customers "
            "WHERE customer_id = :id"
        );
        query.bindValue(":id", booking.customerId);

        if (!query.exec() || !query.next()) continue;

        QString customerName = query.value(0).toString();

        if (!customerName.contains(
                searchText, Qt::CaseInsensitive)) {
            continue;
        }

        int row = table->rowCount();
        table->insertRow(row);

        query.prepare(
            "SELECT registration_number FROM vehicles "
            "WHERE vehicle_id = :id"
        );
        query.bindValue(":id", booking.vehicleId);

        QString registration;
        if (query.exec() && query.next()) {
            registration = query.value(0).toString();
        }

        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(booking.bookingId)));

        table->setItem(row, 1,
            new QTableWidgetItem(customerName));

        table->setItem(row, 2,
            new QTableWidgetItem(registration));

        table->setItem(row, 3,
            new QTableWidgetItem(
                "Point " + QString::number(booking.chargingPointId)));

        table->setItem(row, 4,
            new QTableWidgetItem(
                booking.startTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 5,
            new QTableWidgetItem(
                booking.endTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 6,
            new QTableWidgetItem(booking.status));
    }
}