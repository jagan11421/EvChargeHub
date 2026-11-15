#include "PaymentRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDateTime>

QList<Payment> PaymentRepository::getAllPayments()
{
    QList<Payment> payments;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(R"(
        SELECT
            payment_id,
            session_id,
            customer_id,
            amount,
            payment_method,
            status,
            paid_at
        FROM payments
        ORDER BY payment_id DESC
    )")) {
        m_lastError = query.lastError().text();
        return payments;
    }

    while (query.next()) {
        Payment payment;

        payment.paymentId = query.value(0).toInt();
        payment.sessionId = query.value(1).toInt();
        payment.customerId = query.value(2).toInt();
        payment.amount = query.value(3).toDouble();
        payment.paymentMethod = query.value(4).toString();
        payment.status = query.value(5).toString();

        if (!query.value(6).isNull()) {
            payment.paidAt = QDateTime::fromString(
                query.value(6).toString(),
                Qt::ISODate
            );
        }

        payments.append(payment);
    }

    m_lastError.clear();
    return payments;
}

bool PaymentRepository::createPaymentForSession(int sessionId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    // Only completed charging sessions can be billed.
    query.prepare(R"(
        SELECT customer_id, cost, status
        FROM charging_sessions
        WHERE session_id = :sessionId
    )");

    query.bindValue(":sessionId", sessionId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (!query.next()) {
        m_lastError = "Charging session not found.";
        return false;
    }

    int customerId = query.value(0).toInt();
    double amount = query.value(1).toDouble();
    QString sessionStatus = query.value(2).toString();

    if (sessionStatus != "COMPLETED") {
        m_lastError =
            "Payment can only be created for completed sessions.";
        return false;
    }

    query.prepare(R"(
        INSERT INTO payments (
            session_id,
            customer_id,
            amount,
            payment_method,
            status
        )
        VALUES (
            :sessionId,
            :customerId,
            :amount,
            'UPI',
            'PENDING'
        )
    )");

    query.bindValue(":sessionId", sessionId);
    query.bindValue(":customerId", customerId);
    query.bindValue(":amount", amount);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}

bool PaymentRepository::updatePaymentStatus(
    int paymentId,
    const QString &status)
{
    if (status != "PAID" &&
        status != "FAILED" &&
        status != "REFUNDED") {
        m_lastError = "Invalid payment status.";
        return false;
    }

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(R"(
        UPDATE payments
        SET
            status = :status,
            paid_at = CASE
                WHEN :status = 'PAID'
                THEN CURRENT_TIMESTAMP
                ELSE paid_at
            END
        WHERE payment_id = :paymentId
    )");

    query.bindValue(":status", status);
    query.bindValue(":paymentId", paymentId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        m_lastError = "Payment not found or no changes made.";
        return false;
    }

    m_lastError.clear();
    return true;
}

QString PaymentRepository::lastError() const
{
    return m_lastError;
}