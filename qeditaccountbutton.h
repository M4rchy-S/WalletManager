#ifndef QEDITACCOUNTBUTTON_H
#define QEDITACCOUNTBUTTON_H

#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
    class QEditAccountButton;
}
QT_END_NAMESPACE

class QEditAccountButton : QPushButton
{
    private:
        Ui::QEditAccountButton *ui;
    protected:
        void hitButton(QPoint &pos);
    public:
        QEditAccountButton();
        ~QEditAccountButton();
};

#endif // QEDITACCOUNTBUTTON_H
