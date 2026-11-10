
#ifndef PLACEHOLDERPAGE_H
#define PLACEHOLDERPAGE_H

#include <QWidget>
#include <QString>

class PlaceholderPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlaceholderPage(
        const QString &pageTitle,
        QWidget *parent = nullptr
    );
};

#endif
