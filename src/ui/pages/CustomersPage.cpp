
#include "CustomersPage.h"

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
#include <QRegularExpression>

CustomersPage::CustomersPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadCustomers();
}

void CustomersPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Customer Management");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #ffffff;"
    );

    QLabel *subtitle = new QLabel(
        "Register and manage EV charging customers"
    );
    subtitle->setStyleSheet("color: #aaaaaa;");

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    // Search section
    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText(
        "Search by customer name, email or phone..."
    );

    QPushButton *searchButton = new QPushButton("Search");
    QPushButton *refreshButton = new QPushButton("Refresh");

    searchLayout->addWidget(searchInput);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(refreshButton);

    mainLayout->addLayout(searchLayout);

    // Customer table
    table = new QTableWidget();
    table->setColumnCount(6);

    table->setHorizontalHeaderLabels({
        "ID",
        "Full Name",
        "Email",
        "Phone",
        "Address",
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

    QPushButton *addButton = new QPushButton("Add Customer");
    QPushButton *editButton = new QPushButton("Edit Customer");
    QPushButton *deleteButton = new QPushButton("Delete Customer");

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
            this, &CustomersPage::addCustomer);

    connect(editButton, &QPushButton::clicked,
            this, &CustomersPage::editCustomer);

    connect(deleteButton, &QPushButton::clicked,
            this, &CustomersPage::deleteCustomer);

    connect(searchButton, &QPushButton::clicked,
            this, &CustomersPage::searchCustomers);

    connect(refreshButton, &QPushButton::clicked,
            this, &CustomersPage::loadCustomers);

    connect(searchInput, &QLineEdit::returnPressed,
            this, &CustomersPage::searchCustomers);
}


void CustomersPage::loadCustomers()
{
    searchInput->clear();

    QList<Customer> customers = repository.getAllCustomers();

    table->setRowCount(0);

    for (const Customer &customer : customers) {

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::number(customer.customerId)));

        table->setItem(row, 1,
            new QTableWidgetItem(customer.fullName));

        table->setItem(row, 2,
            new QTableWidgetItem(customer.email));

        table->setItem(row, 3,
            new QTableWidgetItem(customer.phone));

        table->setItem(row, 4,
            new QTableWidgetItem(customer.address));

        table->setItem(row, 5,
            new QTableWidgetItem(customer.status));
    }

    if (!repository.lastError().isEmpty()) {
        QMessageBox::warning(
            this, "Database Error", repository.lastError()
        );
    }
}


void CustomersPage::addCustomer()
{
    Customer customer;
    bool ok = false;

    customer.fullName = QInputDialog::getText(
        this, "Add Customer", "Full Name:",
        QLineEdit::Normal, "", &ok
    ).trimmed();

    if (!ok) return;

    customer.email = QInputDialog::getText(
        this, "Add Customer", "Email Address:",
        QLineEdit::Normal, "", &ok
    ).trimmed().toLower();

    if (!ok) return;

    customer.phone = QInputDialog::getText(
        this, "Add Customer", "Phone Number:",
        QLineEdit::Normal, "", &ok
    ).trimmed();

    if (!ok) return;

    customer.address = QInputDialog::getText(
        this, "Add Customer", "Address:",
        QLineEdit::Normal, "", &ok
    ).trimmed();

    if (!ok) return;

    // Input validation
    if (customer.fullName.isEmpty() ||
        customer.email.isEmpty() ||
        customer.phone.isEmpty()) {

        QMessageBox::warning(
            this, "Invalid Input",
            "Name, email and phone are required."
        );
        return;
    }

    static const QRegularExpression emailPattern(
        R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)"
    );

    if (!emailPattern.match(customer.email).hasMatch()) {
        QMessageBox::warning(
            this, "Invalid Email",
            "Please enter a valid email address."
        );
        return;
    }

    customer.status = "ACTIVE";

    if (repository.addCustomer(customer)) {

        QMessageBox::information(
            this, "Success",
            "Customer registered successfully."
        );

        loadCustomers();

    } else {

        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}


void CustomersPage::editCustomer()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a customer first."
        );
        return;
    }

    Customer customer;

    customer.customerId =
        table->item(row, 0)->text().toInt();

    bool ok = false;

    customer.fullName = QInputDialog::getText(
        this, "Edit Customer", "Full Name:",
        QLineEdit::Normal,
        table->item(row, 1)->text(), &ok
    ).trimmed();

    if (!ok) return;

    customer.email = QInputDialog::getText(
        this, "Edit Customer", "Email:",
        QLineEdit::Normal,
        table->item(row, 2)->text(), &ok
    ).trimmed().toLower();

    if (!ok) return;

    customer.phone = QInputDialog::getText(
        this, "Edit Customer", "Phone:",
        QLineEdit::Normal,
        table->item(row, 3)->text(), &ok
    ).trimmed();

    if (!ok) return;

    customer.address = QInputDialog::getText(
        this, "Edit Customer", "Address:",
        QLineEdit::Normal,
        table->item(row, 4)->text(), &ok
    ).trimmed();

    if (!ok) return;

    QStringList statuses = {"ACTIVE", "INACTIVE"};

    int statusIndex = statuses.indexOf(
        table->item(row, 5)->text()
    );

    if (statusIndex < 0) statusIndex = 0;

    customer.status = QInputDialog::getItem(
        this, "Edit Customer", "Status:",
        statuses, statusIndex, false, &ok
    );

    if (!ok) return;

    if (customer.fullName.isEmpty() ||
        customer.email.isEmpty() ||
        customer.phone.isEmpty()) {

        QMessageBox::warning(
            this, "Invalid Input",
            "Name, email and phone are required."
        );
        return;
    }

    static const QRegularExpression emailPattern(
        R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)"
    );

    if (!emailPattern.match(customer.email).hasMatch()) {
        QMessageBox::warning(
            this, "Invalid Email",
            "Please enter a valid email address."
        );
        return;
    }

    if (repository.updateCustomer(customer)) {

        QMessageBox::information(
            this, "Success",
            "Customer updated successfully."
        );

        loadCustomers();

    } else {

        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}


void CustomersPage::deleteCustomer()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Selection Required",
            "Please select a customer first."
        );
        return;
    }

    int customerId =
        table->item(row, 0)->text().toInt();

    QString customerName =
        table->item(row, 1)->text();

    QMessageBox::StandardButton reply =
        QMessageBox::question(
            this,
            "Confirm Delete",
            "Delete customer " + customerName + "?",
            QMessageBox::Yes | QMessageBox::No
        );

    if (reply != QMessageBox::Yes) return;

    if (repository.deleteCustomer(customerId)) {

        QMessageBox::information(
            this, "Success",
            "Customer deleted successfully."
        );

        loadCustomers();

    } else {

        QMessageBox::critical(
            this, "Database Error",
            repository.lastError()
        );
    }
}


void CustomersPage::searchCustomers()
{
    QString searchText =
        searchInput->text().trimmed();

    if (searchText.isEmpty()) {
        loadCustomers();
        return;
    }

    QList<Customer> customers =
        repository.getAllCustomers();

    table->setRowCount(0);

    for (const Customer &customer : customers) {

        bool matches =
            customer.fullName.contains(
                searchText, Qt::CaseInsensitive) ||
            customer.email.contains(
                searchText, Qt::CaseInsensitive) ||
            customer.phone.contains(searchText);

        if (!matches) continue;

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::number(customer.customerId)));

        table->setItem(row, 1,
            new QTableWidgetItem(customer.fullName));

        table->setItem(row, 2,
            new QTableWidgetItem(customer.email));

        table->setItem(row, 3,
            new QTableWidgetItem(customer.phone));

        table->setItem(row, 4,
            new QTableWidgetItem(customer.address));

        table->setItem(row, 5,
            new QTableWidgetItem(customer.status));
    }
}
