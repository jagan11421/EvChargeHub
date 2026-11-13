
#include "MainWindow.h"

#include "pages/DashboardPage.h"
#include "pages/PlaceholderPage.h"
#include "pages/StationsPage.h"
#include "pages/ChargingPointsPage.h"
#include "pages/VehiclesPage.h"
#include "pages/CustomersPage.h"
#include "pages/BookingsPage.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QButtonGroup>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

QPushButton* MainWindow::createNavButton(
    const QString &text,
    int index
)
{
    QPushButton *button = new QPushButton(text);

    button->setCheckable(true);
    button->setMinimumHeight(45);
    button->setCursor(Qt::PointingHandCursor);

    button->setStyleSheet(
        "QPushButton {"
        "background: transparent;"
        "color: #D1D5DB;"
        "border: none;"
        "border-radius: 8px;"
        "text-align: left;"
        "padding-left: 18px;"
        "font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "background: #374151;"
        "color: white;"
        "}"
        "QPushButton:checked {"
        "background: #2563EB;"
        "color: white;"
        "font-weight: bold;"
        "}"
    );

    navGroup->addButton(button, index);

    return button;
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    QWidget *sidebar = new QWidget;
    sidebar->setFixedWidth(230);
    sidebar->setStyleSheet("background: #111827;");

    QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);

    sideLayout->setContentsMargins(12, 15, 12, 15);
    sideLayout->setSpacing(8);

    QLabel *logo = new QLabel("⚡ EV ChargeHub");

    logo->setStyleSheet(
        "color: white;"
        "font-size: 20px;"
        "font-weight: bold;"
        "padding: 15px;"
    );

    sideLayout->addWidget(logo);

    QLabel *menuLabel = new QLabel("MAIN MENU");

    menuLabel->setStyleSheet(
        "color: #9CA3AF;"
        "font-size: 11px;"
        "padding: 10px 15px;"
    );

    sideLayout->addWidget(menuLabel);

    navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);

    QStringList menuItems = {
        "Dashboard",
        "Stations",
        "Charging Points",
        "Vehicles",
        "Customers",
        "Bookings",
        "Charging Sessions",
        "Payments",
        "Reports"
    };

    for (int i = 0; i < menuItems.size(); i++) {
        QPushButton *button =
            createNavButton(menuItems[i], i);

        sideLayout->addWidget(button);
    }

    sideLayout->addStretch();

    // Page container
    pages = new QStackedWidget;

    pages->addWidget(new DashboardPage());       // 0
    pages->addWidget(new StationsPage());        // 1
    pages->addWidget(new ChargingPointsPage());  // 2
    pages->addWidget(new VehiclesPage());        // 3
    pages->addWidget(new CustomersPage());       // 4

    // Bookings page - create only once
    bookingsPage = new BookingsPage();
    pages->addWidget(bookingsPage);              // 5

    pages->addWidget(
        new PlaceholderPage("Charging Sessions")
    );                                            // 6

    pages->addWidget(
        new PlaceholderPage("Payments")
    );                                            // 7

    pages->addWidget(
        new PlaceholderPage("Reports")
    );                                            // 8

    // Navigation functionality
    connect(
        navGroup,
        &QButtonGroup::idClicked,
        pages,
        &QStackedWidget::setCurrentIndex
    );

    navGroup->button(0)->setChecked(true);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(pages, 1);

    setWindowTitle("EV ChargeHub");
    resize(1200, 750);
}
