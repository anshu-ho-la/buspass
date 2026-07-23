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
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QStandardPaths>
#include <QDateEdit>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QDateTime>
#include <QHeaderView>
#include <QDateEdit>
#include <QMenu>
#include <QDebug>

BookingHistory::BookingHistory(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BookingHistoryForm)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);

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

    ui->dateToFilter->setDate(QDate::currentDate().addYears(1));

    QMenu *searchMenu = new QMenu(this);
    searchMenu->addAction("Search by Date", this, [this]() { setSearchMode(SearchMode::Date); });
    searchMenu->addAction("Search by Route", this, [this]() { setSearchMode(SearchMode::Route); });
    ui->searchModeBtn->setMenu(searchMenu);

    connect(ui->dateFromFilter, &QDateEdit::dateChanged, this, &BookingHistory::applyActiveFilter);
    connect(ui->dateToFilter, &QDateEdit::dateChanged, this, &BookingHistory::applyActiveFilter);
    connect(ui->routeSearchBar, &QLineEdit::textChanged, this, &BookingHistory::applyActiveFilter);
    connect(ui->clearFilterBtn, &QPushButton::clicked, this, &BookingHistory::clearBookingFilter);

    loadBookings();
}

BookingHistory::~BookingHistory()
{
    delete ui;
}

void BookingHistory::loadBookings()
{
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setRowCount(0);

    int loggedInId = Session::instance().id();
    if (loggedInId == -1) {
        return;
    }

    QSqlQuery userQuery;
    userQuery.prepare("SELECT username, name FROM user WHERE id = :id");
    userQuery.bindValue(":id", loggedInId);
    if (userQuery.exec() && userQuery.next()) {
        currentUsername = userQuery.value(0).toString();
        currentName = userQuery.value(1).toString();
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
        editBtn->setProperty("tableButton", true);
        connect(editBtn, &QPushButton::clicked, this, [this, bookingId, busID, seatsBooked]() {
            editBooking(bookingId, busID, seatsBooked);
        });
        ui->tableWidget->setCellWidget(row, 6, editBtn);

        QPushButton *ticketBtn = new QPushButton("Download", ui->tableWidget);
        ticketBtn->setProperty("tableButton", true);
        connect(ticketBtn, &QPushButton::clicked, this, [this, bookingId, busName, route, departureTime, price, seatsBooked]() {
            downloadTicket(bookingId, busName, route, departureTime, price, seatsBooked);
        });
        ui->tableWidget->setCellWidget(row, 7, ticketBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void BookingHistory::filterBookingsByDateRange()
{
    const QDate fromDate = ui->dateFromFilter->date();
    const QDate toDate = ui->dateToFilter->date();
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *departureItem = ui->tableWidget->item(row, 2);
        if (!departureItem) {
            ui->tableWidget->setRowHidden(row, true);
            continue;
        }
        const QDateTime departureDT = QDateTime::fromString(departureItem->text(), "yyyy-MM-dd HH:mm:ss");
        const bool matches = departureDT.isValid() && departureDT.date() >= fromDate && departureDT.date() <= toDate;
        ui->tableWidget->setRowHidden(row, !matches);
    }
}

void BookingHistory::setSearchMode(SearchMode mode)
{
    currentSearchMode = mode;

    const bool dateMode = (mode == SearchMode::Date);
    ui->fromLabel->setVisible(dateMode);
    ui->dateFromFilter->setVisible(dateMode);
    ui->toLabel->setVisible(dateMode);
    ui->dateToFilter->setVisible(dateMode);
    ui->routeSearchBar->setVisible(!dateMode);

    applyActiveFilter();
}

void BookingHistory::applyActiveFilter()
{
    if (currentSearchMode == SearchMode::Route) {
        const QString text = ui->routeSearchBar->text();
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *routeItem = ui->tableWidget->item(row, 1);
            const bool matches = routeItem && routeItem->text().contains(text, Qt::CaseInsensitive);
            ui->tableWidget->setRowHidden(row, !matches);
        }
        return;
    }

    filterBookingsByDateRange();
}

void BookingHistory::clearBookingFilter()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, false);
    }
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

void BookingHistory::downloadTicket(int bookingId, const QString &busName, const QString &route,
                                    const QString &departureTime, double price, int seatsBooked)
{
    const QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultName = QString("BusPass_Ticket_%1.pdf").arg(bookingId);
    const QString filePath = QFileDialog::getSaveFileName(
        this, "Save Ticket", defaultDir + "/" + defaultName, "PDF Files (*.pdf)");

    if (filePath.isEmpty()) {
        return;   // user cancelled the save dialog
    }

    QPdfWriter writer(filePath);
    writer.setPageSize(QPageSize(QPageSize::A5));
    writer.setResolution(300);

    QPainter painter(&writer);
    const int pageWidth = writer.width();
    const int margin = pageWidth / 15;
    int y = margin;

    const QString passengerDisplay = currentName.isEmpty() ? currentUsername : currentName;
    const double totalPrice = price * seatsBooked;

    painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
    painter.drawText(margin, y, "BusPass - E-Ticket");
    y += pageWidth / 10;

    painter.setFont(QFont("Segoe UI", 10));
    const QStringList lines = {
        QString("Booking ID: %1").arg(bookingId),
        QString("Passenger: %1").arg(passengerDisplay),
        QString("Bus Name: %1").arg(busName),
        QString("Route: %1").arg(route),
        QString("Departure Time: %1").arg(departureTime),
        QString("Seats Booked: %1").arg(seatsBooked),
        QString("Price per Seat: Rs. %1").arg(price, 0, 'f', 2),
        QString("Total Price: Rs. %1").arg(totalPrice, 0, 'f', 2),
    };

    const int lineHeight = pageWidth / 14;
    for (const QString &line : lines) {
        painter.drawText(margin, y, line);
        y += lineHeight;
    }

    painter.end();

    QMessageBox::information(this, "Ticket Saved",
                             QString("Your ticket was saved to:\n%1").arg(filePath));
}
