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

    if (currentWindow->isMaximized()) {
        page->showMaximized();
    } else {
        page->setGeometry(currentWindow->geometry());
        page->show();
    }

    currentWindow->close();
}

inline void addNavBar(QWidget *centralWidget,const QStringList &labels,const QList<std::function<void()>> &actions)
{
    QLayout *existingLayout = centralWidget->layout();
    if (!existingLayout) return;

    QHBoxLayout *navLayout = new QHBoxLayout();
    for (int i = 0; i < labels.size(); ++i) {
        QPushButton *btn = new QPushButton(labels[i], centralWidget);
        if (labels[i].compare("Logout", Qt::CaseInsensitive) == 0) {
            btn->setProperty("danger", true);
        }
        QObject::connect(btn, &QPushButton::clicked, centralWidget, actions[i]);
        navLayout->addWidget(btn);
    }

    QWidget *contentWrapper = new QWidget(centralWidget);
    contentWrapper->setLayout(existingLayout);

    QVBoxLayout *masterLayout = new QVBoxLayout(centralWidget);
    masterLayout->addLayout(navLayout);
    masterLayout->addWidget(contentWrapper);
    centralWidget->setLayout(masterLayout);
}


#endif // PAGESWITCH_H