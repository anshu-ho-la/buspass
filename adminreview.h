#ifndef ADMINREVIEW_H
#define ADMINREVIEW_H

#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

namespace Ui {
class adminreview;
}

class adminreview : public QWidget
{
    Q_OBJECT

public:
    explicit adminreview(QWidget *parent = nullptr);
    ~adminreview();

private slots:
    void loadReviews();      // fetches all reviews from DB and fills the list
    void refreshReviews();   // connected to the Refresh button

private:
    Ui::adminreview *ui;
};

#endif // ADMINREVIEW_H