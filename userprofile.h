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
    // FIX: renamed to match actual method names used in .cpp
    void onChangeUsernameClicked();
    void onChangePasswordClicked();

private:
    Ui::userprofile *ui;

    QString currentusername;
    QString currentEmail;

    void userInfo();
    bool verifyCurrentPassword(QString password);


};

#endif // USERPROFILE_H
