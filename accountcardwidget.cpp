#include "accountcardwidget.h"

AccountCardWidget::AccountCardWidget(QWidget *parent)
{

}

void AccountCardWidget::setAccountName(std::string accName)
{
    this->AccountName = accName;
}

std::string AccountCardWidget::getAccountName()
{
    return this->AccountName;
}

void AccountCardWidget::enterEvent(QEnterEvent *event)
{
    this->MouseInSignal();
    update();
}

void AccountCardWidget::leaveEvent(QEvent *event)
{
    this->MouseOutSignal();
    update();
}

void AccountCardWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if(underMouse())
    {

        painter.fillRect(rect(), QColor(53,53,53));
        this->setStyleSheet("QWidget{background-color:rgb(53,53,53);}");
    }
    else
    {
        painter.fillRect(rect(), QColor(0,0,0, 0));
        this->setStyleSheet("QWidget{background-color:rgba(0,0,0,0);}");
    }

}
