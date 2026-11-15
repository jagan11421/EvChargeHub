
#include "ChargingSessionsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTableWidgetItem>
#include <QDoubleValidator>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QAbstractItemView>
#include <QLineEdit>

ChargingSessionsPage::ChargingSessionsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadSessions();
}

void ChargingSessionsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Charging Sessions");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: white;"
    );

    QLabel *subtitle = new QLabel(
        "Monitor and manage EV charging activity"
    );
    subtitle->setStyleSheet("color: #aaaaaa;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search by session ID...");

    QPushButton *searchButton = new QPushButton("Search");
    QPushButton *refreshButton = new QPushButton("Refresh");

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(refreshButton);

    mainLayout->addLayout(searchLayout);

    table = new QTableWidget();
    table->setColumnCount(9);

    table->setHorizontalHeaderLabels({
        "Session ID",
        "Booking ID",
        "Customer ID",
        "Vehicle ID",
        "Charging Point",
        "Start Time",
        "End Time",
        "Energy (kWh)",
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

    QPushButton *startButton =
        new QPushButton("Start Session");

    QPushButton *completeButton =
        new QPushButton("Complete Session");

    QPushButton *cancelButton =
        new QPushButton("Cancel Session");

    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(completeButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(startButton, &QPushButton::clicked,
            this, &ChargingSessionsPage::startSession);

    connect(completeButton, &QPushButton::clicked,
            this, &ChargingSessionsPage::completeSession);

    connect(cancelButton, &QPushButton::clicked,
            this, &ChargingSessionsPage::cancelSession);

    connect(searchButton, &QPushButton::clicked,
            this, &ChargingSessionsPage::searchSessions);

    connect(refreshButton, &QPushButton::clicked,
            this, &ChargingSessionsPage::loadSessions);

    connect(searchInput, &QLineEdit::returnPressed,
            this, &ChargingSessionsPage::searchSessions);
}

void ChargingSessionsPage::loadSessions()
{
    searchInput->clear();

    QList<ChargingSession> sessions =
        repository.getAllSessions();

    table->setRowCount(0);

    for (const ChargingSession &session : sessions) {

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::number(session.sessionId)));

        table->setItem(row, 1,
            new QTableWidgetItem(
                QString::number(session.bookingId)));

        table->setItem(row, 2,
            new QTableWidgetItem(
                QString::number(session.customerId)));

        table->setItem(row, 3,
            new QTableWidgetItem(
                QString::number(session.vehicleId)));

        table->setItem(row, 4,
            new QTableWidgetItem(
                QString::number(session.chargingPointId)));

        table->setItem(row, 5,
            new QTableWidgetItem(
                session.startTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 6,
            new QTableWidgetItem(
                session.endTime.isValid()
                    ? session.endTime.toString("dd-MM-yyyy hh:mm AP")
                    : "In Progress"));

        table->setItem(row, 7,
            new QTableWidgetItem(
                QString::number(session.energyConsumed, 'f', 2)));

        table->setItem(row, 8,
            new QTableWidgetItem(session.status));
    }

    if (!repository.lastError().isEmpty()) {
        QMessageBox::warning(
            this, "Database Error",
            repository.lastError()
        );
    }
}

void ChargingSessionsPage::startSession()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT booking_id FROM bookings "
        "WHERE status IN ('PENDING', 'CONFIRMED') "
        "ORDER BY booking_id DESC")) {

        QMessageBox::critical(
            this, "Database Error",
            query.lastError().text()
        );
        return;
    }

    QComboBox *bookingCombo = new QComboBox();

    while (query.next()) {
        bookingCombo->addItem(
            "Booking #" + QString::number(query.value(0).toInt()),
            query.value(0)
        );
    }

    if (bookingCombo->count() == 0) {
        delete bookingCombo;

        QMessageBox::information(
            this, "No Bookings",
            "There are no eligible bookings to start."
        );
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Start Charging Session");

    QFormLayout *form = new QFormLayout(&dialog);

    form->addRow("Select Booking:", bookingCombo);

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

    int bookingId = bookingCombo->currentData().toInt();

    if (repository.startSession(bookingId)) {
        QMessageBox::information(
            this, "Success",
            "Charging session started successfully."
        );

        loadSessions();
    } else {
        QMessageBox::warning(
            this, "Unable to Start",
            repository.lastError()
        );
    }
}

void ChargingSessionsPage::completeSession()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a charging session."
        );
        return;
    }

    int sessionId =
        table->item(row, 0)->text().toInt();

    if (table->item(row, 8)->text() != "IN_PROGRESS") {
        QMessageBox::warning(
            this, "Invalid Session",
            "Only an in-progress session can be completed."
        );
        return;
    }

    bool ok = false;

    double energy = QInputDialog::getDouble(
        this,
        "Complete Charging Session",
        "Energy Consumed (kWh):",
        0.0,
        0.0,
        100000.0,
        2,
        &ok
    );

    if (!ok) return;

    if (energy <= 0.0) {
        QMessageBox::warning(
            this, "Invalid Energy",
            "Energy consumed must be greater than zero."
        );
        return;
    }

    if (repository.completeSession(sessionId, energy)) {
        QMessageBox::information(
            this, "Success",
            "Charging session completed successfully."
        );

        loadSessions();
    } else {
        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}

void ChargingSessionsPage::cancelSession()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a charging session."
        );
        return;
    }

    int sessionId =
        table->item(row, 0)->text().toInt();

    if (table->item(row, 8)->text() != "IN_PROGRESS") {
        QMessageBox::warning(
            this, "Invalid Session",
            "Only an in-progress session can be cancelled."
        );
        return;
    }

    auto reply = QMessageBox::question(
        this,
        "Cancel Session",
        "Are you sure you want to cancel this session?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes) return;

    if (repository.cancelSession(sessionId)) {
        QMessageBox::information(
            this, "Success",
            "Charging session cancelled."
        );

        loadSessions();
    } else {
        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}

void ChargingSessionsPage::searchSessions()
{
    QString searchText = searchInput->text().trimmed();

    if (searchText.isEmpty()) {
        loadSessions();
        return;
    }

    bool ok = false;
    int sessionId = searchText.toInt(&ok);

    if (!ok) {
        QMessageBox::information(
            this, "Search",
            "Please enter a valid session ID."
        );
        return;
    }

    QList<ChargingSession> sessions =
        repository.getAllSessions();

    table->setRowCount(0);

    for (const ChargingSession &session : sessions) {

        if (session.sessionId != sessionId)
            continue;

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(session.sessionId)));

        table->setItem(row, 1,
            new QTableWidgetItem(QString::number(session.bookingId)));

        table->setItem(row, 2,
            new QTableWidgetItem(QString::number(session.customerId)));

        table->setItem(row, 3,
            new QTableWidgetItem(QString::number(session.vehicleId)));

        table->setItem(row, 4,
            new QTableWidgetItem(QString::number(session.chargingPointId)));

        table->setItem(row, 5,
            new QTableWidgetItem(
                session.startTime.toString("dd-MM-yyyy hh:mm AP")));

        table->setItem(row, 6,
            new QTableWidgetItem(
                session.endTime.isValid()
                    ? session.endTime.toString("dd-MM-yyyy hh:mm AP")
                    : "In Progress"));

        table->setItem(row, 7,
            new QTableWidgetItem(
                QString::number(session.energyConsumed, 'f', 2)));

        table->setItem(row, 8,
            new QTableWidgetItem(session.status));
    }
}
