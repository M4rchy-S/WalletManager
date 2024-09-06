#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QCursor>
#include <QImage>
#include <QtCharts>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include <QMessageBox>
#include <QUrl>
#include <QDesktopServices>
#include <QPropertyAnimation>

#include <accountcardwidget.h>
#include <notecardwidget.h>
#include <eWallet.h>

#include <vector>


QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    wlt::eWallet *wallet;
    QTranslator qtLanguageTranslator;
    unsigned long ID = 1;
    unsigned short last_index = 0;

    //      Initialising functions
    void sideBarConnects();
    void formsInit();
    void settingsFormInit();
    void contactFormInit();

    //      Update functions
    void editAccountFormUpdate();
    void addNoteFormUpdate();
    void editNoteFormUpdate(unsigned long id);

    void fillAccountData();
    void addAccountCard(std::string AccName, double Count, std::string Currency);

    void fillNoteData();
    QWidget *addNoteCard(wlt::Operation operation, std::string category, std::string accName, std::string accNameAddit,
                      double value, std::string details, unsigned int day, unsigned int month, unsigned int year, unsigned long ID);

    void fillRatesData();
    void addRateCard(std::string Rate_cur_name, double rate_value);

    void addStatCard(std::string category, std::string icon_color, double value, double percent );
    void addInfoCard();
    std::vector<double> getPercentFromVector(std::vector<double> expenses_data);

    void updateStatPage();

    void fillFullNotesPage();

    void updateSettingsForm();

    //  Utility functions
    QString fromIntToMonth(unsigned int month);
    void removeLayoutWidgets(QLayout* layout);
    QPixmap getWhiteIcon(std::string);
    void connectSlice(QPieSlice *slice);

public slots:
    void StackedWidgetIndexChanged();
    void AccountNameChangedInEditor();

    void OperationTypeChangedInForm();
    void AccountComboBoxChangedInAddNoteForm();
    void OperationTypeChangedInForm_edit();
    void AccountComboBoxChangedInAddNoteForm_edit();

    void CreateAccount();
    void DeleteAccount();
    void SaveAccountEdit();

    void CreateNote();
    void EditNote();
    void DeleteNote();

    void CurrencyChanged();
    void ChangeTranslation();


};
#endif // MAINWINDOW_H
