
#include "DashboardPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QTableWidgetItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>
#include <QVariant>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadDashboardData();
}

void DashboardPage::setupUI()
{
    setStyleSheet("background: #F3F4F6;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(18);

    // Heading
    QLabel *heading = new QLabel("Dashboard");

    heading->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    layout->addWidget(heading);

    QLabel *subtitle = new QLabel(
        "Welcome to EV ChargeHub Management System"
    );

    subtitle->setStyleSheet(
        "color: #6B7280;"
        "font-size: 14px;"
    );

    layout->addWidget(subtitle);

    // Refresh button
    QHBoxLayout *headerLayout = new QHBoxLayout();

    headerLayout->addStretch();

    QPushButton *refreshButton =
        new QPushButton("Refresh Dashboard");

    refreshButton->setStyleSheet(
        "QPushButton {"
        "background: #2563EB;"
        "color: white;"
        "padding: 9px 16px;"
        "border-radius: 6px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: #1D4ED8;"
        "}"
    );

    headerLayout->addWidget(refreshButton);
    layout->addLayout(headerLayout);

    // Statistics cards
    QGridLayout *cards = new QGridLayout();
    cards->setSpacing(15);

    auto createCard = [](const QString &title, QLabel **valueLabel)
    {
        QFrame *card = new QFrame();

        card->setStyleSheet(
            "QFrame {"
            "background: white;"
            "border-radius: 12px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(18, 18, 18, 18);
        cardLayout->setSpacing(8);

        *valueLabel = new QLabel("0");

        (*valueLabel)->setStyleSheet(
            "font-size: 27px;"
            "font-weight: bold;"
            "color: #111827;"
        );

        QLabel *titleLabel = new QLabel(title);

        titleLabel->setStyleSheet(
            "font-size: 13px;"
            "color: #6B7280;"
        );

        cardLayout->addWidget(*valueLabel);
        cardLayout->addWidget(titleLabel);

        return card;
    };

    cards->addWidget(
        createCard("Total Stations", &stationsValue), 0, 0
    );

    cards->addWidget(
        createCard("Total Customers", &customersValue), 0, 1
    );

    cards->addWidget(
        createCard("Charging Points", &pointsValue), 0, 2
    );

    cards->addWidget(
        createCard("Active Sessions", &sessionsValue), 1, 0
    );

    cards->addWidget(
        createCard("Registered Vehicles", &vehiclesValue), 1, 1
    );

    cards->addWidget(
        createCard("Today's Revenue", &revenueValue), 1, 2
    );

    layout->addLayout(cards);

    // Recent bookings section
    QLabel *bookingsHeading = new QLabel("Recent Bookings");

    bookingsHeading->setStyleSheet(
        "font-size: 19px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    layout->addWidget(bookingsHeading);

    recentBookingsTable = new QTableWidget();

    recentBookingsTable->setColumnCount(6);

    recentBookingsTable->setHorizontalHeaderLabels({
        "Booking ID",
        "Customer",
        "Vehicle",
        "Charging Point",
        "Start Time",
        "Status"
    });

    recentBookingsTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    recentBookingsTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    recentBookingsTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    recentBookingsTable->setAlternatingRowColors(true);
    recentBookingsTable->setMinimumHeight(220);

    layout->addWidget(recentBookingsTable);

    layout->addStretch();

    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        [this]() {
            loadDashboardData();
        }
    );
}

void DashboardPage::loadDashboardData()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.isOpen()) {
        QMessageBox::critical(
            this,
            "Database Error",
            "Database connection is not open."
        );
        return;
    }

    QSqlQuery query(db);

    // Total stations
    if (!query.exec(
        "SELECT COUNT(*) FROM stations"
    )) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        stationsValue->setText(query.value(0).toString());
    }

    // Total customers
    if (!query.exec(
        "SELECT COUNT(*) FROM customers"
    )) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        customersValue->setText(query.value(0).toString());
    }

    // Total charging points
    if (!query.exec(
        "SELECT COUNT(*) FROM charging_points"
    )) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        pointsValue->setText(query.value(0).toString());
    }

    // Active charging sessions
    if (!query.exec(R"(
        SELECT COUNT(*)
        FROM charging_sessions
        WHERE status = 'IN_PROGRESS'
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        sessionsValue->setText(query.value(0).toString());
    }

    // Registered vehicles
    if (!query.exec(
        "SELECT COUNT(*) FROM vehicles"
    )) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        vehiclesValue->setText(query.value(0).toString());
    }

    // Today's revenue from paid payments
    if (!query.exec(R"(
        SELECT COALESCE(SUM(amount), 0)
        FROM payments
        WHERE status = 'PAID'
          AND date(paid_at) = date('now', 'localtime')
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        double revenue = query.value(0).toDouble();

        revenueValue->setText(
            QString("₹ %1").arg(revenue, 0, 'f', 2)
        );
    }

    loadRecentBookings();
}

void DashboardPage::loadRecentBookings()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(R"(
        SELECT
            b.booking_id,
            c.full_name,
            v.registration_number,
            cp.point_id,
            b.start_time,
            b.status
        FROM bookings b

        JOIN customers c
            ON b.customer_id = c.customer_id

        JOIN vehicles v
            ON b.vehicle_id = v.vehicle_id

        JOIN charging_points cp
            ON b.point_id = cp.point_id

        ORDER BY b.booking_id DESC
        LIMIT 10
    )");

    if (!query.exec()) {
        QMessageBox::critical(
            this,
            "Database Error",
            query.lastError().text()
        );
        return;
    }

    recentBookingsTable->setRowCount(0);

    while (query.next()) {

        int row = recentBookingsTable->rowCount();

        recentBookingsTable->insertRow(row);

        for (int column = 0; column < 6; column++) {

            QString value = query.value(column).toString();

            // Format start time
            if (column == 4) {
                QDateTime dateTime = QDateTime::fromString(
                    value, Qt::ISODate
                );

                if (dateTime.isValid()) {
                    value = dateTime.toString(
                        "dd-MM-yyyy hh:mm"
                    );
                }
            }

            recentBookingsTable->setItem(
                row,
                column,
                new QTableWidgetItem(value)
            );
        }
    }
}
