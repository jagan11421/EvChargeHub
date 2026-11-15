
#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>

class QLabel;
class QTableWidget;
class QChartView;
class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget *parent = nullptr);

private:
    QLabel *stationsValue;
    QLabel *customersValue;
    QLabel *pointsValue;
    QLabel *sessionsValue;
    QLabel *vehiclesValue;
    QLabel *revenueValue;

    QTableWidget *recentBookingsTable;

    void setupUI();
    void loadDashboardData();
    void loadRecentBookings();
    QChartView *revenueChartView;
    QChartView *sessionChartView;

    void loadCharts();
};

#endif
