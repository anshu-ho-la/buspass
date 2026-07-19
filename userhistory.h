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

    void loadBookings();
    void editBooking(int bookingId, int busID, int currentSeats);
};

#endif // USERHISTORY_H
