#include "mainwindow.h"
#include "ui_mainwindowadmin.h"
#include "add.h" // Makes sure MainWindow knows what the Add page is
#include "login.h"
#include "pageswitch.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget *innerWidget = ui->centralwidget->findChild<QWidget*>();
    if (innerWidget && innerWidget->layout()) {
        addNavBar(innerWidget,
                  {"Login", "Booking History", "Profile", "Review"},
                  {
                      [this]() { openPage<login>(this); },
                      [this]() { openPage<login>(this); },
                      [this]() { openPage<login>(this); },
                      [this]() { openPage<login>(this); }
                  });    }

    addNavBar(ui->centralwidget,
              {"Login"},
              {
               [this]() { openPage<login>(this); },
               });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Triggers when you click "Add Bus" on your main screen
void MainWindow::on_pushButton_clicked()
{
    add *a = new add(this);

    // THIS LINE HOOKS UP THE WIRE: Connects dialog signal to main window slot
    connect(a, &add::busAdded, this, &MainWindow::handleBusAddition);

    a->show();
}

// THIS FUNCTION HANDLES ADDING THE DATA DYNAMICALLY INTO YOUR TABLE
void MainWindow::handleBusAddition(QString id, QString route, QString seats, QString available, QString departure, QString price)
{
    // 1. Grab current row position count
    int currentRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(currentRow);

    // 2. Insert items sequentially into your table columns (0 to 5)
    ui->tableWidget->setItem(currentRow, 0, new QTableWidgetItem(id));
    ui->tableWidget->setItem(currentRow, 1, new QTableWidgetItem(route));
    ui->tableWidget->setItem(currentRow, 2, new QTableWidgetItem(seats));
    ui->tableWidget->setItem(currentRow, 3, new QTableWidgetItem(available));
    ui->tableWidget->setItem(currentRow, 4, new QTableWidgetItem(departure));
    ui->tableWidget->setItem(currentRow, 5, new QTableWidgetItem(price));
}



