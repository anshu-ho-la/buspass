
#include "review.h"
#include "ui_review.h"
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QDebug>

bool review::checkReview(QString reviewtext)
{
    QSqlQuery query;

    query.prepare("SELECT * FROM user WHERE reviewtext = :review");
    query.bindValue(":review", reviewtext);
    query.exec();


    return false;
}

review::review(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::review)
{
    ui->setupUi(this);
    ui->titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; margin-bottom: 4px;");
    connect(ui->buttonBox, &QDialogButtonBox::accepted,this,&review::submitReview);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,this,&review::resetForm);
}
void review::submitReview()
{
    QString reviewtext = ui->reviewTextLineEdit->toPlainText().trimmed();
    if (reviewtext.isEmpty()) {
        setfeedback("Please write a review.");
        return;
    }
    if (checkReview(reviewtext)) {
        setfeedback("Success");
    } else {
        setfeedback("Error");

    }

    qDebug() << " Review Submitted ";
    resetForm();
}
void review::resetForm()
{
    ui->reviewTitleLineEdit->clear();
    ui->reviewTextEdit->clear();
}

void review::setfeedback(QString message){
    ui->feedbackLabel->setText(message);
}

