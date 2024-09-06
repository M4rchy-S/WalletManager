#ifndef ACCOUNTWIDGET_H
#define ACCOUNTWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
    class AccountWidget;
}
QT_END_NAMESPACE

class AccountWidget : public QWidget
{
    \Q_OBJECT
private:
    Ui::Widget *ui;
    std::string AccountName;
public:
    explicit AccountWidget(QWidget *parent = nullptr);
    ~AccountWidget();
};

#endif // ACCOUNTWIDGET_H
