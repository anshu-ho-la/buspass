#include "adminhistory.h"
#include "ui_adminhistory.h"
#include "pageswitch.h"
#include "login.h"
#include "adminmainwindow.h"
#include "adminprofile.h"
#include "adminreview.h"
#include "adminusers.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QDateTime>
#include <QHeaderView>
#include <QDateEdit>
#include <QMenu>
#include <QDebug>
#include <QTableWidgetItem>

bookings::bookings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(34);

    addNavBar(this,
              {"Add Buses", "Booking History", "Profile", "Review", "Users", "Logout"},
              {
                  [this]() { openPage<MainWindow>(this); },
                  [this]() { openPage<bookings>(this); },
                  [this]() { openPage<adminprofile>(this); },
                  [this]() { openPage<adminreview>(this); },
                  [this]() { openPage<adminusers>(this); },
                  [this]() { openPage<login>(this); }
              });
    ui->dateToFilter->setDate(QDate::currentDate().addYears(1));

    QMenu *searchMenu = new QMenu(this);
    searchMenu->addAction("Search by Date", this, [this]() { setSearchMode(SearchMode::Date); });
    searchMenu->addAction("Search by Route", this, [this]() { setSearchMode(SearchMode::Route); });
    ui->searchModeBtn->setMenu(searchMenu);

    connect(ui->dateFromFilter, &QDateEdit::dateChanged, this, &bookings::applyActiveFilter);
    connect(ui->dateToFilter, &QDateEdit::dateChanged, this, &bookings::applyActiveFilter);
    connect(ui->searchBar, &QLineEdit::textChanged, this, &bookings::applyActiveFilter);
    connect(ui->clearFilterBtn, &QPushButton::clicked, this, &bookings::clearBusFilter);

    loadbookings();
}

bookings::~bookings()
{
    delete ui;
}

void bookings::loadbookings()
{
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setRowCount(0);

    QSqlQuery query(
        "SELECT user.username, bookings.busName, bookings.route, "
        "bookings.departureTime, bookings.price, bookings.seatsBooked "
        "FROM bookings "
        "JOIN user ON bookings.userid = user.id "
        "ORDER BY bookings.id DESC"
        );

    if (!query.isActive()) {
        qDebug() << "loadBookings error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        const QString passenger     = query.value(0).toString();
        const QString busName       = query.value(1).toString();
        const QString route         = query.value(2).toString();
        const QString departureTime = query.value(3).toString();
        const double price          = query.value(4).toDouble();
        const int seatsBooked       = query.value(5).toInt();

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(passenger));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(busName));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(route));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(price, 'f', 2)));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(seatsBooked)));
        ui->tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(price * seatsBooked, 'f', 2)));
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void bookings::setSearchMode(SearchMode mode)
{
    currentSearchMode = mode;

    const bool dateMode = (mode == SearchMode::Date);
    ui->fromLabel->setVisible(dateMode);
    ui->dateFromFilter->setVisible(dateMode);
    ui->toLabel->setVisible(dateMode);
    ui->dateToFilter->setVisible(dateMode);
    ui->searchBar->setVisible(!dateMode);

    applyActiveFilter();
}

void bookings::applyActiveFilter()
{
    if (currentSearchMode == SearchMode::Route) {
        const QString text = ui->searchBar->text();
        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QTableWidgetItem *routeItem = ui->tableWidget->item(row, 2);
            const bool matches = routeItem && routeItem->text().contains(text, Qt::CaseInsensitive);
            ui->tableWidget->setRowHidden(row, !matches);
        }
        return;
    }

    const QDate fromDate = ui->dateFromFilter->date();
    const QDate toDate = ui->dateToFilter->date();
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QTableWidgetItem *departureItem = ui->tableWidget->item(row, 3);
        if (!departureItem) {
            ui->tableWidget->setRowHidden(row, true);
            continue;
        }
        const QDateTime departureDT = QDateTime::fromString(departureItem->text(), "yyyy-MM-dd HH:mm:ss");
        const bool matches = departureDT.isValid() && departureDT.date() >= fromDate && departureDT.date() <= toDate;
        ui->tableWidget->setRowHidden(row, !matches);
    }
}

void bookings::clearBusFilter()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, false);
    }
}
