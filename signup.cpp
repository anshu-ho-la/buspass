#include "signup.h"
#include "login.h"
#include "ui_signup.h"
#include <qregularexpression.h>
signup::signup(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::signup)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");
    setAttribute(Qt::WA_StyledBackground, true);
    ui->formCard->setAttribute(Qt::WA_StyledBackground, true);

    connect(ui->name, &QLineEdit::returnPressed, ui->signup_button, &QPushButton::click);
    connect(ui->username, &QLineEdit::returnPressed, ui->signup_button, &QPushButton::click);
    connect(ui->email, &QLineEdit::returnPressed, ui->signup_button, &QPushButton::click);
    connect(ui->password, &QLineEdit::returnPressed, ui->signup_button, &QPushButton::click);
}

void signup::on_signup_button_clicked()
{
    QString name = ui->name->text().trimmed();
    QString password = ui->password->text().trimmed();
    QString username = ui->username->text().trimmed();
    QString email = ui->email->text().trimmed();

    QRegularExpression rx("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");

    if (name.isEmpty() || password.isEmpty() || username.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter all fields!");
        return;
    }

    if (password.length()<6) {
        QMessageBox::warning(this, "Warning", "Password length must be atleast 6!");
        return;
    }

        if (checkUsername(username)==1) {
        QMessageBox::critical(this, "Failed", "Invalid username already taken. Try again.");

        return;
        }
            else if (checkEmail(email)==1) {
                QMessageBox::critical(this, "Failed", "Invalid email already in use.");

                return;
            }
            else if (!rx.match(email).hasMatch()){
                QMessageBox::critical(this, "Failed", "Invalid email format.");
            }
            else{
                QMessageBox::information(this, "Success", "Signup successful!");

                QSqlQuery query;
                query.prepare("INSERT INTO user (username, password, name, email, isAdmin) VALUES (:username, :password, :name, :email, 0)");
                query.bindValue(":username", username);
                query.bindValue(":password", password);
                query.bindValue(":name", name);
                query.bindValue(":email", email);
                query.exec();

                {
                login *s = new login();
                if (this->isMaximized()) {
                    s->showMaximized();
                } else {
                    s->setGeometry(this->geometry());
                    s->show();
                }
                this->close();
                }
            }

}

bool signup::checkUsername(QString username)
{
    QSqlQuery query;

    query.prepare("SELECT * FROM user WHERE username = :username");
    query.bindValue(":username", username);

    query.exec();

    int n=0;

    if (query.next()) {
        n=1;
    }

    return n;
}

bool signup::checkEmail(QString email)
{
    QSqlQuery query;

    query.prepare("SELECT * FROM user WHERE email = :email");
    query.bindValue(":email", email);

    query.exec();

    int n=0;

    if (query.next()) {
        n=1;
    }

    return n;
}

void signup::on_login_button_clicked()
{
    login *s = new login();
    if (this->isMaximized()) {
        s->showMaximized();
    } else {
        s->setGeometry(this->geometry());
        s->show();
    }
    this->close();
}

signup::~signup()
{
    delete ui;
}
