#ifndef PAYMENTREPOSITORY_H
#define PAYMENTREPOSITORY_H

#include <QList>
#include <QString>

#include "../models/Payment.h"

class PaymentRepository
{
public:
    QList<Payment> getAllPayments();

    bool createPaymentForSession(int sessionId);

    bool updatePaymentStatus(int paymentId, const QString &status);

    QString lastError() const;

private:
    QString m_lastError;
};

#endif