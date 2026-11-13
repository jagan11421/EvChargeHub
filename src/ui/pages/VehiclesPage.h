
#ifndef VEHICLESPAGE_H
#define VEHICLESPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "../../database/VehicleRepository.h"

class VehiclesPage : public QWidget
{
    Q_OBJECT

public:
    explicit VehiclesPage(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QLineEdit *searchInput;

    VehicleRepository repository;

    void setupUI();
    void loadVehicles();
    void addVehicle();
    void editVehicle();
    void deleteVehicle();
    void searchVehicles();
};

#endif
