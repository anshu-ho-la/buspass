#include "bookinghistory.h"
#include "ui_bookinghistory.h"

BookingHistory::BookingHistory(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("My Booking History");
    refreshList();
}

BookingHistory::~BookingHistory()
{
    delete ui;
}

void BookingHistory::addBooking(int busID, const QString &route,
                                const QString &departureTime, const QString &arrivalTime,
                                int seatNo)
{
    Booking b;
    b.busID         = busID;
    b.route         = route;
    b.departureTime = departureTime;
    b.arrivalTime   = arrivalTime;
    b.seatNo        = seatNo;
    bookings.append(b);
    refreshList();
}

void BookingHistory::refreshList()
{
    // Clear all 5 list widgets
    ui->listWidget->clear();
    ui->listWidget_2->clear();
    ui->listWidget_3->clear();
    ui->listWidget_4->clear();
    ui->listWidget_5->clear();

    if (bookings.isEmpty()) {
        ui->listWidget->addItem("No bookings yet.");
        return;
    }

    for (const Booking &b : bookings) {
        ui->listWidget->addItem(b.busID);
        ui->listWidget_2->addItem(b.route);
        ui->listWidget_3->addItem(b.departureTime);
        ui->listWidget_4->addItem(b.arrivalTime);
        ui->listWidget_5->addItem(QString::number(b.seatNo));
    }
}
