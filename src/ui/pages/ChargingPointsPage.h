
#ifndef CHARGINGPOINTSPAGE_H
#define CHARGINGPOINTSPAGE_H

#include <QWidget>

#include "../../database/ChargingPointRepository.h"

class QTableWidget;

class ChargingPointsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChargingPointsPage(QWidget *parent = nullptr);

private:
    QTableWidget *pointTable;

    ChargingPointRepository repository;

    void setupUI();
    void refreshTable();

    void addPoint();
    void editPoint();
    void deletePoint();
};

#endif
