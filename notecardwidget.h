#ifndef NOTECARDWIDGET_H
#define NOTECARDWIDGET_H

#include <QWidget>
#include <QPainter>

#include <mainwindow.h>


QT_BEGIN_NAMESPACE
namespace Ui {
    class NoteCardWidget;
}
QT_END_NAMESPACE

class NoteCardWidget : public QWidget
{
    Q_OBJECT


public:
    explicit NoteCardWidget(QWidget *parent = nullptr);

    void setID(unsigned long ID);
    unsigned long getID();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event) override;

signals:
    void clickedSignal();

private:
    Ui::NoteCardWidget *ui;
    unsigned long ID;
};

#endif // NOTECARDWIDGET_H
