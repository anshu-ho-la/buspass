#include "login.h"
#include "signup.h"
#include "session.h"
#include "ui_login.h"
#include "mainwindow.h"


login::login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowTitle("Login");
}

bool login::checkCredentials(QString username, QString password)
{
    QSqlQuery query;

    query.prepare("SELECT * FROM user WHERE username = :username AND password = :password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.exec();

    if (query.next()) {
        int id = query.value(0).toInt();
        Session::instance().login(id);

        int isadmin = query.value(5).toInt();


        if (isadmin==0){
        MainWindow *d = new MainWindow();
        d->show();
        this->close();
        return true;
        }
        else if (isadmin==1){
        MainWindow *d = new MainWindow();
        d->show();
        this->close();
        return true;
        }
    }

    return false;
}

void login::on_login_button_clicked()
{
    QString username = ui->username->text().trimmed();
    QString password = ui->password->text().trimmed();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter both username and password!");
        return;
    }

    if (checkCredentials(username, password)) {
        QMessageBox::information(this, "Success", "Login successful! Welcome, " + username + "!");
    } else {
        QMessageBox::critical(this, "Failed", "Invalid username or password. Try again.");

    }
}

void login::on_signup_button_clicked()
{
    signup *s = new signup();
    s->show();
    this->close();
}

login::~login()
{
    delete ui;
}
