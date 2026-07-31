#include "userhistory.h"
#include "ui_userhistory.h"
#include "session.h"
#include "pageswitch.h"
#include "login.h"
#include "usermainwindow.h"
#include "userprofile.h"
#include "userreview.h"
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
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

    loggedInId = Session::instance().id();
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
        rowBookingId          = query.value(0).toInt();
        rowBusID              = query.value(1).toInt();
        rowBusName        = query.value(2).toString();
        rowRoute          = query.value(3).toString();
        rowDepartureTime  = query.value(4).toString();
        rowPrice           = query.value(5).toDouble();
        rowSeatsBooked        = query.value(6).toInt();

        const QDateTime departureDT = QDateTime::fromString(rowDepartureTime, "yyyy-MM-dd HH:mm:ss");
        const bool alreadyDeparted = departureDT.isValid() && departureDT <= now;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(rowBusName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(rowRoute));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(rowDepartureTime));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(rowPrice, 'f', 2)));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(rowSeatsBooked)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(rowPrice * rowSeatsBooked, 'f', 2)));

        QPushButton *editBtn = new QPushButton(alreadyDeparted ? "Departed" : "Edit", ui->tableWidget);
        editBtn->setEnabled(!alreadyDeparted);
        editBtn->setProperty("tableButton", true);
        connect(editBtn, &QPushButton::clicked, this, [this, bookingId = rowBookingId, busID = rowBusID, seatsBooked = rowSeatsBooked]() {     //lambda capture box
            editBookingId = bookingId;
            editBusID = busID;
            editCurrentSeats = seatsBooked;
            editBooking();
        });
        ui->tableWidget->setCellWidget(row, 6, editBtn);

        QPushButton *ticketBtn = new QPushButton("Download", ui->tableWidget);
        ticketBtn->setProperty("tableButton", true);
        connect(ticketBtn, &QPushButton::clicked, this, [this, bookingId = rowBookingId, busName = rowBusName, route = rowRoute, departureTime = rowDepartureTime, price = rowPrice, seatsBooked = rowSeatsBooked]() {
            ticketBookingId = bookingId;
            ticketBusName = busName;
            ticketRoute = route;
            ticketDepartureTime = departureTime;
            ticketPrice = price;
            ticketSeatsBooked = seatsBooked;
            downloadTicket();
        });
        ui->tableWidget->setCellWidget(row, 7, ticketBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void BookingHistory::filterBookingsByDateRange()
{
    filterFromDate = ui->dateFromFilter->date();
    filterToDate = ui->dateToFilter->date();
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *departureItem = ui->tableWidget->item(row, 2);
        if (!departureItem) {
            ui->tableWidget->setRowHidden(row, true);
            continue;
        }
        const QDateTime departureDT = QDateTime::fromString(departureItem->text(), "yyyy-MM-dd HH:mm:ss");
        const bool matches = departureDT.isValid() && departureDT.date() >= filterFromDate && departureDT.date() <= filterToDate;
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
        filterText = ui->routeSearchBar->text();
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *routeItem = ui->tableWidget->item(row, 1);
            const bool matches = routeItem && routeItem->text().contains(filterText, Qt::CaseInsensitive);
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

void BookingHistory::editBooking()
{
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT availableSeats FROM buses WHERE busID = :busID");
    checkQuery.bindValue(":busID", editBusID);
    if (!checkQuery.exec() || !checkQuery.next()) {
        QMessageBox::critical(this, "Edit Booking", "Could not find this bus. Please refresh.");
        return;
    }

    editAvailableSeats = checkQuery.value(0).toInt();
    editMaxSeats = editAvailableSeats + editCurrentSeats;

    editOk = false;
    editNewSeats = QInputDialog::getInt(this,"Edit Booking",QString("How many seats would you like booked? (currently %1, upto %2 available.)")
            .arg(editCurrentSeats) .arg(editMaxSeats),
        editCurrentSeats,     //starting value
        0,editMaxSeats,1    //min ,max ,stepup arrow
        ,&editOk);   //ok or cancel

    if (!editOk || editNewSeats < 0 || editNewSeats == editCurrentSeats) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();

    if (editNewSeats == 0) {
        if (QMessageBox::question(this, "Cancel Booking",
                                  "Setting this to 0 will cancel the booking entirely. Continue?")
            != QMessageBox::Yes) {
            return;
        }

        if (!db.transaction()) {
            QMessageBox::critical(this, "Edit Booking", "Failed to cancel booking. Please try again.");
            qDebug() << "editBooking transaction start error:" << db.lastError().text();
            return;
        }

        QSqlQuery deleteQuery;
        deleteQuery.prepare("DELETE FROM bookings WHERE id = :id");
        deleteQuery.bindValue(":id", editBookingId);
        if (!deleteQuery.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Edit Booking", "Failed to cancel booking. Please try again.");
            qDebug() << "editBooking delete error:" << deleteQuery.lastError().text();
            return;
        }

        QSqlQuery updateBusQuery;
        updateBusQuery.prepare("UPDATE buses SET availableSeats = availableSeats + :seats WHERE busID = :busID");
        updateBusQuery.bindValue(":seats", editCurrentSeats);
        updateBusQuery.bindValue(":busID", editBusID);
        if (!updateBusQuery.exec()) {
            db.rollback();
            QMessageBox::critical(this, "Edit Booking", "Failed to cancel booking. Please try again.");
            qDebug() << "editBooking cancel-refund error:" << updateBusQuery.lastError().text();
            return;
        }

        if (!db.commit()) {
            db.rollback();
            QMessageBox::critical(this, "Edit Booking", "Failed to cancel booking. Please try again.");
            qDebug() << "editBooking commit error:" << db.lastError().text();
            return;
        }

        QMessageBox::information(this, "Cancel Booking", "Booking cancelled.");
        loadBookings();
        return;
    }

    editDelta = editNewSeats - editCurrentSeats;

    if (!db.transaction()) {
        QMessageBox::critical(this, "Edit Booking", "Failed to update booking. Please try again.");
        qDebug() << "editBooking transaction start error:" << db.lastError().text();
        return;
    }

    QSqlQuery updateBookingQuery;
    updateBookingQuery.prepare("UPDATE bookings SET seatsBooked = :seats WHERE id = :id");
    updateBookingQuery.bindValue(":seats", editNewSeats);
    updateBookingQuery.bindValue(":id", editBookingId);
    if (!updateBookingQuery.exec()) {
        db.rollback();
        QMessageBox::critical(this, "Edit Booking", "Failed to update booking. Please try again.");
        qDebug() << "editBooking update error:" << updateBookingQuery.lastError().text();
        return;
    }

    QSqlQuery updateBusQuery;
    updateBusQuery.prepare("UPDATE buses SET availableSeats = availableSeats - :delta "
                          "WHERE busID = :busID AND availableSeats >= :delta");
    updateBusQuery.bindValue(":delta", editDelta);
    updateBusQuery.bindValue(":busID", editBusID);

    if (!updateBusQuery.exec()) {
        db.rollback();
        QMessageBox::critical(this, "Edit Booking", "Not enough seats remaining on this bus. Please try again.");
        qDebug() << "editBooking seat-decrement error:" << updateBusQuery.lastError().text();
        loadBookings();
        return;
    }

    if (!db.commit()) {
        db.rollback();
        QMessageBox::critical(this, "Edit Booking", "Failed to update booking. Please try again.");
        qDebug() << "editBooking commit error:" << db.lastError().text();
        return;
    }

    QMessageBox::information(this, "Edit Booking", "Booking updated.");
    loadBookings();
}

void BookingHistory::downloadTicket()
{
    const QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString defaultName = QString("BusPass_Ticket_%1.pdf").arg(ticketBookingId);
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

    const double totalPrice = ticketPrice * ticketSeatsBooked;

    painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
    painter.drawText(margin, y, "BusPass - E-Ticket");
    y += pageWidth / 10;

    painter.setFont(QFont("Segoe UI", 10));
    const QStringList lines = {
        QString("Booking ID: %1").arg(ticketBookingId),
        QString("Passenger: %1").arg(currentName),
        QString("Passenger username: %1").arg(currentUsername),
        QString("Bus Name: %1").arg(ticketBusName),
        QString("Route: %1").arg(ticketRoute),
        QString("Departure Time: %1").arg(ticketDepartureTime),
        QString("Seats Booked: %1").arg(ticketSeatsBooked),
        QString("Price per Seat: Rs. %1").arg(ticketPrice, 0, 'f', 2),
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
