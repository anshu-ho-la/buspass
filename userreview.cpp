#include "userreview.h"
#include "ui_userreview.h"
#include "session.h"
#include "pageswitch.h"
#include "login.h"
#include "usermainwindow.h"
#include "userhistory.h"
#include "userprofile.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QSqlError>

userreview::userreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userreview)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    ui->formCard->setAttribute(Qt::WA_StyledBackground, true);

    addNavBar(this,
              {"Book a bus", "Booking History", "Profile", "review", "Logout"},
              {
                  [this]() { openPage<MainWindowUser>(this); },
                  [this]() { openPage<BookingHistory>(this); },
                  [this]() { openPage<userprofile>(this); },
                  [this]() { openPage<userreview>(this); },
                  [this]() { openPage<login>(this); }
              });

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
    const QString body = ui->userreviewTextEdit->toPlainText().trimmed();

    if (body.isEmpty()) {
        setFeedback("Please fill in all fields.");
        return;
    }


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
    ui->userreviewTextEdit->clear();
}

void userreview::setFeedback(QString message)
{
    ui->feedbackLabel->setText(message);
}
