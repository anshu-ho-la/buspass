#ifndef ADMINHISTORY_H
#define ADMINHISTORY_H

#include <QDialog>
#include <QDate>

namespace Ui { class Dialog; }

class bookings : public QDialog
{
    Q_OBJECT

public:
    explicit bookings(QWidget *parent = nullptr);
    ~bookings();

private:
    Ui::Dialog *ui;


    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    QString rowPassenger;
    QString rowBusName;
    QString rowRoute;
    QString rowDepartureTime;
    double rowPrice;
    int rowSeatsBooked;
    QString filterText;
    QDate filterFromDate;
    QDate filterToDate;

    void loadbookings();
    void setSearchMode(SearchMode mode);
    void applyActiveFilter();
    void clearBusFilter();
};

#endif