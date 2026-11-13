#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;
class QButtonGroup;
class QStackedWidget;
class BookingsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QButtonGroup *navGroup;
    QStackedWidget *pages;

    BookingsPage *bookingsPage = nullptr;

    QPushButton* createNavButton(
        const QString &text,
        int index
    );

    void setupUI();
};

#endif