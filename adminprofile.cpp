#include "adminprofile.h"
#include "ui_adminprofile.h"
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

adminprofile::adminprofile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::adminprofile),
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
    adminInfo();

    connect(ui->updateUsernameBtn, &QPushButton::clicked, this, &adminprofile::onChangeUsernameClicked);
    connect(ui->updatePasswordBtn, &QPushButton::clicked, this, &adminprofile::onChangePasswordClicked);
    connect(ui->createAdminBtn,    &QPushButton::clicked, this, &adminprofile::onCreateAdminClicked);
}

adminprofile::~adminprofile()
{
    delete ui;
}


void adminprofile::adminInfo()
{
    QSqlQuery query;
    query.prepare("SELECT email FROM user WHERE username = :username");
    query.bindValue(":username", currentusername);

    if (query.exec() && query.next()) {
        QString email = query.value(0).toString();
        currentEmail  = email;
        ui->adminNameLabel->setText(currentusername);
        ui->displayusername->setText(currentusername);
        ui->displayemail->setText(currentEmail);
    } else {
        ui->displayusername->setText(currentusername);
        qDebug() << "AdminInfo error:" << query.lastError().text();
    }
}

void adminprofile::onChangeUsernameClicked()
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
        ui->adminNameLabel->setText(currentusername);

        QMessageBox::information(this, "Update Username",
                                 "Username updated successfully to \"" + currentusername + "\".");
        qDebug() << "Username changed to:" << currentusername;
    } else {
        QMessageBox::critical(this, "Update Username", "Failed to update username. Please try again.");
        qDebug() << "Username update error:" << updateQuery.lastError().text();
    }
}

void adminprofile::onChangePasswordClicked()
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
        qDebug() << "Password updated for:" << currentusername;
    } else {
        QMessageBox::critical(this, "Update Password", "Failed to update password. Please try again.");
        qDebug() << "Password update error:" << updateQuery.lastError().text();
    }
}

bool adminprofile::verifyCurrentPassword(QString enteredPassword)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM user WHERE username = :username");
    query.bindValue(":username", currentusername);

    if (query.exec() && query.next()) {
        return query.value(0).toString() == enteredPassword;
    }
    return false;
}

void adminprofile::onCreateAdminClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Create Admin Account");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QLabel *userLabel = new QLabel("Username:", &dialog);
    QLineEdit *userEdit = new QLineEdit(&dialog);
    userEdit->setPlaceholderText("Enter username");

    QLabel *passLabel = new QLabel("Password:", &dialog);
    QLineEdit *passEdit = new QLineEdit(&dialog);
    passEdit->setEchoMode(QLineEdit::Password);
    passEdit->setPlaceholderText("Enter password");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

    layout->addWidget(userLabel);
    layout->addWidget(userEdit);
    layout->addWidget(passLabel);
    layout->addWidget(passEdit);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QString username = userEdit->text().trimmed();
    QString password = passEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Create Admin Account", "All fields are required to create an account.");
        return;
    }

    QSqlQuery insertQuery;
    insertQuery.prepare("INSERT INTO user (username, password, name, email, isAdmin) VALUES (:username, :password, :name, :email, :isAdmin)");
    insertQuery.bindValue(":username", username);
    insertQuery.bindValue(":password", password);
    insertQuery.bindValue(":name",     username);   // use username as display name
    insertQuery.bindValue(":email",    "");
    insertQuery.bindValue(":isAdmin",  1);

    if (insertQuery.exec()) {
        QMessageBox::information(this, "Create Admin Account",
                                 "Admin account \"" + username + "\" created successfully.");
        qDebug() << "New admin created:" << username;
    } else {
        QMessageBox::critical(this, "Create Admin Account", "Failed to create admin account. Try again.");
        qDebug() << "Create admin error:" << insertQuery.lastError().text();
    }
}