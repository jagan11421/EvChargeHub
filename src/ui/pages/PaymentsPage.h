#ifndef PAYMENTSPAGE_H
#define PAYMENTSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "../../database/PaymentRepository.h"

class PaymentsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PaymentsPage(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QLineEdit *searchInput;

    PaymentRepository repository;

    void setupUI();
    void loadPayments();
    void createPayment();
    void markAsPaid();
    void markAsFailed();
    void refundPayment();
    void searchPayments();
};

#endif