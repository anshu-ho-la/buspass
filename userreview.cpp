#include "userreview.h"
#include "ui_userreview.h"
#include "session.h"              // FIX — needed to resolve logged-in username
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDebug>
#include <QtSql/QSqlQuery>        // FIX — needed for DB insert
#include <QSqlError>              // FIX — needed for error logging

userreview::userreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userreview)
{
    ui->setupUi(this);
    ui->titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 4px;");
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &userreview::submituserreview);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &userreview::resetForm);
}

userreview::~userreview()
{
    delete ui;
}

void userreview::submituserreview()
{
    // FIX — title was never read or validated
    const QString title = ui->userreviewTitleLineEdit->text().trimmed();
    const QString body  = ui->userreviewTextEdit->toPlainText().trimmed();

    if (title.isEmpty() || body.isEmpty()) {
        setFeedback("Please fill in all fields.");
        return;
    }

    // FIX — review was never saved to the database; now resolves
    //        the logged-in username from Session and inserts into reviews table
    QString reviewer = "anonymous";
    int loggedInId = Session::instance().id();
    if (loggedInId != -1) {
        QSqlQuery userQuery;
        userQuery.prepare("SELECT username FROM user WHERE id = :id");
        userQuery.bindValue(":id", loggedInId);
        if (userQuery.exec() && userQuery.next()) {
            reviewer = userQuery.value(0).toString();
        }
    }

    QSqlQuery insertQuery;
    insertQuery.prepare(
        "INSERT INTO reviews (username, review_text) VALUES (:username, :review)"
        );
    insertQuery.bindValue(":username", reviewer);
    insertQuery.bindValue(":review",   body);

    if (insertQuery.exec()) {
        setFeedback("Review submitted successfully!");
        qDebug() << "userreview submitted by:" << reviewer;
        resetForm();
    } else {
        setFeedback("Failed to submit review. Please try again.");
        qDebug() << "userreview insert error:" << insertQuery.lastError().text();
    }
}

void userreview::resetForm()
{
    ui->userreviewTitleLineEdit->clear();
    ui->userreviewTextEdit->clear();
}

void userreview::setFeedback(QString message)
{
    ui->feedbackLabel->setText(message);
}
