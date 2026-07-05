#ifndef BOOKINGS_H
#define BOOKINGS_H

#include <QDialog>
#include <QString>
#include <QVector>

struct UserBooking {
    QString passenger;
    int     busID;
    QString route;
    QString departureTime;
    QString arrivalTime;
    int     seatNo;
};

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class bookings : public QDialog
{
    Q_OBJECT

public:
    explicit bookings(QWidget *parent = nullptr);
    ~bookings();

    // Called from the booking page after a user books a seat
    void addBooking(const QString &passenger, int busID,
                    const QString &route, const QString &departureTime,
                    const QString &arrivalTime, int seatNo);

private:
    Ui::Dialog          *ui;
    QVector<UserBooking> bookingList;
    void refreshTable();
};

#endif // BOOKINGS_H
