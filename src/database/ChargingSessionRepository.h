#ifndef CHARGINGSESSIONREPOSITORY_H
#define CHARGINGSESSIONREPOSITORY_H

#include "../models/ChargingSession.h"

#include <QList>
#include <QString>

class ChargingSessionRepository
{
public:
    QList<ChargingSession> getAllSessions();

    bool startSession(int bookingId);

    bool completeSession(
        int sessionId,
        double energyConsumed
    );

    bool cancelSession(int sessionId);

    QString lastError() const;

private:
    QString m_lastError;
};

#endif