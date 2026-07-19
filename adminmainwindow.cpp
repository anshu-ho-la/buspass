#include "adminmainwindow.h"
#include "ui_adminmainwindow.h"
#include "add.h"
#include "login.h"
#include "adminprofile.h"
#include "adminreview.h"
#include "adminhistory.h"
#include "pageswitch.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QDateTime>
#include <QHeaderView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowAdmin)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");

    addNavBar(ui->centralwidget,
              {"Add Buses", "Booking History", "Profile", "Review", "Logout"},
              {
                  [this]() { openPage<MainWindow>(this); },
                  [this]() { openPage<bookings>(this); },
                  [this]() { openPage<adminprofile>(this); },
                  [this]() { openPage<adminreview>(this); },
                  [this]() { openPage<login>(this); }
              });

    loadBuses();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    add *a = new add(this);

    connect(a, &add::busAdded, this, &MainWindow::handleBusAddition);

    a->show();
}


void MainWindow::handleBusAddition(QString busName, QString route, QString seats, QString available, QString departure, QString price)
{
    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO buses (busName, route, totalSeats, availableSeats, departureTime, price) "
        "VALUES (:busName, :route, :totalSeats, :availableSeats, :departureTime, :price)"
        );
    insertQuery.bindValue(":busName", busName);
    insertQuery.bindValue(":route", route);
    insertQuery.bindValue(":totalSeats", seats.toInt());
    insertQuery.bindValue(":availableSeats", available.toInt());
    insertQuery.bindValue(":departureTime", departure);
    insertQuery.bindValue(":price", price.toDouble());

    if (!insertQuery.exec()) {
        QMessageBox::critical(this, "Add Bus", "Failed to save the new bus. Please try again.");
        qDebug() << "Add bus error:" << insertQuery.lastError().text();
        return;
    }

    loadBuses();
}

void MainWindow::loadBuses()
{
    ui->tableWidget->setRowCount(0);

    QSqlQuery query("SELECT busID, busName, route, totalSeats, availableSeats, departureTime, price FROM buses");
    if (!query.isActive()) {
        qDebug() << "loadBuses error:" << query.lastError().text();
        return;
    }

    const QDateTime now = QDateTime::currentDateTime();

    while (query.next()) {
        int busID              = query.value(0).toInt();
        QString busName        = query.value(1).toString();
        QString route           = query.value(2).toString();
        QString totalSeats      = query.value(3).toString();
        QString availableSeats  = query.value(4).toString();
        QString departureTime   = query.value(5).toString();
        double price            = query.value(6).toDouble();

        const QDateTime departureDT = QDateTime::fromString(departureTime, "yyyy-MM-dd HH:mm:ss");
        const bool alreadyDeparted = departureDT.isValid() && departureDT <= now;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(busName));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(route));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(totalSeats));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(availableSeats));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price, 'f', 2)));

        QPushButton *deleteBtn = new QPushButton(alreadyDeparted ? "Departed" : "Delete", ui->tableWidget);
        deleteBtn->setEnabled(!alreadyDeparted);
        connect(deleteBtn, &QPushButton::clicked, this, [this, busID]() {
            deleteBus(busID);
        });
        ui->tableWidget->setCellWidget(row, 6, deleteBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void MainWindow::deleteBus(int busID)
{
    if (QMessageBox::question(this, "Delete Bus",
                              "Are you sure you want to delete this bus?")
        != QMessageBox::Yes) {
        return;
    }

    QSqlQuery deleteQuery;
    deleteQuery.prepare("DELETE FROM buses WHERE busID = :busID");
    deleteQuery.bindValue(":busID", busID);

    if (!deleteQuery.exec()) {
        QMessageBox::critical(this, "Delete Bus", "Failed to delete this bus. Please try again.");
        qDebug() << "deleteBus error:" << deleteQuery.lastError().text();
        return;
    }

    loadBuses();
}



