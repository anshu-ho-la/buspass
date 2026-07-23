#include "adminmainwindow.h"
#include "ui_adminmainwindow.h"
#include "add.h"
#include "login.h"
#include "adminprofile.h"
#include "adminreview.h"
#include "adminhistory.h"
#include "adminusers.h"
#include "pageswitch.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QDateTime>
#include <QHeaderView>
#include <QDebug>
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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindowAdmin)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40);

    addNavBar(ui->centralwidget,
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

    connect(ui->dateFromFilter, &QDateEdit::dateChanged, this, &MainWindow::applyActiveFilter);
    connect(ui->dateToFilter, &QDateEdit::dateChanged, this, &MainWindow::applyActiveFilter);
    connect(ui->routeSearchBar, &QLineEdit::textChanged, this, &MainWindow::applyActiveFilter);
    connect(ui->clearFilterBtn, &QPushButton::clicked, this, &MainWindow::clearBusFilter);

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
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setRowCount(0);

    const QString nowStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    QSqlQuery query;
    query.prepare("SELECT busID, busName, route, totalSeats, availableSeats, departureTime, price "
                  "FROM buses "
                  "ORDER BY (departureTime > :now) DESC, departureTime ASC");
    query.bindValue(":now", nowStr);

    if (!query.exec()) {
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

        QTableWidgetItem *availableItem = new QTableWidgetItem(availableSeats);
        const int totalSeatsValue = totalSeats.toInt();
        const int availableSeatsValue = availableSeats.toInt();
        if (availableSeatsValue <= 0) {
            availableItem->setBackground(QColor("#f8d7da"));
            availableItem->setForeground(QColor("#7a1c1c"));
        } else if (totalSeatsValue > 0 && (double(availableSeatsValue) / totalSeatsValue) <= 0.5) {
            availableItem->setBackground(QColor("#fff3cd"));
            availableItem->setForeground(QColor("#7a5c00"));
        } else {
            availableItem->setBackground(QColor("#c8e6c9"));
            availableItem->setForeground(QColor("#1b5e20"));
        }
        ui->tableWidget->setItem(row, 3, availableItem);

        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(departureTime));
        ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price, 'f', 2)));

        QPushButton *deleteBtn = new QPushButton(alreadyDeparted ? "Departed" : "Delete", ui->tableWidget);
        deleteBtn->setEnabled(!alreadyDeparted);
        deleteBtn->setProperty("tableButton", true);
        if (!alreadyDeparted) {
            deleteBtn->setProperty("danger", true);
        }
        connect(deleteBtn, &QPushButton::clicked, this, [this, busID]() {
            deleteBus(busID);
        });
        ui->tableWidget->setCellWidget(row, 6, deleteBtn);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void MainWindow::setSearchMode(SearchMode mode)
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

void MainWindow::applyActiveFilter()
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

void MainWindow::clearBusFilter()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ui->tableWidget->setRowHidden(row, false);
    }
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



