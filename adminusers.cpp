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
        const QString username = query.value(0).toString();
        const QString name     = query.value(1).toString();
        const QString email    = query.value(2).toString();
        const bool isAdmin     = query.value(3).toInt() != 0;

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(username));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(email.isEmpty() ? "-" : email));

        QTableWidgetItem *roleItem = new QTableWidgetItem(isAdmin ? "Admin" : "Passenger");
        if (isAdmin) {
            roleItem->setBackground(QColor("#cfe3dc"));
            roleItem->setForeground(QColor("#16324a"));
        } else {
            roleItem->setBackground(QColor("#f0f5f4"));
            roleItem->setForeground(QColor("#4a5754"));
        }
        ui->tableWidget->setItem(row, 3, roleItem);
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSortingEnabled(true);
}

void adminusers::filterUsers(const QString &text)
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        const QString username = ui->tableWidget->item(row, 0) ? ui->tableWidget->item(row, 0)->text() : "";
        const QString name     = ui->tableWidget->item(row, 1) ? ui->tableWidget->item(row, 1)->text() : "";
        const bool matches = username.contains(text, Qt::CaseInsensitive) || name.contains(text, Qt::CaseInsensitive);
        ui->tableWidget->setRowHidden(row, !matches);
    }
}
