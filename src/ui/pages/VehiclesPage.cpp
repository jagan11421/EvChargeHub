
#include "VehiclesPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QTableWidgetItem>
#include <QStringList>
#include "../../database/CustomerRepository.h"
#include "../../models/Customer.h"
VehiclesPage::VehiclesPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadVehicles();
}

void VehiclesPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Vehicle Management");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #ffffff;"
    );

    QLabel *subtitle = new QLabel(
        "Register and manage electric vehicles"
    );

    subtitle->setStyleSheet("color: #aaaaaa;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    // Search bar
    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search registration number...");

    QPushButton *searchButton = new QPushButton("Search");
    QPushButton *refreshButton = new QPushButton("Refresh");

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(refreshButton);

    mainLayout->addLayout(searchLayout);

    // Vehicle table
    table = new QTableWidget();

    table->setColumnCount(8);

    table->setHorizontalHeaderLabels({
        "ID",
        "Owner Name",
        "Registration No.",
        "Brand",
        "Model",
        "Battery (kWh)",
        "Connector",
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

    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addButton = new QPushButton("Add Vehicle");
    QPushButton *editButton = new QPushButton("Edit Vehicle");
    QPushButton *deleteButton = new QPushButton("Delete Vehicle");

    addButton->setStyleSheet(
        "background-color: #16a34a; color: white; padding: 9px;"
    );

    editButton->setStyleSheet(
        "background-color: #2563eb; color: white; padding: 9px;"
    );

    deleteButton->setStyleSheet(
        "background-color: #dc2626; color: white; padding: 9px;"
    );

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);

    mainLayout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked,
            this, &VehiclesPage::addVehicle);

    connect(editButton, &QPushButton::clicked,
            this, &VehiclesPage::editVehicle);

    connect(deleteButton, &QPushButton::clicked,
            this, &VehiclesPage::deleteVehicle);

    connect(searchButton, &QPushButton::clicked,
            this, &VehiclesPage::searchVehicles);

    connect(refreshButton, &QPushButton::clicked,
            this, &VehiclesPage::loadVehicles);

    connect(searchInput, &QLineEdit::returnPressed,
            this, &VehiclesPage::searchVehicles);
}


void VehiclesPage::loadVehicles()
{
    searchInput->clear();

    QList<Vehicle> vehicles = repository.getAllVehicles();

    table->setRowCount(0);

    for (const Vehicle &vehicle : vehicles) {
        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(vehicle.vehicleId)));

        table->setItem(row, 1,
            new QTableWidgetItem(vehicle.ownerName));

        table->setItem(row, 2,
            new QTableWidgetItem(vehicle.registrationNumber));

        table->setItem(row, 3,
            new QTableWidgetItem(vehicle.brand));

        table->setItem(row, 4,
            new QTableWidgetItem(vehicle.model));

        table->setItem(row, 5,
            new QTableWidgetItem(
                QString::number(vehicle.batteryCapacity, 'f', 1)));

        table->setItem(row, 6,
            new QTableWidgetItem(vehicle.connectorType));

        table->setItem(row, 7,
            new QTableWidgetItem(vehicle.status));
    }

    if (!repository.lastError().isEmpty()) {
        QMessageBox::warning(
            this, "Database Error", repository.lastError());
    }
}

void VehiclesPage::addVehicle()
{
    Vehicle vehicle;

    CustomerRepository customerRepository;
    QList<Customer> customers = customerRepository.getAllCustomers();

    if (customers.isEmpty()) {
        QMessageBox::warning(
            this,
            "No Customers",
            "Please register a customer before adding a vehicle."
        );
        return;
    }

    QStringList customerNames;

    for (const Customer &customer : customers) {
        customerNames.append(
            customer.fullName + " (ID: " +
            QString::number(customer.customerId) + ")"
        );
    }

    bool ok = false;

    QString selectedCustomer = QInputDialog::getItem(
        this,
        "Select Customer",
        "Customer:",
        customerNames,
        0,
        false,
        &ok
    );

    if (!ok) return;

    int customerIndex = customerNames.indexOf(selectedCustomer);

    if (customerIndex < 0) return;

    vehicle.customerId = customers[customerIndex].customerId;
    vehicle.ownerName = customers[customerIndex].fullName;

    vehicle.registrationNumber = QInputDialog::getText(
        this, "Add Vehicle",
        "Registration Number:",
        QLineEdit::Normal, "", &ok
    ).trimmed().toUpper();

    if (!ok) return;

    vehicle.brand = QInputDialog::getText(
        this, "Add Vehicle", "Brand:",
        QLineEdit::Normal, "", &ok
    ).trimmed();

    if (!ok) return;

    vehicle.model = QInputDialog::getText(
        this, "Add Vehicle", "Model:",
        QLineEdit::Normal, "", &ok
    ).trimmed();

    if (!ok) return;

    vehicle.batteryCapacity = QInputDialog::getDouble(
        this, "Add Vehicle",
        "Battery Capacity (kWh):",
        50.0, 0.1, 1000.0, 1, &ok
    );

    if (!ok) return;

    QStringList connectors = {
        "TYPE1", "TYPE2", "CCS", "CHADEMO"
    };

    vehicle.connectorType = QInputDialog::getItem(
        this,
        "Add Vehicle",
        "Connector Type:",
        connectors,
        0,
        false,
        &ok
    );

    if (!ok) return;

    vehicle.status = "ACTIVE";

    if (vehicle.registrationNumber.isEmpty() ||
        vehicle.brand.isEmpty() ||
        vehicle.model.isEmpty()) {

        QMessageBox::warning(
            this, "Invalid Input",
            "Registration, brand and model are required."
        );
        return;
    }

    if (repository.addVehicle(vehicle)) {
        QMessageBox::information(
            this, "Success",
            "Vehicle added successfully."
        );

        loadVehicles();
    } else {
        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}

void VehiclesPage::editVehicle()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a vehicle first."
        );
        return;
    }

    int vehicleId = table->item(row, 0)->text().toInt();

    Vehicle vehicle;
    vehicle.vehicleId = vehicleId;

    bool ok = false;

    vehicle.ownerName = QInputDialog::getText(
        this,
        "Edit Vehicle",
        "Owner Name:",
        QLineEdit::Normal,
        table->item(row, 1)->text(),
        &ok
    ).trimmed();

    if (!ok) return;

    if (vehicle.ownerName.isEmpty()) {
        QMessageBox::warning(
            this, "Invalid Input", "Owner name is required.");
        return;
    }

    vehicle.registrationNumber = QInputDialog::getText(
        this, "Edit Vehicle",
        "Registration Number:",
        QLineEdit::Normal,
        table->item(row, 1)->text(), &ok
    ).trimmed().toUpper();

    if (!ok) return;

    vehicle.brand = QInputDialog::getText(
        this, "Edit Vehicle", "Brand:",
        QLineEdit::Normal,
        table->item(row, 2)->text(), &ok
    ).trimmed();

    if (!ok) return;

    vehicle.model = QInputDialog::getText(
        this, "Edit Vehicle", "Model:",
        QLineEdit::Normal,
        table->item(row, 3)->text(), &ok
    ).trimmed();

    if (!ok) return;

    vehicle.batteryCapacity = QInputDialog::getDouble(
        this, "Edit Vehicle",
        "Battery Capacity (kWh):",
        table->item(row, 4)->text().toDouble(),
        0.1, 1000.0, 1, &ok
    );

    if (!ok) return;

    QStringList connectors = {
        "TYPE1",
        "TYPE2",
        "CCS",
        "CHADEMO"
    };

    int connectorIndex = connectors.indexOf(
        table->item(row, 5)->text()
    );

    if (connectorIndex < 0) connectorIndex = 0;

    vehicle.connectorType = QInputDialog::getItem(
        this,
        "Add Vehicle",
        "Connector Type:",
        connectors,
        0,
        false,
        &ok
    );

    if (!ok) return;

    QStringList statuses = {"ACTIVE", "INACTIVE"};

    int statusIndex = statuses.indexOf(
        table->item(row, 6)->text()
    );

    if (statusIndex < 0) statusIndex = 0;

    vehicle.status = QInputDialog::getItem(
        this, "Edit Vehicle",
        "Status:",
        statuses, statusIndex, false, &ok
    );

    if (!ok) return;

    if (vehicle.registrationNumber.isEmpty() ||
        vehicle.brand.isEmpty() ||
        vehicle.model.isEmpty()) {
        QMessageBox::warning(
            this, "Invalid Input",
            "Registration, brand and model are required."
        );
        return;
    }

    if (repository.updateVehicle(vehicle)) {
        QMessageBox::information(
            this, "Success", "Vehicle updated successfully."
        );

        loadVehicles();
    } else {
        QMessageBox::critical(
            this, "Database Error", repository.lastError()
        );
    }
}

void VehiclesPage::deleteVehicle()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a vehicle first."
        );
        return;
    }

    int vehicleId = table->item(row, 0)->text().toInt();

    QString registration = table->item(row, 1)->text();

    QMessageBox::StandardButton reply =
        QMessageBox::question(
            this,
            "Confirm Delete",
            "Delete vehicle " + registration + "?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes) return;

    if (repository.deleteVehicle(vehicleId)) {
        QMessageBox::information(
            this, "Success", "Vehicle deleted successfully."
        );

        loadVehicles();
    } else {
        QMessageBox::critical(
            this, "Database Error", repository.lastError()
        );
    }
}

void VehiclesPage::searchVehicles()
{
    QString searchText = searchInput->text().trimmed();

    if (searchText.isEmpty()) {
        loadVehicles();
        return;
    }

    QList<Vehicle> vehicles = repository.getAllVehicles();

    table->setRowCount(0);

    for (const Vehicle &vehicle : vehicles) {
        if (!vehicle.registrationNumber.contains(
                searchText, Qt::CaseInsensitive)) {
            continue;
        }

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(QString::number(vehicle.vehicleId)));
        table->setItem(row, 1,
            new QTableWidgetItem(vehicle.ownerName));
        table->setItem(row, 2,
            new QTableWidgetItem(vehicle.registrationNumber));

        table->setItem(row, 3,
            new QTableWidgetItem(vehicle.brand));

        table->setItem(row, 4,
            new QTableWidgetItem(vehicle.model));

        table->setItem(row, 5,
            new QTableWidgetItem(
                QString::number(vehicle.batteryCapacity, 'f', 1)
            ));

        table->setItem(row, 6,
            new QTableWidgetItem(vehicle.connectorType));

        table->setItem(row, 7,
            new QTableWidgetItem(vehicle.status));
        
    }
}
