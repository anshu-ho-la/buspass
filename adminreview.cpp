#include "adminreview.h"
#include "ui_adminreview.h"
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
    // FIX — missing space caused "username,review_textFROM reviews..." (invalid SQL).
    // Also removed reference to non-existent "title" column — your reviews table
    // (see database.h) only has username, review_text, created_at.
    if (!query.exec("SELECT username, review_text FROM reviews ORDER BY id DESC")) {
        qDebug() << "Failed to load reviews:" << query.lastError().text();
        ui->reviewsListWidget->addItem("Could not load reviews from the database.");
        return;
    }

    int count = 0;
    while (query.next()) {
        const QString username = query.value("username").toString();
        const QString body     = query.value("review_text").toString();

        // FIX — "entryText" was used but never defined. Build it here.
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

