#ifndef ADD_H
#define ADD_H

#include <QDialog>

namespace Ui {
class add;
}

class add : public QDialog
{
    Q_OBJECT

public:
    explicit add(QWidget *parent = nullptr);
    ~add();

signals:
    void busAdded(QString busName, QString route, QString seats, QString available, QString departure, QString price);

private slots:
    void on_pushButton_clicked();

private:
    Ui::add *ui;

    QString busName;
    QString route;
    QString seats;
    QString available;
    QString departure;
    QString price;
    bool seatsOk;
    int seatsValue;
    bool priceOk;
    double priceValue;
};

#endif