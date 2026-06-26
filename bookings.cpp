#include "bookings.h"
#include "ui_bookings.h"

bookings::bookings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("All Bookings");

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

bookings::~bookings()
{
    delete ui;
}

void bookings::addBooking(const QString &passenger, int busID,
                          const QString &route, const QString &departureTime,
                          const QString &arrivalTime, int seatNo)
{
    UserBooking b;
    b.passenger     = passenger;
    b.busID         = busID;
    b.route         = route;
    b.departureTime = departureTime;
    b.arrivalTime   = arrivalTime;
    b.seatNo        = seatNo;
    bookingList.append(b);
    refreshTable();
}

void bookings::refreshTable()
{
    ui->tableWidget->setRowCount(0);

    for (const UserBooking &b : bookingList) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(b.passenger));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(b.busID));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(b.route));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(b.departureTime));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(b.arrivalTime));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(b.seatNo)));
    }
}
