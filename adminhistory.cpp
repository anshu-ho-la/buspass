#include "adminhistory.h"
#include "ui_adminhistory.h"
#include "pageswitch.h"
#include "login.h"
#include "adminmainwindow.h"
#include "adminprofile.h"
#include "adminreview.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>

bookings::bookings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    addNavBar(this,
              {"Add Buses", "Booking History", "Profile", "Review", "Logout"},
              {
                  [this]() { openPage<MainWindow>(this); },
                  [this]() { openPage<bookings>(this); },
                  [this]() { openPage<adminprofile>(this); },
                  [this]() { openPage<adminreview>(this); },
                  [this]() { openPage<login>(this); }
              });

    loadBookings();
}

bookings::~bookings()
{
    delete ui;
}

void bookings::loadBookings()
{
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
}
