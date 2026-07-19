#include "usermainwindow.h"
#include "ui_usermainwindow.h"
#include "session.h"
#include "pageswitch.h"
#include "login.h"
#include "userhistory.h"
#include "userprofile.h"
#include "userreview.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QDateTime>
#include <QHeaderView>
#include <QDebug>

MainWindowUser::MainWindowUser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowUserForm)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");

    addNavBar(ui->centralwidget,
              {"Book a bus", "Booking History", "Profile", "review", "Logout"},
              {
                  [this]() { openPage<MainWindowUser>(this); },
                  [this]() { openPage<BookingHistory>(this); },
                  [this]() { openPage<userprofile>(this); },
                  [this]() { openPage<userreview>(this); },
                  [this]() { openPage<login>(this); }
              });

    loadBuses();
}

MainWindowUser::~MainWindowUser()
{
    delete ui;
}

void MainWindowUser::loadBuses()
{
    ui->tableWidget->setRowCount(0);

    const QString nowStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QSqlQuery query;
    query.prepare("SELECT busID, busName, route, totalSeats, availableSeats, departureTime, price "
                  "FROM buses WHERE departureTime > :now ORDER BY departureTime ASC");
    query.bindValue(":now", nowStr);

    if (!query.exec()) {
        qDebug() << "loadBuses error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        int busID               = query.value(0).toInt();
        QString busName         = query.value(1).toString();
        QString route           = query.value(2).toString();
        int totalSeats          = query.value(3).toInt();
        int availableSeats      = query.value(4).toInt();
        QString departureTime   = query.value(5).toString();
        double price            = query.value(6).toDouble();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(busName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(route));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(totalSeats)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(availableSeats)));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price, 'f', 2)));

        QPushButton *bookBtn = new QPushButton(availableSeats > 0 ? "Book" : "Full", ui->tableWidget);
        bookBtn->setEnabled(availableSeats > 0);
        connect(bookBtn, &QPushButton::clicked, this, [this, busID, busName, route, departureTime, price]() {
            bookSeat(busID, busName, route, departureTime, price);
        });
        ui->tableWidget->setCellWidget(row, 6, bookBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindowUser::bookSeat(int busID, const QString &busName, const QString &route,
                              const QString &departureTime, double price)
{
    int loggedInId = Session::instance().id();
    if (loggedInId == -1) {
        QMessageBox::warning(this, "Book Seat", "You must be logged in to book a seat.");
        return;
    }

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT availableSeats FROM buses WHERE busID = :busID");
    checkQuery.bindValue(":busID", busID);
    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::critical(this, "Book Seat", "Could not find this bus. Please refresh.");
        return;
    }

    int availableSeats = checkQuery.value(0).toInt();
    if (availableSeats <= 0) {
        QMessageBox::warning(this, "Book Seat", "No seats available on this bus.");
        loadBuses();
        return;
    }

    bool ok = false;
    int numSeats = QInputDialog::getInt(
        this,
        "Book Seats",
        QString("How many seats would you like to book? (%1 available)").arg(availableSeats),
        1,
        1,
        availableSeats,
        1,
        &ok
        );

    if (!ok) {
        return;
    }

    QSqlQuery existingQuery;
    existingQuery.prepare("SELECT id, seatsBooked FROM bookings WHERE userid = :userid AND busID = :busID");
    existingQuery.bindValue(":userid", loggedInId);
    existingQuery.bindValue(":busID", busID);

    if (!existingQuery.exec()) {
        QMessageBox::critical(this, "Book Seat", "Failed to book seats. Please try again.");
        qDebug() << "bookSeat lookup error:" << existingQuery.lastError().text();
        return;
    }

    if (existingQuery.next()) {
        // Already have a booking for this bus - add to it instead of creating a duplicate row
        const int existingId = existingQuery.value(0).toInt();
        const int existingSeats = existingQuery.value(1).toInt();

        QSqlQuery updateBookingQuery;
        updateBookingQuery.prepare("UPDATE bookings SET seatsBooked = :seats WHERE id = :id");
        updateBookingQuery.bindValue(":seats", existingSeats + numSeats);
        updateBookingQuery.bindValue(":id", existingId);

        if (!updateBookingQuery.exec()) {
            QMessageBox::critical(this, "Book Seat", "Failed to book seats. Please try again.");
            qDebug() << "bookSeat merge error:" << updateBookingQuery.lastError().text();
            loadBuses();
            return;
        }
    } else {
        QSqlQuery insertQuery;
        insertQuery.prepare(
            "INSERT INTO bookings (userid, busID, busName, route, departureTime, seatsBooked, price) "
            "VALUES (:userid, :busID, :busName, :route, :departureTime, :seatsBooked, :price)"
            );
        insertQuery.bindValue(":userid", loggedInId);
        insertQuery.bindValue(":busID", busID);
        insertQuery.bindValue(":busName", busName);
        insertQuery.bindValue(":route", route);
        insertQuery.bindValue(":departureTime", departureTime);
        insertQuery.bindValue(":seatsBooked", numSeats);
        insertQuery.bindValue(":price", price);

        if (!insertQuery.exec()) {
            QMessageBox::critical(this, "Book Seat", "Failed to book seats. Please try again.");
            qDebug() << "bookSeat insert error:" << insertQuery.lastError().text();
            loadBuses();
            return;
        }
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE buses SET availableSeats = availableSeats - :numSeats WHERE busID = :busID");
    updateQuery.bindValue(":numSeats", numSeats);
    updateQuery.bindValue(":busID", busID);
    updateQuery.exec();

    QMessageBox::information(this, "Book Seat",
        QString("%1 seat%2 booked successfully!").arg(numSeats).arg(numSeats > 1 ? "s" : ""));
    loadBuses();
}