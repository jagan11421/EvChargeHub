
#include "PlaceholderPage.h"

#include <QVBoxLayout>
#include <QLabel>

PlaceholderPage::PlaceholderPage(
    const QString &pageTitle,
    QWidget *parent
) : QWidget(parent)
{
    setStyleSheet("background: #F3F4F6;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 25, 30, 25);

    QLabel *heading = new QLabel(pageTitle);

    heading->setStyleSheet(
        "font-size: 28px;"
        "font-weight: bold;"
        "color: #111827;"
    );

    QLabel *description = new QLabel(
        "Manage your " + pageTitle.toLower() +
        " in EV ChargeHub."
    );

    description->setStyleSheet(
        "font-size: 14px;"
        "color: #6B7280;"
    );

    layout->addWidget(heading);
    layout->addWidget(description);
    layout->addStretch();
}
