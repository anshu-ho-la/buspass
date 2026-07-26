#ifndef USERHISTORY_H
#define USERHISTORY_H

#include <QMainWindow>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class BookingHistoryForm; }
QT_END_NAMESPACE

class BookingHistory : public QMainWindow
{
    Q_OBJECT

public:
    explicit BookingHistory(QWidget *parent = nullptr);
    ~BookingHistory();

private:
    Ui::BookingHistoryForm *ui;

    QString currentUsername;
    QString currentName;
    int loggedInId;

    int rowBookingId;
    int rowBusID;
    QString rowBusName;
    QString rowRoute;
    QString rowDepartureTime;
    double rowPrice;
    int rowSeatsBooked;

    QString filterText;
    QDate filterFromDate;
    QDate filterToDate;

    int editBookingId;
    int editBusID;
    int editCurrentSeats;
    int editAvailableSeats;
    int editMaxSeats;
    bool editOk;
    int editNewSeats;
    int editDelta;

    int ticketBookingId;
    QString ticketBusName;
    QString ticketRoute;
    QString ticketDepartureTime;
    double ticketPrice;
    int ticketSeatsBooked;

    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    void loadBookings();
    void editBooking();
    void downloadTicket();
    void setSearchMode(SearchMode mode);
    void filterBookingsByDateRange();
    void applyActiveFilter();
    void clearBookingFilter();
};

#endif // USERHISTORY_H
