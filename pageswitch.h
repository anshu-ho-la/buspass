#ifndef PAGESWITCH_H
#define PAGESWITCH_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLayout>
#include <QString>
#include <QStringList>
#include <QList>
#include <functional>

template <typename T>
inline void openPage(QWidget *currentWindow)
{
    T *page = new T();
    page->setAttribute(Qt::WA_DeleteOnClose);
    page->show();
    currentWindow->close();
}

// Adds a nav bar with buttons at the top of any page's central widget layout.
// existingLayout must already exist (e.g. ui->centralwidget->layout()).
// labels and slots must be same length and same order.
inline void addNavBar(QWidget *centralWidget,
                      const QStringList &labels,
                      const QList<std::function<void()>> &actions)
{
    QLayout *existingLayout = centralWidget->layout();
    if (!existingLayout) return;

    QHBoxLayout *navLayout = new QHBoxLayout();
    for (int i = 0; i < labels.size(); ++i) {
        QPushButton *btn = new QPushButton(labels[i], centralWidget);
        QObject::connect(btn, &QPushButton::clicked, centralWidget, actions[i]);
        navLayout->addWidget(btn);
    }

    // Insert nav bar as the FIRST item, pushing everything else down
    if (QVBoxLayout *vbox = qobject_cast<QVBoxLayout*>(existingLayout)) {
        vbox->insertLayout(0, navLayout);
    } else {
        // Fallback: wrap nav layout into a widget and add normally
        existingLayout->addItem(navLayout);
    }
}

#endif // PAGESWITCH_H