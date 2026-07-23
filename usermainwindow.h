#ifndef USERMAINWINDOW_H
#define USERMAINWINDOW_H

#include <QMainWindow>

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

    void loadBuses();
    void bookSeat(int busID, const QString &busName, const QString &route,
                  const QString &departureTime, double price);
    void setSearchMode(SearchMode mode);
    void applyActiveFilter();
    void clearBusFilter();
};

#endif // USERMAINWINDOW_H
