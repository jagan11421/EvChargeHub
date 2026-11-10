
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QButtonGroup;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *pages;
    QButtonGroup *navGroup;

    void setupUI();

    QPushButton* createNavButton(
        const QString &text,
        int index
    );
};

#endif
