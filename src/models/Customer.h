
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>

struct Customer {
    int customerId = 0;
    QString fullName;
    QString email;
    QString phone;
    QString address;
    QString status = "ACTIVE";
};

#endif
