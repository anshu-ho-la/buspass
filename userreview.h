#ifndef USERREVIEW_H
#define USERREVIEW_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QtSql/QSqlQuery>
#include <QSqlError>

namespace Ui {
class userreview;
}

class userreview : public QDialog
{
    Q_OBJECT

public:
    explicit userreview(QWidget *parent = nullptr);
    ~userreview();

private slots:
    void submituserreview();   // FIX — was "submitReview()", didn't match .cpp or connect()
    void resetForm();

private:
    Ui::userreview *ui;
    void setFeedback(QString message);
};

#endif // USERREVIEW_H
