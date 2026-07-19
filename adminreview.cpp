#include "adminreview.h"
#include "ui_adminreview.h"
#include "pageswitch.h"
#include "login.h"
#include "adminmainwindow.h"
#include "adminhistory.h"
#include "adminprofile.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QSqlError>

adminreview::adminreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminreview)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    ui->formCard->setAttribute(Qt::WA_StyledBackground, true);

    addNavBar(this,
              {"Add Buses", "Booking History", "Profile", "Review", "Logout"},
              {
                  [this]() { openPage<MainWindow>(this); },
                  [this]() { openPage<bookings>(this); },
                  [this]() { openPage<adminprofile>(this); },
                  [this]() { openPage<adminreview>(this); },
                  [this]() { openPage<login>(this); }
              });

    ui->titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 4px;");
    connect(ui->refreshButton, &QPushButton::clicked, this, &adminreview::refreshReviews);
    loadReviews();

}

adminreview::~adminreview()
{
    delete ui;
}


void adminreview::loadReviews()
{
    ui->reviewsListWidget->clear();

    QSqlQuery query;
    if (!query.exec("SELECT username, review_text FROM reviews ORDER BY id DESC")) {
        qDebug() << "Failed to load reviews:" << query.lastError().text();
        ui->reviewsListWidget->addItem("Could not load reviews from the database.");
        return;
    }

    int count = 0;
    while (query.next()) {
        const QString username = query.value("username").toString();
        const QString body     = query.value("review_text").toString();

        const QString entryText = username + ": " + body;

        QListWidgetItem *item = new QListWidgetItem(entryText, ui->reviewsListWidget);
        item->setToolTip(body);
        ++count;
    }

    if (count == 0) {
        ui->reviewsListWidget->addItem("No reviews have been submitted yet.");
    }
}

void adminreview::refreshReviews()
{
    loadReviews();
}

