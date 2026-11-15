
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
#include <QDate>
#include <QStringList>
#include <QVariant>
#include <QPainter>

// Qt Charts
#include <QChartView>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPieSeries>
#include <QPieSlice>

// Constructor
DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadDashboardData();
}

// =====================================
// SETUP UI
// =====================================

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

    // =====================================
    // STATISTICS CARDS
    // =====================================

    QGridLayout *cards = new QGridLayout();
    cards->setSpacing(15);

    auto createCard = [](const QString &title,
                         QLabel **valueLabel)
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

    // =====================================
    // ANALYTICS CHARTS
    // =====================================

    QHBoxLayout *chartsLayout = new QHBoxLayout();

    chartsLayout->setSpacing(15);

    revenueChartView = new QChartView(this);
    sessionChartView = new QChartView(this);

    revenueChartView->setRenderHint(QPainter::Antialiasing);
    sessionChartView->setRenderHint(QPainter::Antialiasing);

    revenueChartView->setMinimumHeight(280);
    sessionChartView->setMinimumHeight(280);

    chartsLayout->addWidget(revenueChartView);
    chartsLayout->addWidget(sessionChartView);

    layout->addLayout(chartsLayout);

    // =====================================
    // RECENT BOOKINGS
    // =====================================

    QLabel *bookingsHeading =
        new QLabel("Recent Bookings");

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

    // Refresh connection
    connect(
        refreshButton,
        &QPushButton::clicked,
        this,
        [this]()
        {
            loadDashboardData();
        }
    );
}

// =====================================
// LOAD DASHBOARD STATISTICS
// =====================================

void DashboardPage::loadDashboardData()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.isOpen())
    {
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
    ))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
        stationsValue->setText(query.value(0).toString());

    // Total customers
    if (!query.exec(
        "SELECT COUNT(*) FROM customers"
    ))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
        customersValue->setText(query.value(0).toString());

    // Total charging points
    if (!query.exec(
        "SELECT COUNT(*) FROM charging_points"
    ))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
        pointsValue->setText(query.value(0).toString());

    // Active charging sessions
    if (!query.exec(R"(
        SELECT COUNT(*)
        FROM charging_sessions
        WHERE status = 'IN_PROGRESS'
    )"))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
        sessionsValue->setText(query.value(0).toString());

    // Registered vehicles
    if (!query.exec(
        "SELECT COUNT(*) FROM vehicles"
    ))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
        vehiclesValue->setText(query.value(0).toString());

    // Today's revenue
    if (!query.exec(R"(
        SELECT COALESCE(SUM(amount), 0)
        FROM payments
        WHERE status = 'PAID'
          AND date(paid_at) = date('now', 'localtime')
    )"))
    {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next())
    {
        double revenue = query.value(0).toDouble();

        revenueValue->setText(
            QString("₹ %1").arg(revenue, 0, 'f', 2)
        );
    }

    loadRecentBookings();
    loadCharts();
}

// =====================================
// LOAD RECENT BOOKINGS
// =====================================

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

    if (!query.exec())
    {
        QMessageBox::critical(
            this,
            "Database Error",
            query.lastError().text()
        );

        return;
    }

    recentBookingsTable->setRowCount(0);

    while (query.next())
    {
        int row = recentBookingsTable->rowCount();

        recentBookingsTable->insertRow(row);

        for (int column = 0; column < 6; column++)
        {
            QString value = query.value(column).toString();

            // Format start time
            if (column == 4)
            {
                QDateTime dateTime = QDateTime::fromString(
                    value, Qt::ISODate
                );

                if (dateTime.isValid())
                {
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

// =====================================
// LOAD ANALYTICS CHARTS
// =====================================

void DashboardPage::loadCharts()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.isOpen())
        return;

    // =================================
    // MONTHLY REVENUE BAR CHART
    // =================================

    QBarSet *revenueSet = new QBarSet("Revenue");

    QStringList months;

    QSqlQuery revenueQuery(db);

    if (!revenueQuery.exec(R"(
        SELECT
            strftime('%Y-%m', paid_at) AS month,
            SUM(amount)
        FROM payments
        WHERE status = 'PAID'
          AND paid_at IS NOT NULL
        GROUP BY month
        ORDER BY month DESC
        LIMIT 12
    )"))
    {
        delete revenueSet;
        return;
    }

    // Reverse results to display oldest month first.
    QStringList monthLabels;
    QList<double> revenueValues;

    while (revenueQuery.next())
    {
        QString month = revenueQuery.value(0).toString();

        QDate date = QDate::fromString(
            month + "-01", "yyyy-MM-dd"
        );

        monthLabels.prepend(
            date.isValid()
                ? date.toString("MMM yyyy")
                : month
        );

        revenueValues.prepend(
            revenueQuery.value(1).toDouble()
        );
    }

    months = monthLabels;

    for (double value : revenueValues)
        *revenueSet << value;

    QBarSeries *barSeries = new QBarSeries();

    barSeries->append(revenueSet);

    QChart *revenueChart = new QChart();

    revenueChart->addSeries(barSeries);
    revenueChart->setTitle("Monthly Revenue");

    revenueChart->setAnimationOptions(
        QChart::SeriesAnimations
    );

    QBarCategoryAxis *axisX = new QBarCategoryAxis();

    if (!months.isEmpty())
        axisX->append(months);

    QValueAxis *axisY = new QValueAxis();

    axisY->setTitleText("Revenue (₹)");
    axisY->setLabelFormat("%.0f");
    axisY->setMin(0);

    double maxRevenue = 0;

    for (double value : revenueValues)
        maxRevenue = qMax(maxRevenue, value);

    axisY->setMax(maxRevenue > 0 ? maxRevenue * 1.2 : 100);

    revenueChart->addAxis(axisX, Qt::AlignBottom);
    revenueChart->addAxis(axisY, Qt::AlignLeft);

    barSeries->attachAxis(axisX);
    barSeries->attachAxis(axisY);

    revenueChart->legend()->setVisible(false);

    if (months.isEmpty())
        revenueChart->setTitle("Monthly Revenue (No Data Yet)");

    revenueChartView->setChart(revenueChart);

    // =================================
    // SESSION STATUS PIE CHART
    // =================================

    QPieSeries *pieSeries = new QPieSeries();

    QSqlQuery sessionQuery(db);

    if (!sessionQuery.exec(R"(
        SELECT status, COUNT(*)
        FROM charging_sessions
        GROUP BY status
    )"))
    {
        return;
    }

    while (sessionQuery.next())
    {
        QString status = sessionQuery.value(0).toString();

        int count = sessionQuery.value(1).toInt();

        QPieSlice *slice = pieSeries->append(
            status + " (" + QString::number(count) + ")",
            count
        );

        slice->setLabelVisible(true);
    }

    QChart *sessionChart = new QChart();

    sessionChart->addSeries(pieSeries);

    sessionChart->setTitle("Charging Session Status");

    sessionChart->setAnimationOptions(
        QChart::SeriesAnimations
    );

    sessionChart->legend()->setAlignment(Qt::AlignBottom);

    if (pieSeries->count() == 0)
        sessionChart->setTitle("Charging Session Status (No Data Yet)");

    sessionChartView->setChart(sessionChart);
}
