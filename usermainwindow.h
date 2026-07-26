#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>
#include <QDate>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindowUserForm; }
QT_END_NAMESPACE

class MainWindowUser : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowUser(QWidget *parent = nullptr);
    ~MainWindowUser();

private:
    Ui::MainWindowUserForm *ui;
    enum class SearchMode { Date, Route };
    SearchMode currentSearchMode = SearchMode::Date;

    int rowBusID;
    QString rowBusName;
    QString rowRoute;
    int rowTotalSeats;
    int rowAvailableSeats;
    QString rowDepartureTime;
    double rowPrice;

    QString filterText;
    QDate filterFromDate;
    QDate filterToDate;

    int bookBusID;
    QString bookBusName;
    QString bookRoute;
    QString bookDepartureTime;
    double bookPrice;
    int loggedInId;
    int availableSeatsForBus;
    bool seatCountOk;
    int numSeats;
    int existingBookingId;
    int existingBookingSeats;

    void loadBuses();
    void bookSeat();
    void setSearchMode(SearchMode mode);
    void applyActiveFilter();
    void clearBusFilter();
};

#endif // USERMAINWINDOW_H
