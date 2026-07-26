#ifndef ADMINREVIEW_H
#define ADMINREVIEW_H

#include <QDialog>
#include <QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>


namespace Ui {
class adminreview;
}

class adminreview : public QDialog
{
    Q_OBJECT

public:
    explicit adminreview(QWidget *parent = nullptr);
    ~adminreview();

private slots:
    void refreshReviews();

private:
    Ui::adminreview *ui;
    int reviewCount;
    QString reviewUsername;
    QString reviewBody;
    QString reviewEntryText;

    void loadReviews();
};

#endif // ADMINREVIEW_H
