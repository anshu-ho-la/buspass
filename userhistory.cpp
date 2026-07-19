#include "userhistory.h"
#include "ui_userhistory.h"
#include "session.h"
#include "pageswitch.h"
#include "login.h"
#include "usermainwindow.h"
#include "userprofile.h"
#include "userreview.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

BookingHistory::BookingHistory(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BookingHistoryForm)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    addNavBar(ui->centralwidget,
              {"Book a bus", "Booking History", "Profile", "review", "Logout"},
              {
                  [this]() { openPage<MainWindowUser>(this); },
                  [this]() { openPage<BookingHistory>(this); },
                  [this]() { openPage<userprofile>(this); },
                  [this]() { openPage<userreview>(this); },
                  [this]() { openPage<login>(this); }
              });

    loadBookings();
}

BookingHistory::~BookingHistory()
{
    delete ui;
}

void BookingHistory::loadBookings()
{
    ui->tableWidget->setRowCount(0);

    int loggedInId = Session::instance().id();
    if (loggedInId == -1) {
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id, busID, busName, route, departureTime, price, seatsBooked "
                  "FROM bookings WHERE userid = :userid ORDER BY id DESC");
    query.bindValue(":userid", loggedInId);

    if (!query.exec()) {
        qDebug() << "loadBookings error:" << query.lastError().text();
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();

    while (query.next()) {
        const int bookingId          = query.value(0).toInt();
        const int busID              = query.value(1).toInt();
        const QString busName        = query.value(2).toString();
        const QString route          = query.value(3).toString();
        const QString departureTime  = query.value(4).toString();
        const double price           = query.value(5).toDouble();
        const int seatsBooked        = query.value(6).toInt();

        const QDateTime departureDT = QDateTime::fromString(departureTime, "yyyy-MM-dd HH:mm:ss");
        const bool alreadyDeparted = departureDT.isValid() && departureDT <= now;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(busName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(route));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(price, 'f', 2)));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(seatsBooked)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price * seatsBooked, 'f', 2)));

        QPushButton *editBtn = new QPushButton(alreadyDeparted ? "Departed" : "Edit", ui->tableWidget);
        editBtn->setEnabled(!alreadyDeparted);
        connect(editBtn, &QPushButton::clicked, this, [this, bookingId, busID, seatsBooked]() {
            editBooking(bookingId, busID, seatsBooked);
        });
        ui->tableWidget->setCellWidget(row, 6, editBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void BookingHistory::editBooking(int bookingId, int busID, int currentSeats)
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT availableSeats FROM buses WHERE busID = :busID");
    checkQuery.bindValue(":busID", busID);
    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::critical(this, "Edit Booking", "Could not find this bus. Please refresh.");
        return;
    }

    const int availableSeats = checkQuery.value(0).toInt();
    const int maxSeats = availableSeats + currentSeats;

    bool ok = false;
    int newSeats = QInputDialog::getInt(
        this,
        "Edit Booking",
        QString("How many seats would you like booked? (currently %1, %2 available to change to)")
            .arg(currentSeats)
            .arg(maxSeats),
        currentSeats,
        0,
        maxSeats,
        1,
        &ok
        );

    if (!ok || newSeats == currentSeats) {
        return;
    }

    if (newSeats == 0) {
        if (QMessageBox::question(this, "Cancel Booking",
                                  "Setting this to 0 will cancel the booking entirely. Continue?")
            != QMessageBox::Yes) {
            return;
        }

        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM bookings WHERE id = :id");
        deleteQuery.bindValue(":id", bookingId);
        if (!deleteQuery.exec()) {
            QMessageBox::critical(this, "Edit Booking", "Failed to cancel booking. Please try again.");
            qDebug() << "editBooking delete error:" << deleteQuery.lastError().text();
            return;
        }

        QSqlQuery updateBusQuery;
        updateBusQuery.prepare("UPDATE buses SET availableSeats = availableSeats + :seats WHERE busID = :busID");
        updateBusQuery.bindValue(":seats", currentSeats);
        updateBusQuery.bindValue(":busID", busID);
        updateBusQuery.exec();

        QMessageBox::information(this, "Cancel Booking", "Booking cancelled.");
        loadBookings();
        return;
    }

    const int delta = newSeats - currentSeats;

    QSqlQuery updateBookingQuery;
    updateBookingQuery.prepare("UPDATE bookings SET seatsBooked = :seats WHERE id = :id");
    updateBookingQuery.bindValue(":seats", newSeats);
    updateBookingQuery.bindValue(":id", bookingId);
    if (!updateBookingQuery.exec()) {
        QMessageBox::critical(this, "Edit Booking", "Failed to update booking. Please try again.");
        qDebug() << "editBooking update error:" << updateBookingQuery.lastError().text();
        return;
    }

    QSqlQuery updateBusQuery;
    updateBusQuery.prepare("UPDATE buses SET availableSeats = availableSeats - :delta WHERE busID = :busID");
    updateBusQuery.bindValue(":delta", delta);
    updateBusQuery.bindValue(":busID", busID);
    updateBusQuery.exec();

    QMessageBox::information(this, "Edit Booking", "Booking updated.");
    loadBookings();
}
