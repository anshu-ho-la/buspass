#ifndef BOOKINGHISTORY_H
#define BOOKINGHISTORY_H

#include <QMainWindow>
#include <QString>
#include <QVector>

// One booking record
struct Booking {
    int     busID;
    QString route;
    QString departureTime;
    QString arrivalTime;
    int     seatNo;
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class BookingHistory : public QMainWindow
{
    Q_OBJECT

public:
    explicit BookingHistory(QWidget *parent = nullptr);
    ~BookingHistory();

    // Called from booking page to add a booking into history
    void addBooking(int busID, const QString &route,
                    const QString &departureTime, const QString &arrivalTime,
                    int seatNo);

private:
    Ui::MainWindow *ui;

    QVector<Booking> bookings;  // stores all user bookings

    void refreshList();         // rebuilds the display after any change
};

#endif // BOOKINGHISTORY_H
