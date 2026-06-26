#ifndef MAINWINDOWUSER_H
#define MAINWINDOWUSER_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // THIS LINE IS CRITICAL: It declares the receiver slot for the data packet
    void handleBusAddition(QString id, QString route, QString seats, QString available, QString departure, QString price);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOWUSER_H
