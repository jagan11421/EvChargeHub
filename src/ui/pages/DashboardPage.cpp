
#include "DashboardPage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>

DashboardPage::DashboardPage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background: #F3F4F6;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 25, 30, 25);
    layout->setSpacing(20);

    QLabel *heading = new QLabel("Dashboard");
    heading->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    layout->addWidget(heading);

    QLabel *subtitle = new QLabel(
        "Welcome to EV ChargeHub Management System"
    );

    subtitle->setStyleSheet("color: #6B7280; font-size: 14px;");

    layout->addWidget(subtitle);

    QHBoxLayout *cards = new QHBoxLayout;
    cards->setSpacing(15);

    QStringList titles = {
        "Total Customers",
        "Charging Points",
        "Active Sessions",
        "Today's Revenue"
    };

    QStringList values = {
        "250",
        "24",
        "08",
        "₹18,500"
    };

    for (int i = 0; i < titles.size(); i++)
    {
        QFrame *card = new QFrame;
        card->setStyleSheet(
            "QFrame {"
            "background: white;"
            "border-radius: 12px;"
            "}"
        );

        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        QLabel *value = new QLabel(values[i]);
        value->setStyleSheet(
            "font-size: 27px;"
            "font-weight: bold;"
            "color: #111827;"
        );

        QLabel *title = new QLabel(titles[i]);
        title->setStyleSheet(
            "font-size: 13px;"
            "color: #6B7280;"
        );

        cardLayout->addWidget(value);
        cardLayout->addWidget(title);

        cards->addWidget(card);
    }

    layout->addLayout(cards);
    layout->addStretch();
}
