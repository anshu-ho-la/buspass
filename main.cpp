#include "login.h"
#include "database.h"
#include <QApplication>
#include <QMessageBox>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Bus logo shown in every window's title bar / the taskbar while the app is running
    a.setWindowIcon(QIcon(":/images/bus_logo.png"));

    a.setStyleSheet(
        "QWidget {"
        "    font-family: \"Segoe UI\";"
        "    font-size: 10.5pt;"
        "    color: #202124;"
        "}"
        "QMainWindow, QDialog {"
        "    background-color: #eef3f1;"
        "}"
        "QLabel#titleLabel {"
        "    color: #16324a;"
        "}"
        "QPushButton {"
        "    background-color: #1f5c4d;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 7px 18px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #17493d;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #123a30;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #aab5b2;"
        "    color: #f0f0f0;"
        "}"
        "QLineEdit, QTextEdit, QDateTimeEdit {"
        "    border: 1px solid #c3cbc9;"
        "    border-radius: 5px;"
        "    padding: 6px;"
        "    background: white;"
        "    selection-background-color: #1f5c4d;"
        "}"
        "QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus {"
        "    border: 1px solid #1f5c4d;"
        "}"
        "QTableWidget {"
        "    background: white;"
        "    gridline-color: #dde4e2;"
        "    border: 1px solid #c3cbc9;"
        "    border-radius: 5px;"
        "    selection-background-color: #cfe3dc;"
        "    selection-color: #16324a;"
        "}"
        "QHeaderView::section {"
        "    background-color: #16324a;"
        "    color: white;"
        "    padding: 6px;"
        "    border: none;"
        "    font-weight: bold;"
        "}"
        "QListWidget {"
        "    background: white;"
        "    border: 1px solid #c3cbc9;"
        "    border-radius: 5px;"
        "}"
        "login QWidget#centralwidget {"
        "    background-image: url(:/images/bus_bg.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "}"
        "signup {"
        "    background-image: url(:/images/bus_bg.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "}"
        "QWidget#formCard {"
        "    background-color: rgba(255, 255, 255, 235);"
        "    border-radius: 14px;"
        "    border: 1px solid rgba(255, 255, 255, 120);"
        "}"
        "QLabel#cardTitleLabel {"
        "    color: #16324a;"
        "}"
        );

    if (!Database::connect()) {
        QMessageBox::critical(nullptr, "Error", "Could not connect to database!");
        return -1;
    }

    login w;
    w.showMaximized();
    return QCoreApplication::exec();
}
