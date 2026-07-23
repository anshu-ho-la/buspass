#include "add.h"
#include "ui_add.h"
#include <QMessageBox>
#include <QDateTime>

add::add(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
    setWindowTitle("Buspass");

    ui->dateTimeDep->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeDep->setMinimumDateTime(QDateTime::currentDateTime());
}

add::~add()
{
    delete ui;
}

void add::on_pushButton_clicked()
{
    QString busName = ui->txtRoute_2->text().trimmed();
    QString route = ui->txtRoute->text().trimmed();
    QString seats = ui->txtSeats->text().trimmed();
    QString available = ui->txtSeats->text().trimmed();
    QString departure = ui->dateTimeDep->dateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString price = ui->lineEdit_6->text().trimmed();


    if(busName.isEmpty() || route.isEmpty() || seats.isEmpty() || available.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "Empty Fields", "Please fill in all details before adding a bus.");
        return;
    }

    bool seatsOk = false;
    int seatsValue = seats.toInt(&seatsOk);
    if (!seatsOk || seatsValue <= 0) {
        QMessageBox::warning(this, "Invalid Seats", "Total seats must be a whole number greater than 0.");
        return;
    }

    bool priceOk = false;
    double priceValue = price.toDouble(&priceOk);
    if (!priceOk || priceValue <= 0) {
        QMessageBox::warning(this, "Invalid Price", "Price must be a number greater than 0.");
        return;
    }

    emit busAdded(busName, route, seats, available, departure, price);

    this->close();
}