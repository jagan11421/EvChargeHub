#include "PaymentsPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

PaymentsPage::PaymentsPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    loadPayments();
}

void PaymentsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *title = new QLabel("Payments");
    title->setStyleSheet(
        "font-size: 24px; font-weight: bold;"
    );

    mainLayout->addWidget(title);

    searchInput = new QLineEdit();
    searchInput->setPlaceholderText("Search by Payment ID...");

    connect(searchInput, &QLineEdit::textChanged,
            this, &PaymentsPage::searchPayments);

    mainLayout->addWidget(searchInput);

    table = new QTableWidget();
    table->setColumnCount(7);

    table->setHorizontalHeaderLabels({
        "Payment ID",
        "Session ID",
        "Customer",
        "Amount",
        "Method",
        "Status",
        "Paid At"
    });

    table->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch
    );

    table->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    table->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    table->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    mainLayout->addWidget(table);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *createButton =
        new QPushButton("Generate Payment");

    QPushButton *paidButton =
        new QPushButton("Mark as Paid");

    QPushButton *failedButton =
        new QPushButton("Mark as Failed");

    QPushButton *refundButton =
        new QPushButton("Refund");

    QPushButton *refreshButton =
        new QPushButton("Refresh");

    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(paidButton);
    buttonLayout->addWidget(failedButton);
    buttonLayout->addWidget(refundButton);
    buttonLayout->addWidget(refreshButton);

    mainLayout->addLayout(buttonLayout);

    connect(createButton, &QPushButton::clicked,
            this, &PaymentsPage::createPayment);

    connect(paidButton, &QPushButton::clicked,
            this, &PaymentsPage::markAsPaid);

    connect(failedButton, &QPushButton::clicked,
            this, &PaymentsPage::markAsFailed);

    connect(refundButton, &QPushButton::clicked,
            this, &PaymentsPage::refundPayment);

    connect(refreshButton, &QPushButton::clicked,
            this, &PaymentsPage::loadPayments);
}

void PaymentsPage::loadPayments()
{
    QList<Payment> payments = repository.getAllPayments();

    table->setRowCount(0);

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    for (const Payment &payment : payments) {

        query.prepare(R"(
            SELECT full_name
            FROM customers
            WHERE customer_id = :customerId
        )");

        query.bindValue(":customerId", payment.customerId);

        QString customerName = "Unknown";

        if (query.exec() && query.next()) {
            customerName = query.value(0).toString();
        }

        int row = table->rowCount();
        table->insertRow(row);

        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::number(payment.paymentId)
            ));

        table->setItem(row, 1,
            new QTableWidgetItem(
                QString::number(payment.sessionId)
            ));

        table->setItem(row, 2,
            new QTableWidgetItem(customerName)
        );

        table->setItem(row, 3,
            new QTableWidgetItem(
                QString::number(payment.amount, 'f', 2)
            ));

        table->setItem(row, 4,
            new QTableWidgetItem(payment.paymentMethod)
        );

        table->setItem(row, 5,
            new QTableWidgetItem(payment.status)
        );

        table->setItem(row, 6,
            new QTableWidgetItem(
                payment.paidAt.isValid()
                    ? payment.paidAt.toString("dd-MM-yyyy hh:mm")
                    : "-"
            )
        );
    }
}

void PaymentsPage::createPayment()
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(R"(
        SELECT session_id
        FROM charging_sessions
        WHERE status = 'COMPLETED'
          AND session_id NOT IN (
              SELECT session_id FROM payments
          )
        ORDER BY session_id DESC
    )")) {
        QMessageBox::critical(
            this, "Database Error", query.lastError().text()
        );
        return;
    }

    QStringList sessions;

    while (query.next()) {
        sessions.append(query.value(0).toString());
    }

    if (sessions.isEmpty()) {
        QMessageBox::information(
            this,
            "No Sessions",
            "There are no completed sessions awaiting payment."
        );
        return;
    }

    bool ok = false;

    QString selectedSession = QInputDialog::getItem(
        this,
        "Generate Payment",
        "Select completed session:",
        sessions,
        0,
        false,
        &ok
    );

    if (!ok || selectedSession.isEmpty()) {
        return;
    }

    int sessionId = selectedSession.toInt();

    if (repository.createPaymentForSession(sessionId)) {
        QMessageBox::information(
            this, "Success", "Payment generated successfully."
        );
        loadPayments();
    } else {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}

void PaymentsPage::markAsPaid()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Select Payment", "Please select a payment."
        );
        return;
    }

    int paymentId =
        table->item(row, 0)->text().toInt();

    QString status = table->item(row, 5)->text();

    if (status != "PENDING") {
        QMessageBox::warning(
            this, "Invalid Action",
            "Only pending payments can be marked as paid."
        );
        return;
    }

    if (repository.updatePaymentStatus(paymentId, "PAID")) {
        QMessageBox::information(
            this, "Success", "Payment marked as paid."
        );
        loadPayments();
    } else {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}

void PaymentsPage::markAsFailed()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Select Payment", "Please select a payment."
        );
        return;
    }

    int paymentId =
        table->item(row, 0)->text().toInt();

    QString status = table->item(row, 5)->text();

    if (status != "PENDING") {
        QMessageBox::warning(
            this, "Invalid Action",
            "Only pending payments can be marked as failed."
        );
        return;
    }

    if (repository.updatePaymentStatus(paymentId, "FAILED")) {
        loadPayments();
    } else {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}

void PaymentsPage::refundPayment()
{
    int row = table->currentRow();

    if (row < 0) {
        QMessageBox::warning(
            this, "Select Payment", "Please select a payment."
        );
        return;
    }

    int paymentId =
        table->item(row, 0)->text().toInt();

    QString status = table->item(row, 5)->text();

    if (status != "PAID") {
        QMessageBox::warning(
            this, "Invalid Action",
            "Only paid payments can be refunded."
        );
        return;
    }

    QMessageBox::StandardButton confirmation =
        QMessageBox::question(
            this,
            "Confirm Refund",
            "Are you sure you want to refund this payment?"
        );

    if (confirmation != QMessageBox::Yes) {
        return;
    }

    if (repository.updatePaymentStatus(paymentId, "REFUNDED")) {
        QMessageBox::information(
            this, "Success", "Payment refunded."
        );
        loadPayments();
    } else {
        QMessageBox::critical(
            this, "Error", repository.lastError()
        );
    }
}

void PaymentsPage::searchPayments()
{
    QString searchText = searchInput->text().trimmed();

    for (int row = 0; row < table->rowCount(); ++row) {
        bool match = searchText.isEmpty() ||
            table->item(row, 0)->text().contains(
                searchText, Qt::CaseInsensitive
            );

        table->setRowHidden(row, !match);
    }
}