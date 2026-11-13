
#ifndef CUSTOMERSPAGE_H
#define CUSTOMERSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "../../database/CustomerRepository.h"

class CustomersPage : public QWidget
{
    Q_OBJECT

public:
    explicit CustomersPage(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QLineEdit *searchInput;

    CustomerRepository repository;

    void setupUI();
    void loadCustomers();
    void addCustomer();
    void editCustomer();
    void deleteCustomer();
    void searchCustomers();
};

#endif
