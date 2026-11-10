
#ifndef STATIONSPAGE_H
#define STATIONSPAGE_H

#include <QWidget>

#include "../../database/StationRepository.h"

class QTableWidget;

class StationsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StationsPage(QWidget *parent = nullptr);

private:
    QTableWidget *stationTable;

    StationRepository repository;

    void setupUI();
    void refreshTable();

    void addStation();
    void editStation();
    void deleteStation();
};

#endif
