#ifndef ADMINHISTORY_H
#define ADMINHISTORY_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class bookings : public QDialog
{
    Q_OBJECT

public:
    explicit bookings(QWidget *parent = nullptr);
    ~bookings();

private:
    Ui::Dialog *ui;


    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    void loadbookings();
    void setSearchMode(SearchMode mode);
    void applyActiveFilter();
    void clearBusFilter();
};

#endif