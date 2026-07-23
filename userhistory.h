#ifndef USERHISTORY_H
#define USERHISTORY_H

#include <QMainWindow>

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

    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    void loadBookings();
    void editBooking(int bookingId, int busID, int currentSeats);
    void downloadTicket(int bookingId, const QString &busName, const QString &route,
                        const QString &departureTime, double price, int seatsBooked);
    void setSearchMode(SearchMode mode);
    void filterBookingsByDateRange();
    void applyActiveFilter();
    void clearBookingFilter();
};

#endif // USERHISTORY_H
