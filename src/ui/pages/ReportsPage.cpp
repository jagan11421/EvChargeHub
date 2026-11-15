#include "ReportsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QFrame>
#include <QVariant>
#include <QAbstractItemView>
#include <QTableWidgetItem>
ReportsPage::ReportsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadReports();
}

void ReportsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Reports & Analytics");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold;"
    );

    mainLayout->addWidget(title);

    QGridLayout *statsLayout = new QGridLayout();

    auto createCard = [](const QString &heading, QLabel **valueLabel) {
        QFrame *card = new QFrame();
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet(
            "QFrame { border: 1px solid #cccccc;"
            " border-radius: 8px; padding: 12px; }"
        );

        QVBoxLayout *layout = new QVBoxLayout(card);

        QLabel *headingLabel = new QLabel(heading);
        headingLabel->setStyleSheet(
            "font-size: 14px; color: gray; border: none;"
        );

        *valueLabel = new QLabel("0");
        (*valueLabel)->setStyleSheet(
            "font-size: 23px; font-weight: bold; border: none;"
        );

        layout->addWidget(headingLabel);
        layout->addWidget(*valueLabel);

        return card;
    };

    statsLayout->addWidget(
        createCard("Total Revenue", &totalRevenueLabel), 0, 0
    );

    statsLayout->addWidget(
        createCard("Charging Sessions", &totalSessionsLabel), 0, 1
    );

    statsLayout->addWidget(
        createCard("Energy Consumed", &totalEnergyLabel), 1, 0
    );

    statsLayout->addWidget(
        createCard("Paid Payments", &paidPaymentsLabel), 1, 1
    );

    mainLayout->addLayout(statsLayout);

    QLabel *monthlyTitle = new QLabel("Monthly Revenue");
    monthlyTitle->setStyleSheet(
        "font-size: 18px; font-weight: bold; margin-top: 12px;"
    );

    mainLayout->addWidget(monthlyTitle);

    monthlyRevenueTable = new QTableWidget();
    monthlyRevenueTable->setColumnCount(2);

    monthlyRevenueTable->setHorizontalHeaderLabels({
        "Month",
        "Revenue"
    });

    monthlyRevenueTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    monthlyRevenueTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    mainLayout->addWidget(monthlyRevenueTable);

    QPushButton *refreshButton = new QPushButton("Refresh Reports");
    mainLayout->addWidget(refreshButton);

    connect(refreshButton, &QPushButton::clicked,
            this, &ReportsPage::loadReports);
}

void ReportsPage::loadReports()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    if (!db.isOpen()) {
        QMessageBox::critical(
            this, "Database Error",
            "Database connection is not open."
        );
        return;
    }

    QSqlQuery query(db);

    // Total revenue from successful payments.
    if (!query.exec(R"(
        SELECT COALESCE(SUM(amount), 0)
        FROM payments
        WHERE status = 'PAID'
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        totalRevenueLabel->setText(
            "₹ " + QString::number(query.value(0).toDouble(), 'f', 2)
        );
    }

    // Completed charging sessions.
    if (!query.exec(R"(
        SELECT COUNT(*)
        FROM charging_sessions
        WHERE status = 'COMPLETED'
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        totalSessionsLabel->setText(
            query.value(0).toString()
        );
    }

    // Total energy consumed by completed sessions.
    if (!query.exec(R"(
        SELECT COALESCE(SUM(energy_consumed), 0)
        FROM charging_sessions
        WHERE status = 'COMPLETED'
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        totalEnergyLabel->setText(
            QString::number(query.value(0).toDouble(), 'f', 2)
            + " kWh"
        );
    }

    // Number of successful payments.
    if (!query.exec(R"(
        SELECT COUNT(*)
        FROM payments
        WHERE status = 'PAID'
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    if (query.next()) {
        paidPaymentsLabel->setText(
            query.value(0).toString()
        );
    }

    // Monthly revenue.
    if (!query.exec(R"(
        SELECT
            strftime('%Y-%m', paid_at) AS month,
            COALESCE(SUM(amount), 0) AS revenue
        FROM payments
        WHERE status = 'PAID'
          AND paid_at IS NOT NULL
        GROUP BY strftime('%Y-%m', paid_at)
        ORDER BY month DESC
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    monthlyRevenueTable->setRowCount(0);

    while (query.next()) {
        int row = monthlyRevenueTable->rowCount();
        monthlyRevenueTable->insertRow(row);

        monthlyRevenueTable->setItem(
            row, 0,
            new QTableWidgetItem(query.value(0).toString())
        );

        monthlyRevenueTable->setItem(
            row, 1,
            new QTableWidgetItem(
                "₹ " + QString::number(
                    query.value(1).toDouble(), 'f', 2
                )
            )
        );
    }
}