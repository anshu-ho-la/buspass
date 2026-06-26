#include "add.h"
#include "ui_add.h"
#include <QMessageBox>

add::add(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
}

add::~add()
{
    delete ui;
}

// CHANGED THIS to match the new declaration in add.h
void add::on_pushButton_clicked()
{
    // 1. Fetch text values using your EXACT object names from the UI panel
    QString id = ui->txtID->text().trimmed();
    QString route = ui->txtRoute->text().trimmed();
    QString seats = ui->txtSeats->text().trimmed();
    QString available = ui->txtSeats->text().trimmed();
    QString departure = ui->txtDep->text().trimmed();
    QString price = ui->lineEdit_6->text().trimmed(); // From your UI screenshot object tree

    // 2. Simple Validation Check
    if(id.isEmpty() || route.isEmpty() || seats.isEmpty() || available.isEmpty() || departure.isEmpty() || price.isEmpty()) {
        QMessageBox::warning(this, "Empty Fields", "Please fill in all details before adding a bus.");
        return;
    }

    // 3. Emit the signal with the collected data
    emit busAdded(id, route, seats, available, departure, price);

    // 4. Close the dialog window after sending data
    this->close();
}