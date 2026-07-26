#ifndef USERPROFILE_H
#define USERPROFILE_H

#include <QDialog>
#include <QSqlError>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>


namespace Ui {
class userprofile;
}

class userprofile : public QDialog
{
    Q_OBJECT

public:
    explicit userprofile(QWidget *parent = nullptr);
    ~userprofile();

private slots:
    void onChangeUsernameClicked();
    void onChangePasswordClicked();

private:
    Ui::userprofile *ui;

    QString currentusername;
    QString currentName;
    QString currentEmail;
    int loggedInId;
    QString newusername;
    QString currentpassword;
    QString newpassword;

    void userInfo();
    bool verifyCurrentPassword();


};

#endif // USERPROFILE_H
