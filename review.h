#ifndef REVIEW_H
#define REVIEW_H

#include <QWidget>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class review;
}
QT_END_NAMESPACE

class review : public QWidget
{
    Q_OBJECT

public:
    explicit review(QWidget *parent = nullptr);
    ~review();

private slots:
    void submitReview();
    void resetForm();

private:
    Ui::review *ui;
    bool setupDatabase();
    bool checkReview(QString reviewtext);
    void setfeedback(QString message);
};

#endif // REVIEW_H

/*
#ifndef REVIEW_H
#define REVIEW_H

#include <QWidget>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>
#include <QMessageBox>

class QLineEdit;
class QComboBox;
class QTextEdit;
class QCheckBox;
class QLabel;
class QDialogButtonBox;

QT_BEGIN_NAMESPACE
namespace Ui{
class review;
}
class review: public QWidget
{
    Q_OBJECT
public:
    explicit review(QWidget *parent = nullptr);
    ~review();
private slots:
    void submitReview();
    void resetForm();
private:
    review *ui;
    bool setupDatabase();
    bool checkReview(QString Reviewpost);
};
#endif // REVIEW_H
*/