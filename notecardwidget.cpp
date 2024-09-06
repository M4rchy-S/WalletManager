#include "notecardwidget.h"

NoteCardWidget::NoteCardWidget(QWidget *parent)
{

}

void NoteCardWidget::setID(unsigned long ID)
{
    this->ID = ID;
}

unsigned long NoteCardWidget::getID()
{
    return this->ID;
}

void NoteCardWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //std::cout << "Note id from event: " << this->ID << std::endl;
    this->clickedSignal();
}

void NoteCardWidget::enterEvent(QEnterEvent *event)
{
    update();
}

void NoteCardWidget::leaveEvent(QEvent *event)
{
    update();
}

void NoteCardWidget::paintEvent(QPaintEvent *event)
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
