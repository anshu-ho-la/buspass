#include "userprofile.h"
#include "ui_userprofile.h"
#include "session.h"
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>

userprofile::userprofile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userprofile),
    currentusername(""),
    currentEmail("")
{
    ui->setupUi(this);
    int loggedInId = Session::instance().id();
    if (loggedInId != -1) {
        QSqlQuery query;
        query.prepare("SELECT username FROM user WHERE id = :id");
        query.bindValue(":id", loggedInId);
        if (query.exec() && query.next()) {
            currentusername = query.value(0).toString();
        } else {
            qDebug() << "userprofile: could not resolve username"
                     << query.lastError().text();
        }
    } else {
        qDebug() << "userprofile: no user logged in (Session id == -1)";
    }
    userInfo();

    // FIX: was "onChangesernameClicked" (typo) — corrected to "onChangeUsernameClicked"
    connect(ui->updateUsernameBtn, &QPushButton::clicked, this, &userprofile::onChangeUsernameClicked);
    connect(ui->updatePasswordBtn, &QPushButton::clicked, this, &userprofile::onChangePasswordClicked);
}

userprofile::~userprofile()
{
    delete ui;
}


void userprofile::userInfo()
{
    QSqlQuery query;
    query.prepare("SELECT email FROM user WHERE username = :username");
    query.bindValue(":username", currentusername);

    if (query.exec() && query.next()) {
        // FIX: was query.value(2) — email is the only selected column so index is 0
        QString email = query.value(0).toString();
        currentEmail  = email;
        ui->UserNameLabel->setText(currentusername);
        ui->displayusername->setText(currentusername);
        ui->displayemail->setText(currentEmail);
    } else {
        // FIX: was missing semicolon after setText(currentusername)
        ui->displayusername->setText(currentusername);
        qDebug() << "UserInfo error:" << query.lastError().text();
    }
}

void userprofile::onChangeUsernameClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Change Username");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *label = new QLabel("New Username:", &dialog);
    QLineEdit *newUsernameEdit = new QLineEdit(&dialog);
    newUsernameEdit->setPlaceholderText("Enter new username");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    layout->addWidget(label);
    layout->addWidget(newUsernameEdit);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString newusername = newUsernameEdit->text().trimmed();

    if (newusername.isEmpty()) {
        // FIX: was "::warning(this,QMessageBox" — inverted/broken syntax
        QMessageBox::warning(this, "Update Username", "Username field cannot be empty.");
        return;
    }
    if (newusername == currentusername) {
        QMessageBox::information(this, "Update Username", "That is already your current username.");
        return;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE user SET username = :newname WHERE username = :oldname");
    updateQuery.bindValue(":newname", newusername);
    updateQuery.bindValue(":oldname", currentusername);

    if (updateQuery.exec()) {
        currentusername = newusername;
        ui->displayusername->setText(currentusername);
        ui->UserNameLabel->setText(currentusername);

        QMessageBox::information(this, "Update Username",
                                 "Username updated successfully to \"" + currentusername + "\".");
        qDebug() << "Username changed to:" << currentusername;
    } else {
        QMessageBox::critical(this, "Update Username", "Failed to update username. Please try again.");
        qDebug() << "Username update error:" << updateQuery.lastError().text();
    }
}

void userprofile::onChangePasswordClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Change Password");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *currentLabel = new QLabel("Current Password:", &dialog);
    QLineEdit *currentEdit = new QLineEdit(&dialog);
    currentEdit->setEchoMode(QLineEdit::Password);

    QLabel *newLabel = new QLabel("New Password:", &dialog);
    QLineEdit *newEdit = new QLineEdit(&dialog);
    newEdit->setEchoMode(QLineEdit::Password);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    layout->addWidget(currentLabel);
    layout->addWidget(currentEdit);
    layout->addWidget(newLabel);
    layout->addWidget(newEdit);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString currentpassword = currentEdit->text();
    QString newpassword     = newEdit->text();

    if (currentpassword.isEmpty() || newpassword.isEmpty()) {
        QMessageBox::warning(this, "Update Password", "All password fields must be filled.");
        return;
    }
    if (newpassword == currentpassword) {
        QMessageBox::information(this, "Update Password", "New password must differ from current password.");
        return;
    }
    if (!verifyCurrentPassword(currentpassword)) {
        QMessageBox::critical(this, "Update Password", "Current password is incorrect.");
        return;
    }

    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE user SET password = :newpassword WHERE username = :username");
    updateQuery.bindValue(":newpassword", newpassword);
    updateQuery.bindValue(":username", currentusername);

    if (updateQuery.exec()) {
        QMessageBox::information(this, "Update Password", "Password updated successfully.");
        // FIX: was "currentUsername" (wrong case) — corrected to "currentusername"
        qDebug() << "Password updated for:" << currentusername;
    } else {
        QMessageBox::critical(this, "Update Password", "Failed to update password. Please try again.");
        qDebug() << "Password update error:" << updateQuery.lastError().text();
    }
}

bool userprofile::verifyCurrentPassword(QString enteredPassword)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM user WHERE username = :username");
    query.bindValue(":username", currentusername);

    if (query.exec() && query.next()) {
        return query.value(0).toString() == enteredPassword;
    }
    return false;
}