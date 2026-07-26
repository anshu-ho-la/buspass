#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

namespace Ui {
class signup;
}

class signup : public QWidget
{
    Q_OBJECT

public:
    explicit signup(QWidget *parent = nullptr);
    ~signup();

private slots:
     void on_signup_button_clicked();
    void on_login_button_clicked();


private:
    Ui::signup *ui;

    QString name;
    QString password;
    QString username;
    QString email;
    int matchCount;

    bool checkUsername();
    bool checkEmail();
};

#endif // SIGNUP_H
