#ifndef ACCOUNTCARDWIDGET_H
#define ACCOUNTCARDWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QColor>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AccountCardWidget;
}
QT_END_NAMESPACE

class AccountCardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AccountCardWidget(QWidget *parent = nullptr);
    //~AccountCardWidget();
    void setAccountName(std::string);
    std::string getAccountName();

protected:
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *event) override;

signals:
    void MouseInSignal();
    void MouseOutSignal();

private:
    Ui::AccountCardWidget *ui;
    std::string AccountName;
};

#endif // ACCOUNTCARDWIDGET_H
