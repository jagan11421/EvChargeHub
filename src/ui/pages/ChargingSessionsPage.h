#ifndef CHARGINGSESSIONSPAGE_H
#define CHARGINGSESSIONSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>

#include "../../database/ChargingSessionRepository.h"

class ChargingSessionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChargingSessionsPage(QWidget *parent = nullptr);

private:
    QTableWidget *table;
    QLineEdit *searchInput;

    ChargingSessionRepository repository;

    void setupUI();
    void loadSessions();
    void startSession();
    void completeSession();
    void cancelSession();
    void searchSessions();
};

#endif