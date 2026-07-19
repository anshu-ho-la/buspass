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
    ~login() override;

private slots:
    void on_login_button_clicked();
    void on_signup_button_clicked();

private:
    Ui::login *ui;

    bool checkCredentials(QString username, QString password);
};
#endif // LOGIN_H
