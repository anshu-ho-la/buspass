#ifndef ADMINPROFILE_H
#define ADMINPROFILE_H

#include <QDialog>
#include <QSqlError>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>


namespace Ui {
class adminprofile;
}

class adminprofile : public QDialog
{
    Q_OBJECT

public:
    explicit adminprofile(QWidget *parent = nullptr);
    ~adminprofile();

private slots:
    void onChangeUsernameClicked();
    void onChangePasswordClicked();
    void onCreateAdminClicked();

private:
    Ui::adminprofile *ui;
    QString currentusername;
    QString currentName;
    QString currentEmail;

    void adminInfo();
    bool verifyCurrentPassword(QString password);

};

#endif // ADMINPROFILE_H
