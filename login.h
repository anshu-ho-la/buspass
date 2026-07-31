#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class login;
}
QT_END_NAMESPACE

class login : public QMainWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

private slots:
    void on_login_button_clicked();
    void on_signup_button_clicked();

private:
    Ui::login *ui;

    QString username;
    QString password;
    int id;
    int isadmin;

    bool checkCredentials();
};
#endif // LOGIN_H
