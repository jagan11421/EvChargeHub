
#include "StationsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QAbstractItemView>

StationsPage::StationsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    refreshTable();
}

void StationsPage::setupUI()
{
    setStyleSheet("background: #F3F4F6;");

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    // Heading
    QLabel *heading = new QLabel("Charging Stations");

    heading->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    layout->addWidget(heading);

    QLabel *subtitle = new QLabel(
        "Manage your registered EV charging stations."
    );

    subtitle->setStyleSheet(
        "color: #6B7280;"
        "font-size: 14px;"
    );

    layout->addWidget(subtitle);

    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *addButton =
        new QPushButton("+ Add Station");

    QPushButton *editButton =
        new QPushButton("Edit Station");

    QPushButton *deleteButton =
        new QPushButton("Delete Station");

    addButton->setStyleSheet(
        "background: #2563EB;"
        "color: white;"
        "padding: 10px 18px;"
        "border-radius: 6px;"
    );

    editButton->setStyleSheet(
        "background: #374151;"
        "color: white;"
        "padding: 10px 18px;"
        "border-radius: 6px;"
    );

    deleteButton->setStyleSheet(
        "background: #DC2626;"
        "color: white;"
        "padding: 10px 18px;"
        "border-radius: 6px;"
    );

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);

    // Stations table
    stationTable = new QTableWidget(this);

    stationTable->setColumnCount(5);

    stationTable->setHorizontalHeaderLabels({
        "ID",
        "Station Name",
        "Address",
        "Power (kW)",
        "Status"
    });

    stationTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    stationTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    stationTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    stationTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    stationTable->verticalHeader()->setVisible(false);

    stationTable->setStyleSheet(
        "QTableWidget {"
        "background: white;"
        "border: none;"
        "border-radius: 8px;"
        "gridline-color: #E5E7EB;"
        "}"
        "QHeaderView::section {"
        "background: #E5E7EB;"
        "padding: 10px;"
        "border: none;"
        "font-weight: bold;"
        "}"
    );

    layout->addWidget(stationTable);

    // Connect buttons
    connect(addButton, &QPushButton::clicked,
            this, &StationsPage::addStation);

    connect(editButton, &QPushButton::clicked,
            this, &StationsPage::editStation);

    connect(deleteButton, &QPushButton::clicked,
            this, &StationsPage::deleteStation);
}

void StationsPage::refreshTable()
{
    QList<Station> stations =
        repository.getAllStations();

    if (!repository.lastError().isEmpty())
    {
        QMessageBox::critical(
            this,
            "Database Error",
            repository.lastError()
        );
        return;
    }

    stationTable->setRowCount(stations.size());

    for (int i = 0; i < stations.size(); i++)
    {
        const Station &station = stations[i];

        stationTable->setItem(
            i, 0,
            new QTableWidgetItem(
                QString::number(station.stationId)
            )
        );

        stationTable->setItem(
            i, 1,
            new QTableWidgetItem(station.name)
        );

        stationTable->setItem(
            i, 2,
            new QTableWidgetItem(station.address)
        );

        stationTable->setItem(
            i, 3,
            new QTableWidgetItem(
                QString::number(station.totalPower)
            )
        );

        stationTable->setItem(
            i, 4,
            new QTableWidgetItem(station.status)
        );
    }
}

void StationsPage::addStation()
{
    bool ok;

    Station station;

    station.name = QInputDialog::getText(
        this,
        "Add Station",
        "Station Name:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || station.name.trimmed().isEmpty())
        return;

    station.address = QInputDialog::getText(
        this,
        "Add Station",
        "Station Address:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || station.address.trimmed().isEmpty())
        return;

    station.totalPower = QInputDialog::getDouble(
        this,
        "Add Station",
        "Total Power (kW):",
        100.0,
        0.1,
        100000.0,
        2,
        &ok
    );

    if (!ok)
        return;

    station.name = station.name.trimmed();
    station.address = station.address.trimmed();

    station.status = "ACTIVE";

    if (repository.addStation(station))
    {
        QMessageBox::information(
            this,
            "Success",
            "Station added successfully!"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this,
            "Error",
            repository.lastError()
        );
    }
}

void StationsPage::editStation()
{
    int row = stationTable->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Selection Required",
            "Please select a station to edit."
        );
        return;
    }

    Station station;

    station.stationId =
        stationTable->item(row, 0)->text().toInt();

    station.name =
        stationTable->item(row, 1)->text();

    station.address =
        stationTable->item(row, 2)->text();

    station.totalPower =
        stationTable->item(row, 3)->text().toDouble();

    station.status =
        stationTable->item(row, 4)->text();

    bool ok;

    QString newName = QInputDialog::getText(
        this,
        "Edit Station",
        "Station Name:",
        QLineEdit::Normal,
        station.name,
        &ok
    );

    if (!ok || newName.trimmed().isEmpty())
        return;

    QString newAddress = QInputDialog::getText(
        this,
        "Edit Station",
        "Station Address:",
        QLineEdit::Normal,
        station.address,
        &ok
    );

    if (!ok || newAddress.trimmed().isEmpty())
        return;

    double newPower = QInputDialog::getDouble(
        this,
        "Edit Station",
        "Total Power (kW):",
        station.totalPower,
        0.1,
        100000.0,
        2,
        &ok
    );

    if (!ok)
        return;

    station.name = newName.trimmed();
    station.address = newAddress.trimmed();
    station.totalPower = newPower;

    if (repository.updateStation(station))
    {
        QMessageBox::information(
            this,
            "Success",
            "Station updated successfully!"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this,
            "Error",
            repository.lastError()
        );
    }
}

void StationsPage::deleteStation()
{
    int row = stationTable->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Selection Required",
            "Please select a station to delete."
        );
        return;
    }

    int stationId =
        stationTable->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(
        this,
        "Confirm Delete",
        "Are you sure you want to delete this station?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes)
        return;

    if (repository.deleteStation(stationId))
    {
        QMessageBox::information(
            this,
            "Success",
            "Station deleted successfully!"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this,
            "Error",
            repository.lastError()
        );
    }
}
