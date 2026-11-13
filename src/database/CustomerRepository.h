
#ifndef CUSTOMERREPOSITORY_H
#define CUSTOMERREPOSITORY_H

#include <QList>
#include <QString>

#include "../models/Customer.h"

class CustomerRepository
{
public:
    QList<Customer> getAllCustomers();

    bool addCustomer(const Customer &customer);
    bool updateCustomer(const Customer &customer);
    bool deleteCustomer(int customerId);

    QString lastError() const;

private:
    QString m_lastError;
};

#endif
