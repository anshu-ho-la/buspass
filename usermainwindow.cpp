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
#include <QDateEdit>
#include <QMenu>
#include <QDebug>
#include <QColor>

MainWindowUser::MainWindowUser(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowUserForm)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);

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

    connect(ui->dateFromFilter, &QDateEdit::dateChanged, this, &MainWindowUser::applyActiveFilter);
    connect(ui->dateToFilter, &QDateEdit::dateChanged, this, &MainWindowUser::applyActiveFilter);
    connect(ui->routeSearchBar, &QLineEdit::textChanged, this, &MainWindowUser::applyActiveFilter);
    connect(ui->clearFilterBtn, &QPushButton::clicked, this, &MainWindowUser::clearBusFilter);

    loadBuses();
}

MainWindowUser::~MainWindowUser()
{
    delete ui;
}

void MainWindowUser::loadBuses()
{
    ui->tableWidget->setSortingEnabled(false);
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

        QTableWidgetItem *availableItem = new QTableWidgetItem(QString::number(availableSeats));
        if (availableSeats <= 0) {
            availableItem->setBackground(QColor("#f8d7da"));
            availableItem->setForeground(QColor("#7a1c1c"));
        } else if (totalSeats > 0 && (double(availableSeats) / totalSeats) <= 0.5) {
            availableItem->setBackground(QColor("#fff3cd"));
            availableItem->setForeground(QColor("#7a5c00"));
        } else {
            availableItem->setBackground(QColor("#c8e6c9"));
            availableItem->setForeground(QColor("#1b5e20"));
        }
        ui->tableWidget->setItem(row, 3, availableItem);

        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price, 'f', 2)));

        QPushButton *bookBtn = new QPushButton(availableSeats > 0 ? "Book" : "Full", ui->tableWidget);
        bookBtn->setEnabled(availableSeats > 0);
        bookBtn->setProperty("tableButton", true);
        connect(bookBtn, &QPushButton::clicked, this, [this, busID, busName, route, departureTime, price]() {
            bookSeat(busID, busName, route, departureTime, price);
        });
        ui->tableWidget->setCellWidget(row, 6, bookBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void MainWindowUser::setSearchMode(SearchMode mode)
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

void MainWindowUser::applyActiveFilter()
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

    const QDate fromDate = ui->dateFromFilter->date();
    const QDate toDate = ui->dateToFilter->date();
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *departureItem = ui->tableWidget->item(row, 4);
        if (!departureItem) {
            ui->tableWidget->setRowHidden(row, true);
            continue;
        }
        const QDateTime departureDT = QDateTime::fromString(departureItem->text(), "yyyy-MM-dd HH:mm:ss");
        const bool matches = departureDT.isValid() && departureDT.date() >= fromDate && departureDT.date() <= toDate;
        ui->tableWidget->setRowHidden(row, !matches);
    }
}

void MainWindowUser::clearBusFilter()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, false);
    }
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