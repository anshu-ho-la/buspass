#ifndef ADMINMAINWINDOW_H
#define ADMINMAINWINDOW_H

#include <QMainWindow>
#include <QDate>

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

    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    QString newBusName;
    QString newRoute;
    QString newSeats;
    QString newAvailable;
    QString newDeparture;
    QString newPrice;

    int rowBusID;
    QString rowBusName;
    QString rowRoute;
    QString rowTotalSeats;
    QString rowAvailableSeats;
    QString rowDepartureTime;
    double rowPrice;

    QString filterText;
    QDate filterFromDate;
    QDate filterToDate;

    void loadBuses();
    void deleteBus(int busID);
    void setSearchMode(SearchMode mode);
    void applyActiveFilter();
    void clearBusFilter();
};

#endif