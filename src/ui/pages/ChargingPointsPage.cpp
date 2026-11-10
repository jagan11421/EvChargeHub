
#include "ChargingPointsPage.h"

#include "../../database/StationRepository.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidgetItem>
#include <QAbstractItemView>

ChargingPointsPage::ChargingPointsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    refreshTable();
}

void ChargingPointsPage::setupUI()
{
    setStyleSheet("background: #F3F4F6;");

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QLabel *heading = new QLabel("Charging Points");

    heading->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    layout->addWidget(heading);

    QLabel *subtitle = new QLabel(
        "Manage AC and DC charging points."
    );

    subtitle->setStyleSheet(
        "color: #6B7280;"
        "font-size: 14px;"
    );

    layout->addWidget(subtitle);

    QHBoxLayout *buttons = new QHBoxLayout;

    QPushButton *addButton =
        new QPushButton("+ Add Charging Point");

    QPushButton *editButton =
        new QPushButton("Edit");

    QPushButton *deleteButton =
        new QPushButton("Delete");

    addButton->setStyleSheet(
        "background: #2563EB; color: white;"
        "padding: 10px 18px; border-radius: 6px;"
    );

    editButton->setStyleSheet(
        "background: #374151; color: white;"
        "padding: 10px 18px; border-radius: 6px;"
    );

    deleteButton->setStyleSheet(
        "background: #DC2626; color: white;"
        "padding: 10px 18px; border-radius: 6px;"
    );

    buttons->addWidget(addButton);
    buttons->addWidget(editButton);
    buttons->addWidget(deleteButton);
    buttons->addStretch();

    layout->addLayout(buttons);

    pointTable = new QTableWidget(this);

    pointTable->setColumnCount(7);

    pointTable->setHorizontalHeaderLabels({
        "ID",
        "Station",
        "Type",
        "Power (kW)",
        "Price (₹/kWh)",
        "Status",
        " "
    });

    pointTable->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    pointTable->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    pointTable->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    pointTable->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    pointTable->verticalHeader()->setVisible(false);

    pointTable->setStyleSheet(
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

    layout->addWidget(pointTable);

    connect(addButton, &QPushButton::clicked,
            this, &ChargingPointsPage::addPoint);

    connect(editButton, &QPushButton::clicked,
            this, &ChargingPointsPage::editPoint);

    connect(deleteButton, &QPushButton::clicked,
            this, &ChargingPointsPage::deletePoint);
}

void ChargingPointsPage::refreshTable()
{
    QList<ChargingPoint> points =
        repository.getAllPoints();

    if (!repository.lastError().isEmpty())
    {
        QMessageBox::critical(
            this,
            "Database Error",
            repository.lastError()
        );

        return;
    }

    pointTable->setRowCount(points.size());

    for (int i = 0; i < points.size(); i++)
    {
        const ChargingPoint &point = points[i];

        pointTable->setItem(
            i, 0,
            new QTableWidgetItem(
                QString::number(point.pointId)
            )
        );

        pointTable->setItem(
            i, 1,
            new QTableWidgetItem(point.stationName)
        );

        pointTable->setItem(
            i, 2,
            new QTableWidgetItem(point.chargerType)
        );

        pointTable->setItem(
            i, 3,
            new QTableWidgetItem(
                QString::number(point.powerKW)
            )
        );

        pointTable->setItem(
            i, 4,
            new QTableWidgetItem(
                QString::number(point.pricePerKwh)
            )
        );

        pointTable->setItem(
            i, 5,
            new QTableWidgetItem(point.status)
        );

        pointTable->setItem(
            i, 6,
            new QTableWidgetItem("")
        );
    }
}

void ChargingPointsPage::addPoint()
{
    StationRepository stationRepo;

    QList<Station> stations =
        stationRepo.getAllStations();

    if (!stationRepo.lastError().isEmpty())
    {
        QMessageBox::critical(
            this, "Error", stationRepo.lastError()
        );
        return;
    }

    if (stations.isEmpty())
    {
        QMessageBox::warning(
            this,
            "No Stations",
            "Please add a charging station first."
        );
        return;
    }

    QStringList stationNames;

    for (const Station &station : stations)
    {
        stationNames.append(
            QString::number(station.stationId)
            + " - " + station.name
        );
    }

    bool ok;

    QString selectedStation = QInputDialog::getItem(
        this,
        "Select Station",
        "Charging Station:",
        stationNames,
        0,
        false,
        &ok
    );

    if (!ok)
        return;

    int selectedIndex = stationNames.indexOf(selectedStation);

    if (selectedIndex < 0)
        return;

    ChargingPoint point;

    point.stationId = stations[selectedIndex].stationId;

    point.chargerType = QInputDialog::getItem(
        this,
        "Charger Type",
        "Select Charger Type:",
        {"AC", "DC"},
        0,
        false,
        &ok
    );

    if (!ok)
        return;

    point.powerKW = QInputDialog::getDouble(
        this,
        "Charging Power",
        "Power (kW):",
        point.chargerType == "AC" ? 7.4 : 30.0,
        0.1,
        1000.0,
        2,
        &ok
    );

    if (!ok)
        return;

    point.pricePerKwh = QInputDialog::getDouble(
        this,
        "Electricity Rate",
        "Price per kWh (₹):",
        15.0,
        0.0,
        10000.0,
        2,
        &ok
    );

    if (!ok)
        return;

    point.status = "AVAILABLE";

    if (repository.addPoint(point))
    {
        QMessageBox::information(
            this,
            "Success",
            "Charging point added successfully!"
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

void ChargingPointsPage::editPoint()
{
    int row = pointTable->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this, "Selection Required",
            "Select a charging point first."
        );
        return;
    }

    int pointId =
        pointTable->item(row, 0)->text().toInt();

    ChargingPoint point;

    point.pointId = pointId;

    StationRepository stationRepo;

    QList<Station> stations =
        stationRepo.getAllStations();

    if (!stationRepo.lastError().isEmpty())
    {
        QMessageBox::critical(
            this, "Error", stationRepo.lastError()
        );
        return;
    }

    QStringList stationNames;

    int selectedIndex = -1;

    for (int i = 0; i < stations.size(); i++)
    {
        stationNames.append(
            QString::number(stations[i].stationId)
            + " - " + stations[i].name
        );

        if (stations[i].name ==
            pointTable->item(row, 1)->text())
        {
            selectedIndex = i;
        }
    }

    if (selectedIndex < 0)
        return;

    bool ok;

    QString selectedStation = QInputDialog::getItem(
        this, "Edit Station", "Station:",
        stationNames, selectedIndex, false, &ok
    );

    if (!ok)
        return;

    int newIndex = stationNames.indexOf(selectedStation);

    if (newIndex < 0)
        return;

    point.stationId = stations[newIndex].stationId;

    point.chargerType = QInputDialog::getItem(
        this, "Edit Type", "Charger Type:",
        {"AC", "DC"},
        pointTable->item(row, 2)->text() == "AC" ? 0 : 1,
        false, &ok
    );

    if (!ok)
        return;

    point.powerKW = QInputDialog::getDouble(
        this, "Edit Power", "Power (kW):",
        pointTable->item(row, 3)->text().toDouble(),
        0.1, 1000.0, 2, &ok
    );

    if (!ok)
        return;

    point.pricePerKwh = QInputDialog::getDouble(
        this, "Edit Price", "Price per kWh:",
        pointTable->item(row, 4)->text().toDouble(),
        0.0, 10000.0, 2, &ok
    );

    if (!ok)
        return;

    point.status = QInputDialog::getItem(
        this, "Edit Status", "Status:",
        {"AVAILABLE", "OCCUPIED", "MAINTENANCE", "INACTIVE"},
        0, false, &ok
    );

    if (!ok)
        return;

    if (repository.updatePoint(point))
    {
        QMessageBox::information(
            this, "Success",
            "Charging point updated successfully!"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}

void ChargingPointsPage::deletePoint()
{
    int row = pointTable->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this, "Selection Required",
            "Select a charging point first."
        );
        return;
    }

    int pointId =
        pointTable->item(row, 0)->text().toInt();

    auto reply = QMessageBox::question(
        this,
        "Confirm Delete",
        "Delete this charging point?",
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply != QMessageBox::Yes)
        return;

    if (repository.deletePoint(pointId))
    {
        QMessageBox::information(
            this, "Success",
            "Charging point deleted successfully!"
        );

        refreshTable();
    }
    else
    {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}
