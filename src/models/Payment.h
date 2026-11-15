#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>
#include <QDateTime>

struct Payment {
    int paymentId = 0;
    int sessionId = 0;
    int customerId = 0;

    double amount = 0.0;

    QString paymentMethod = "UPI";
    QString status = "PENDING";

    QDateTime paidAt;
};

#endif