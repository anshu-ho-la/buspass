#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowAdmin; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void handleBusAddition(QString busName, QString route, QString seats, QString available, QString departure, QString price);

private:
    Ui::MainWindowAdmin *ui;

    void loadBuses();
    void deleteBus(int busID);
};

#endif