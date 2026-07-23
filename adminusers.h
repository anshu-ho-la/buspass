#ifndef ADMINUSERS_H
#define ADMINUSERS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class adminusers; }
QT_END_NAMESPACE

class adminusers : public QDialog
{
    Q_OBJECT

public:
    explicit adminusers(QWidget *parent = nullptr);
    ~adminusers();

private:
    Ui::adminusers *ui;

    void loadUsers();   // pulls every registered user (passenger and admin) from the database
    void filterUsers(const QString &text);
};

#endif // ADMINUSERS_H
