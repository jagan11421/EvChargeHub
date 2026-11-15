#ifndef REPORTSPAGE_H
#define REPORTSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>

class ReportsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsPage(QWidget *parent = nullptr);

private:
    QLabel *totalRevenueLabel;
    QLabel *totalSessionsLabel;
    QLabel *totalEnergyLabel;
    QLabel *paidPaymentsLabel;

    QTableWidget *monthlyRevenueTable;

    void setupUI();
    void loadReports();
};

#endif