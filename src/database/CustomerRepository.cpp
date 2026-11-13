
#include "CustomerRepository.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

QList<Customer> CustomerRepository::getAllCustomers()
{
    QList<Customer> customers;

    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    if (!query.exec(
        "SELECT customer_id, full_name, email, phone, "
        "address, status FROM customers "
        "ORDER BY customer_id DESC")) {

        m_lastError = query.lastError().text();
        return customers;
    }

    while (query.next()) {
        Customer customer;

        customer.customerId = query.value(0).toInt();
        customer.fullName = query.value(1).toString();
        customer.email = query.value(2).toString();
        customer.phone = query.value(3).toString();
        customer.address = query.value(4).toString();
        customer.status = query.value(5).toString();

        customers.append(customer);
    }

    m_lastError.clear();
    return customers;
}


bool CustomerRepository::addCustomer(const Customer &customer)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "INSERT INTO customers "
        "(full_name, email, phone, address, status) "
        "VALUES (:name, :email, :phone, :address, :status)"
    );

    query.bindValue(":name", customer.fullName);
    query.bindValue(":email", customer.email);
    query.bindValue(":phone", customer.phone);
    query.bindValue(":address", customer.address);
    query.bindValue(":status", customer.status);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool CustomerRepository::updateCustomer(const Customer &customer)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "UPDATE customers SET "
        "full_name = :name, "
        "email = :email, "
        "phone = :phone, "
        "address = :address, "
        "status = :status "
        "WHERE customer_id = :id"
    );

    query.bindValue(":name", customer.fullName);
    query.bindValue(":email", customer.email);
    query.bindValue(":phone", customer.phone);
    query.bindValue(":address", customer.address);
    query.bindValue(":status", customer.status);
    query.bindValue(":id", customer.customerId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


bool CustomerRepository::deleteCustomer(int customerId)
{
    QSqlDatabase db =
        QSqlDatabase::database("evchargehub_connection");

    QSqlQuery query(db);

    query.prepare(
        "DELETE FROM customers WHERE customer_id = :id"
    );

    query.bindValue(":id", customerId);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_lastError.clear();
    return true;
}


QString CustomerRepository::lastError() const
{
    return m_lastError;
}
