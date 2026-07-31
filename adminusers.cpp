#include "adminusers.h"
#include "ui_adminusers.h"
#include "pageswitch.h"
#include "login.h"
#include "adminmainwindow.h"
#include "adminhistory.h"
#include "adminprofile.h"
#include "adminreview.h"
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QColor>

adminusers::adminusers(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::adminusers)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    ui->formCard->setAttribute(Qt::WA_StyledBackground, true);

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

    connect(ui->searchBar, &QLineEdit::textChanged, this, &adminusers::filterUsers);

    loadUsers();
}

adminusers::~adminusers()
{
    delete ui;
}

void adminusers::loadUsers()
{
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->setRowCount(0);

    QSqlQuery query("SELECT username, name, email, isAdmin FROM user ORDER BY id");
    if (!query.isActive()) {
        qDebug() << "loadUsers error:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        rowUsername = query.value(0).toString();
        rowName     = query.value(1).toString();
        rowEmail    = query.value(2).toString();
        rowIsAdmin  = query.value(3).toInt() != 0;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(rowUsername));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(rowName));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(rowEmail.isEmpty() ? "-" : rowEmail));

        QTableWidgetItem *roleItem = new QTableWidgetItem(rowIsAdmin ? "Admin" : "Passenger");
        if (rowIsAdmin) {
            roleItem->setBackground(QColor("#cfe3dc"));
        }
        ui->tableWidget->setItem(row, 3, roleItem);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void adminusers::filterUsers(const QString &text)
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        filterRowUsername = ui->tableWidget->item(row, 0) ? ui->tableWidget->item(row, 0)->text() : "";
        filterRowName     = ui->tableWidget->item(row, 1) ? ui->tableWidget->item(row, 1)->text() : "";
        const bool matches = filterRowUsername.contains(text, Qt::CaseInsensitive) || filterRowName.contains(text, Qt::CaseInsensitive);
        ui->tableWidget->setRowHidden(row, !matches);
    }
}
