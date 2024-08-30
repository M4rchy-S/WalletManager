#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //  Initialising important data
    ui->setupUi(this);
    this->wallet = new wlt::eWallet();

    QObject::connect(ui->mainWindow, &QStackedWidget::currentChanged, this, &MainWindow::StackedWidgetIndexChanged);

    this->sideBarConnects();
    this->formsInit();
    this->settingsFormInit();

    this->fillAccountData();
    this->fillNoteData();
    this->fillRatesData();

}


MainWindow::~MainWindow()
{
    delete this->wallet;
    delete ui;
}

//      Class slots

void MainWindow::StackedWidgetIndexChanged()
{
    if(ui->mainWindow->currentIndex() == 0)
    {
        this->last_index = 0;
        //  main window
        this->fillAccountData();
        this->fillNoteData();
    }
    else if(ui->mainWindow->currentIndex() == 5)
    {
        this->last_index = 5;
        //  Full notes list page update
        this->fillFullNotesPage();
    }
    else if(ui->mainWindow->currentIndex() == 6)
    {
        //  Stat with chart page
        this->updateStatPage();
    }
    else if(ui->mainWindow->currentIndex() == 7)
    {
        this->fillRatesData();
    }
    else if(ui->mainWindow->currentIndex() == 8)
    {
        this->updateSettingsForm();
    }
}

void MainWindow::CreateAccount()
{
    // qDebug() << ui->accNameLineEdit->text();
    // qDebug() << ui->CountLineEdit->text();
    // qDebug() << ui->currencListCombobox->currentText();

    if(ui->accNameLineEdit->text() == "" || ui->CountLineEdit->text() == "" || ui->currencListCombobox->currentText() == "")
        return;


    if(ui->CountLineEdit->text().length() >= 13)
        return;

    if(  this->wallet->inRatesTable( ui->currencListCombobox->currentText().toUpper().toStdString() ) == -1 )
        return;


    double Value = ui->CountLineEdit->text().replace(",",".").toDouble();
    wallet->Create_Account(ui->accNameLineEdit->text().toStdString(), Value, ui->currencListCombobox->currentText().toStdString() );
    ui->mainWindow->setCurrentIndex(0);
}

void MainWindow::AccountNameChangedInEditor()
{
    wlt::Account choosen_account = wallet->getAccountByName( ui->accountNameComboBox_edit->currentText().toStdString());
    ui->currencListCombobox_edit->setCurrentText( QString::fromStdString( choosen_account.getCurrencyType() ) );
    ui->CountLineEdit_edit->setText( QString::number( choosen_account.getCount(), 'f', 2 ) );
}

void MainWindow::OperationTypeChangedInForm()
{
    if(ui->OperationComboBox->currentIndex() == 2)
    {
        ui->AccountLabelAddit->setVisible(true);
        ui->accountAdditComboBox->setVisible(true);
    }
    else
    {
        ui->AccountLabelAddit->setVisible(false);
        ui->accountAdditComboBox->setVisible(false);
        if(ui->OperationComboBox->currentIndex() == 1)
            ui->categoryComboBox->setCurrentIndex(9);
    }
}

void MainWindow::AccountComboBoxChangedInAddNoteForm()
{

    ui->accountAdditComboBox->clear();


    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    wlt::Account selected_acc = wallet->getAccountByName( ui->accountMainComboBox->currentText().toStdString() );
    for(auto acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
    {
        if(acc_iter->getName() != selected_acc.getName() && acc_iter->getCurrencyType() == selected_acc.getCurrencyType())
            ui->accountAdditComboBox->addItem(QString::fromStdString(acc_iter->getName()));
    }
}

void MainWindow::OperationTypeChangedInForm_edit()
{
    if(ui->OperationComboBox_edit->currentIndex() == 2)
    {
        ui->AccountLabelAddit_edit->setVisible(true);
        ui->accountAdditComboBox_edit->setVisible(true);
    }
    else
    {
        ui->AccountLabelAddit_edit->setVisible(false);
        ui->accountAdditComboBox_edit->setVisible(false);
        if(ui->OperationComboBox_edit->currentIndex() == 1)
            ui->categoryComboBox_edit->setCurrentIndex(9);
    }
}
void MainWindow::AccountComboBoxChangedInAddNoteForm_edit()
{
    ui->accountAdditComboBox_edit->clear();


    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    wlt::Account selected_acc = wallet->getAccountByName( ui->accountMainComboBox_edit->currentText().toStdString() );
    for(auto acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
    {
        if(acc_iter->getName() != selected_acc.getName() && acc_iter->getCurrencyType() == selected_acc.getCurrencyType())
            ui->accountAdditComboBox_edit->addItem(QString::fromStdString(acc_iter->getName()));
    }
}

void MainWindow::DeleteAccount()
{
    wallet->Remove_Account(ui->accountNameComboBox_edit->currentText().toStdString());
    ui->mainWindow->setCurrentIndex(0);
}

void MainWindow::editAccountFormUpdate()
{
    QObject::disconnect(ui->accountNameComboBox_edit, &QComboBox::currentTextChanged, this,  &MainWindow::AccountNameChangedInEditor);

    ui->accountNameComboBox_edit->clear();
    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    for(auto acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
        ui->accountNameComboBox_edit->addItem(QString::fromStdString(acc_iter->getName()));

    if(ui->accountNameComboBox_edit->currentIndex() == 0)
        this->AccountNameChangedInEditor();

    QObject::connect(ui->accountNameComboBox_edit, &QComboBox::currentTextChanged, this,  &MainWindow::AccountNameChangedInEditor);
}

void MainWindow::addNoteFormUpdate()
{
    //QObject::connect(ui->OperationComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::OperationTypeChangedInForm);
    QObject::disconnect(ui->accountMainComboBox, &QComboBox::currentTextChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm);

    ui->accountMainComboBox->clear();
    ui->accountAdditComboBox->clear();
    ui->ValueLineEditNote->setText("");
    ui->detailsTextEditNote->setText("");

    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    for(auto acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
    {
        ui->accountMainComboBox->addItem(QString::fromStdString(acc_iter->getName()));
        //ui->accountAdditComboBox->addItem(QString::fromStdString(acc_iter->getName()));
    }

    QObject::connect(ui->accountMainComboBox, &QComboBox::currentTextChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm);
}

void MainWindow::editNoteFormUpdate(unsigned long id)
{
    QObject::disconnect(ui->accountMainComboBox_edit, &QComboBox::currentIndexChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm_edit);
    ui->accountMainComboBox_edit->clear();
    ui->accountAdditComboBox_edit->clear();
    ui->ValueLineEditNote_edit->setText("");
    ui->detailsTextEditNote_edit->setText("");

    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    for(auto acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
    {
        ui->accountMainComboBox_edit->addItem(QString::fromStdString(acc_iter->getName()));
        //ui->accountAdditComboBox_edit->addItem(QString::fromStdString(acc_iter->getName()));
    }

    QObject::connect(ui->accountMainComboBox_edit, &QComboBox::currentIndexChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm_edit);

    std::list<wlt::Note>::iterator selected_iterator = wallet->getNoteByID(id);
    ui->OperationComboBox_edit->setCurrentIndex((int)selected_iterator->getOperation() );
    ui->categoryComboBox_edit->setCurrentIndex((int) selected_iterator->getCategory() );
    ui->ValueLineEditNote_edit->setText( QString::number(selected_iterator->getValue() ,'f', 2) );
    ui->detailsTextEditNote_edit->setText(QString::fromStdString( selected_iterator->getDetails() ));
    ui->accountMainComboBox_edit->setCurrentText(  QString::fromStdString( selected_iterator->getAccountName()  )       );
    ui->accountAdditComboBox_edit->setCurrentText(   QString::fromStdString( selected_iterator->getAccountNameAddit() )  );

}

void MainWindow::SaveAccountEdit()
{
    if( ui->CountLineEdit_edit->text().length() == 0 || ui->CountLineEdit_edit->text().length() >= 13)
        return;

    if(  this->wallet->inRatesTable( ui->currencListCombobox_edit->currentText().toUpper().toStdString() ) == -1 )
        return;

    double Value = ui->CountLineEdit_edit->text().replace(",",".").toDouble();
    wallet->Edit_Account(ui->accountNameComboBox_edit->currentText().toStdString(), Value, ui->currencListCombobox_edit->currentText().toStdString());
    ui->mainWindow->setCurrentIndex(0);
}

void MainWindow::CreateNote()
{
    if(ui->ValueLineEditNote->text().length() == 0)
        return;

    if(ui->OperationComboBox->currentIndex() == 2 && ui->accountAdditComboBox->currentText() == "")
        return;

    if(ui->ValueLineEditNote->text().length() >= 13)
        return;

    double Value = ui->ValueLineEditNote->text().replace(",",".").toDouble();
    QString details_str = ui->detailsTextEditNote->toPlainText();
    details_str.truncate(500);
    int operation_type = ui->OperationComboBox->currentIndex();

    wlt::Account main_acc = wallet->getAccountByName(  ui->accountMainComboBox->currentText().toStdString() );

    if(ui->accountAdditComboBox->currentText().toStdString() != "")
    {
        wlt::Account addit_acc = wallet->getAccountByName(  ui->accountAdditComboBox->currentText().toStdString() );
        if( addit_acc.getCount() >= 100000000000 && Value > 0 && operation_type == 2)
            return;
    }
    if(Value <= 0 && operation_type == 2)
        return;

    if( main_acc.getCount() >= 100000000000 && Value > 0 && operation_type == 1)
        return;
    else if ( main_acc.getCount() <= -100000000000 && Value > 0 && operation_type == 0)
        return;

    if(operation_type == 2)
    {
        //  Transfer operation
        wallet->Create_Note( static_cast<wlt::Operation>( ui->OperationComboBox->currentIndex() ), static_cast<wlt::Category>( ui->categoryComboBox->currentIndex() ),
                            ui->accountMainComboBox->currentText().toStdString(), ui->accountAdditComboBox->currentText().toStdString() ,Value, details_str.toStdString());
        ui->mainWindow->setCurrentIndex(0);
        return;
    }

    if(operation_type == 0)
        operation_type = 1;
    else if(operation_type == 1)
        operation_type = 0;

    //  Usual operation
    wallet->Create_Note( static_cast<wlt::Operation>( operation_type ), static_cast<wlt::Category>( ui->categoryComboBox->currentIndex() ),
                        ui->accountMainComboBox->currentText().toStdString(), Value, details_str.toStdString());

    ui->mainWindow->setCurrentIndex(this->last_index);
}

void MainWindow::EditNote()
{
    if(ui->ValueLineEditNote_edit->text().length() == 0)
        return;

    if(ui->OperationComboBox_edit->currentIndex() == 2 && ui->accountAdditComboBox_edit->currentText() == "")
        return;

    if(ui->ValueLineEditNote_edit->text().length() >= 13)
        return;

    double Value = ui->ValueLineEditNote_edit->text().replace(",",".").toDouble();
    QString details_str = ui->detailsTextEditNote_edit->toPlainText();
    details_str.truncate(500);
    int operation_type = ui->OperationComboBox_edit->currentIndex();

    wlt::Account main_acc = wallet->getAccountByName(  ui->accountMainComboBox_edit->currentText().toStdString() );

    if(ui->accountAdditComboBox_edit->currentText().toStdString() != "")
    {
        wlt::Account addit_acc = wallet->getAccountByName(  ui->accountAdditComboBox_edit->currentText().toStdString() );
        if( addit_acc.getCount() >= 100000000000 && Value > 0 && operation_type == 2)
            return;
    }
    if(Value <= 0 && operation_type == 2)
        return;

    if( main_acc.getCount() >= 100000000000 && Value > 0 && operation_type == 1)
        return;
    else if ( main_acc.getCount() <= -100000000000 && Value > 0 && operation_type == 0)
        return;

    if(operation_type == 2)
    {
        wallet->Edit_Note(this->ID, static_cast<wlt::Operation>( ui->OperationComboBox_edit->currentIndex() ), static_cast<wlt::Category>( ui->categoryComboBox_edit->currentIndex() ),
                          ui->accountMainComboBox_edit->currentText().toStdString(), ui->accountAdditComboBox_edit->currentText().toStdString() ,Value, details_str.toStdString());
        ui->mainWindow->setCurrentIndex(0);
        return;
    }

    if(operation_type == 0)
        operation_type = 1;
    else if(operation_type == 1)
        operation_type = 0;

    wallet->Edit_Note(this->ID, static_cast<wlt::Operation>( operation_type ), static_cast<wlt::Category>( ui->categoryComboBox_edit->currentIndex() ),
                      ui->accountMainComboBox_edit->currentText().toStdString(), Value, details_str.toStdString() );

    ui->mainWindow->setCurrentIndex(this->last_index);
}

void MainWindow::DeleteNote()
{
    wallet->Remove_Note(this->ID);
    ui->mainWindow->setCurrentIndex(this->last_index);
}

void MainWindow::CurrencyChanged()
{
    this->wallet->setCurrencyType(ui->MainCurrencyComboBox->currentText().toStdString());
}

// -------------------------------------


void MainWindow::formsInit()
{
    //       Account Form
    ui->CountLineEdit->setValidator( new QDoubleValidator(-10000000000, 10000000000, 2) );
    std::list<std::string> RatesStrings = wallet->getRatesNames();
    for(auto it = RatesStrings.begin(); it != RatesStrings.end(); it++)
    {
        ui->currencListCombobox->addItem( QString::fromStdString(*it) );
    }
    QObject::connect(ui->addAccCancelButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(0);} );
    QObject::connect(ui->addAccCreateButton, &QPushButton::clicked, this , &MainWindow::CreateAccount);

    //      Edit Account Form
    ui->CountLineEdit_edit->setValidator( new QDoubleValidator(-10000000000, 10000000000, 2) );
    for(auto it = RatesStrings.begin(); it != RatesStrings.end(); it++)
    {
        ui->currencListCombobox_edit->addItem(QString::fromStdString(*it));
    }
    std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    std::list<wlt::Account>::iterator acc_iter;
    for(acc_iter = lst_acc.begin(); acc_iter != lst_acc.end(); acc_iter++)
    {
        ui->accountNameComboBox_edit->addItem(QString::fromStdString(acc_iter->getName()));
    }

    if(this->wallet->AccountsCount() > 0)
    {
        int choosen = 0;
        acc_iter = lst_acc.begin();
        std::advance(acc_iter, choosen);


        ui->accountNameComboBox_edit->setCurrentIndex( choosen);
        ui->currencListCombobox_edit->setCurrentText( QString::fromStdString( acc_iter->getCurrencyType() ) );
        ui->CountLineEdit_edit->setText( QString::number( acc_iter->getCount(), 'f', 2 ) );
    }

    QObject::connect(ui->accountNameComboBox_edit, &QComboBox::currentTextChanged, this,  &MainWindow::AccountNameChangedInEditor);
    QObject::connect(ui->cancelButton_edit, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(0);});
    QObject::connect(ui->deleteButton_edit, &QPushButton::clicked, this, &MainWindow::DeleteAccount);
    QObject::connect(ui->saveButton_edit, &QPushButton::clicked, this, &MainWindow::SaveAccountEdit);

    //      Add note form

    ui->OperationComboBox->addItem( QString("Расход") );
    ui->OperationComboBox->addItem( QString("Доход") );
    ui->OperationComboBox->addItem( QString("Перевод") );

    //Category { FOOD, PRODUCT, HOUSE, TRANSPORT, CAR, ENTERTAINMENT, NETWORK, FINANCE_EXPENSIES, TRANSFER_OPERATION, EARNINGS};
    ui->categoryComboBox->addItem("Еда и рестораны");
    ui->categoryComboBox->addItem("Товары");
    ui->categoryComboBox->addItem("Здоровье");
    ui->categoryComboBox->addItem("Публичный транспорт");
    ui->categoryComboBox->addItem("Транспортное средство");
    ui->categoryComboBox->addItem("Развлечения");
    ui->categoryComboBox->addItem("Связь и интернет");
    ui->categoryComboBox->addItem("Финансовые расходы");
    ui->categoryComboBox->addItem("Финансовые переводы");
    ui->categoryComboBox->addItem("Доход");

    //      EDIT combobox items !!!

    //std::list<wlt::Account> lst_acc =  wallet->getAccountList();
    for(auto it = lst_acc.begin(); it != lst_acc.end(); it++)
    {
        ui->accountMainComboBox->addItem(QString::fromStdString(it->getName() ) );
        //ui->accountAdditComboBox->addItem(QString::fromStdString(it->getName() ) );
    }
    // if(ui->accountAdditComboBox->count() > 1)
    //     ui->accountAdditComboBox->setCurrentIndex(1);
    ui->accountMainComboBox->setCurrentText("");

    ui->AccountLabelAddit->setVisible(false);
    ui->accountAdditComboBox->setVisible(false);

    ui->ValueLineEditNote->setValidator( new QDoubleValidator(-10000000000, 10000000000, 2) );

    QObject::connect(ui->cancelButtonNote, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(this->last_index);});
    QObject::connect(ui->saveButtonNote, &QPushButton::clicked, this, &MainWindow::CreateNote );
    QObject::connect(ui->OperationComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::OperationTypeChangedInForm);
    QObject::connect(ui->accountMainComboBox, &QComboBox::currentTextChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm);

    //  Edit Note Form
    ui->OperationComboBox_edit->addItem( QString("Расход") );
    ui->OperationComboBox_edit->addItem( QString("Доход") );
    ui->OperationComboBox_edit->addItem( QString("Перевод") );

    ui->categoryComboBox_edit->addItem("Еда и рестораны");
    ui->categoryComboBox_edit->addItem("Товары");
    ui->categoryComboBox_edit->addItem("Здоровье");
    ui->categoryComboBox_edit->addItem("Публичный транспорт");
    ui->categoryComboBox_edit->addItem("Транспортное средство");
    ui->categoryComboBox_edit->addItem("Развлечения");
    ui->categoryComboBox_edit->addItem("Связь и интернет");
    ui->categoryComboBox_edit->addItem("Финансовые расходы");
    ui->categoryComboBox_edit->addItem("Финансовые переводы");
    ui->categoryComboBox_edit->addItem("Доход");

    for(auto it = lst_acc.begin(); it != lst_acc.end(); it++)
    {
        ui->accountMainComboBox_edit->addItem(QString::fromStdString(it->getName() ) );
        //ui->accountAdditComboBox_edit->addItem(QString::fromStdString(it->getName() ) );
    }

    ui->AccountLabelAddit_edit->setVisible(false);
    ui->accountAdditComboBox_edit->setVisible(false);

    ui->ValueLineEditNote_edit->setValidator( new QDoubleValidator(-10000000000, 10000000000, 2) );

    QObject::connect(ui->OperationComboBox_edit, &QComboBox::currentIndexChanged, this, &MainWindow::OperationTypeChangedInForm_edit);
    QObject::connect(ui->accountMainComboBox_edit, &QComboBox::currentIndexChanged, this, &MainWindow::AccountComboBoxChangedInAddNoteForm_edit);
    QObject::connect(ui->cancelButtonNote_edit, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(this->last_index);} );
    QObject::connect(ui->saveButtonNote_edit, &QPushButton::clicked, this, &MainWindow::EditNote );
    QObject::connect( ui->deleteButtonNote_edit, &QPushButton::clicked, this, &MainWindow::DeleteNote );


}

void MainWindow::settingsFormInit()
{
    //  Currency combobox
    std::list<std::string> RatesStrings = wallet->getRatesNames();
    for(auto it = RatesStrings.begin(); it != RatesStrings.end(); it++)
    {
        ui->MainCurrencyComboBox->addItem( QString::fromStdString(*it) );
    }
    ui->MainCurrencyComboBox->setCurrentText(QString::fromStdString(this->wallet->getCurrencyType()));
    QObject::connect(ui->MainCurrencyComboBox, &QComboBox::currentTextChanged, this, &MainWindow::CurrencyChanged);

    //  Main Language
    ui->languageComboBox->addItem(QString("EN"));
    ui->languageComboBox->addItem(QString("RU"));
    ui->languageComboBox->setCurrentText(QString::fromStdString(this->wallet->getLocalLanguage()));
}


void MainWindow::sideBarConnects()
{
    QObject::connect(ui->mainButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(0);} );
    QObject::connect(ui->mainButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Главная");} );

    QObject::connect(ui->notesButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(5);});
    QObject::connect(ui->notesButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Операции");});

    QObject::connect(ui->statisticButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(6);});
    QObject::connect(ui->statisticButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Статистика");});

    QObject::connect(ui->ratesButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(7);});
    QObject::connect(ui->ratesButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Курсы валют");});

    QObject::connect(ui->settingsButton, &QPushButton::clicked, ui->mainWindow, [=](){ui->mainWindow->setCurrentIndex(8);});
    QObject::connect(ui->settingsButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Настройки");});

    QObject::connect(ui->supportButton, &QPushButton::clicked, ui->mainWindow,[=](){ui->mainWindow->setCurrentIndex(9);});
    QObject::connect(ui->supportButton, &QPushButton::clicked, ui->namebar_label, [=](){ui->namebar_label->setText("Поддержать");});

    ui->mainButton->setIcon(this->getWhiteIcon(":/nav-bar/house.svg"));
    ui->mainButton->setIconSize(QSize(23,22));
    ui->mainButton->setCursor(Qt::PointingHandCursor);

    ui->notesButton->setIcon(this->getWhiteIcon(":/nav-bar/note.svg"));
    ui->notesButton->setIconSize(QSize(23,22));
    ui->notesButton->setCursor(Qt::PointingHandCursor);

    ui->statisticButton->setIcon(this->getWhiteIcon(":/nav-bar/chart.svg"));
    ui->statisticButton->setIconSize(QSize(23,22));
    ui->statisticButton->setCursor(Qt::PointingHandCursor);

    ui->ratesButton->setIcon(this->getWhiteIcon(":/nav-bar/glass-dollar.svg"));
    ui->ratesButton->setIconSize(QSize(23,22));
    ui->ratesButton->setCursor(Qt::PointingHandCursor);

    ui->settingsButton->setIcon(this->getWhiteIcon(":/nav-bar/gear.svg"));
    ui->settingsButton->setIconSize(QSize(23,22));
    ui->settingsButton->setCursor(Qt::PointingHandCursor);

    // ui->supportButton->setIcon(this->getWhiteIcon(":/nav-bar/house.svg"));
    // ui->supportButton->setIconSize(QSize(23,22));

}

void MainWindow::fillAccountData()
{
    //      Clear data
    this->removeLayoutWidgets(ui->AccList->layout());

    //      Total account label
    double totalValue = wallet->getTotalCount();
    ui->TotalCountLabel->setText( QString::number(totalValue, 'f', 2) + QString::fromStdString(" " + wallet->getCurrencyType()  ) );

    if(QString::number(totalValue, 'f', 2).length() >= 13)
        ui->TotalCountLabel->setStyleSheet("font: 600 31pt \"Segoe UI\";color:#ECEBDF;");
    else if(QString::number(totalValue, 'f', 2).length() >= 10)
        ui->TotalCountLabel->setStyleSheet("font: 600 34pt \"Segoe UI\";color:#ECEBDF;");
    else
        ui->TotalCountLabel->setStyleSheet("font: 600 42pt \"Segoe UI\";color:#ECEBDF;");

    //  Create cards
    std::list<wlt::Account> acc_lst = this->wallet->getAccountList();
    for(auto it = acc_lst.begin(); it != acc_lst.end(); it++ )
    {
        this->addAccountCard(it->getName(), it->getCount(), it->getCurrencyType());
    }

    //  Create add_acc_button
    if(this->wallet->AccountsCount() < 10)
    {
        QPushButton *addAccountButton = new QPushButton("Добавить новый счёт");
        addAccountButton->setMaximumSize(500, 80);
        addAccountButton->setMinimumSize(350, 80);
        addAccountButton->setStyleSheet("QPushButton{background-color:#1F2021; font: 600 14pt \"Segoe UI\";color:#ECEBDF;border: 5px solid #112D9F; } QPushButton:hover{background-color:#112D9F; font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");
        addAccountButton->setCursor(Qt::PointingHandCursor);
        QObject::connect(addAccountButton, &QPushButton::clicked, this, [=](){ ui->mainWindow->setCurrentIndex(2);  });
        ui->AccList->layout()->addWidget( addAccountButton );
    }


    QSpacerItem *vertical_spacer = new QSpacerItem(20, 40, QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    ui->AccList->layout()->addItem(vertical_spacer);

}

void MainWindow::addAccountCard(std::string AccName, double Count, std::string Currency)
{
    //      Create account card example
    //QWidget *accountCard = new QWidget();
    AccountCardWidget *accountCard = new AccountCardWidget();
    accountCard->setAccountName(AccName);
    QHBoxLayout *layout = new QHBoxLayout(accountCard);


    accountCard->setMinimumSize(350, 80);
    accountCard->setMaximumSize(500, 80);
    //accountCard->setStyleSheet("QWidget#AccountCardWidget{background-color:rgb(86, 86, 86);}");

    //      Create leftside icon
    QWidget *iconBG = new QWidget();
    iconBG->setMaximumSize(60, 60);
    iconBG->setStyleSheet("QWidget{border-radius: 15px; background-color: #530FC8;}");

    QLabel *icon = new QLabel();
    //QPixmap iconImage(":/icons/wallet-solid.svg");
    QPixmap iconImage = this->getWhiteIcon(":/icons/wallet-solid.svg");


    icon->setPixmap(iconImage);
    icon->setScaledContents(true);
    icon->setGeometry(10, 10, 40, 40);
    icon->setParent(iconBG);

    //  Create data info labels
    QVBoxLayout *dataLabelsLayout = new QVBoxLayout();
    QLabel *CountLabel = new QLabel();
    QLabel *AccLabel = new QLabel();
    QString Count_str = QString::number(Count, 'f', 2);

    CountLabel->setText(Count_str + QString::fromStdString(" " + Currency));
    AccLabel->setText(QString::fromStdString(AccName));



    if(Count_str.length() >= 10)
        CountLabel->setStyleSheet("QLabel{font: 600 12pt \"Segoe UI\";color:#ECEBDF;}");
    else
        CountLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");
    AccLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");

    dataLabelsLayout->addWidget(AccLabel);
    dataLabelsLayout->addWidget(CountLabel);

    //  Create right-side buttons
    QPushButton *addButton = new QPushButton();
    QPushButton *editButton = new QPushButton();

    addButton->setMaximumSize(50, 50);
    editButton->setMaximumSize(50, 50);

    //addButton->setText(QString("+"));
    //addButton->setIcon(QIcon(this->getWhiteIcon("")));
    //editButton->setText(QString("E"));

    addButton->setIcon(QIcon(this->getWhiteIcon(":/utility-icons/plus.svg") ) );
    addButton->setIconSize(QSize(35,35));
    editButton->setIcon(QIcon(this->getWhiteIcon(":/utility-icons/edit.svg") ) );
    editButton->setIconSize(QSize(30,30));

    addButton->setStyleSheet("QPushButton{border: 3px solid #112D9F;background-color:#1F2021;border-radius:15px;} QPushButton:hover{background-color:#112D9F;border-radius:15px;}");
    editButton->setStyleSheet("QPushButton{border: 3px solid #112D9F;background-color:#1F2021;border-radius:15px;} QPushButton:hover{background-color:#112D9F;border-radius:15px;}");

    QGraphicsOpacityEffect *effect_add = new QGraphicsOpacityEffect();
    QGraphicsOpacityEffect *effect_edit = new QGraphicsOpacityEffect();
    addButton->setGraphicsEffect(effect_add);
    editButton->setGraphicsEffect(effect_edit);
    effect_add->setOpacity(0);
    effect_edit->setOpacity(0);

    //  add note button logic
    QObject::connect(addButton, &QPushButton::clicked, addButton, [=](){
        QString accountNamefromCard = QString::fromStdString( accountCard->getAccountName() );
        this->addNoteFormUpdate();
        ui->accountMainComboBox->setCurrentText(accountNamefromCard);
        ui->mainWindow->setCurrentIndex(1);
    });

    //  edit account button logic
    QObject::connect(editButton, &QPushButton::clicked, editButton, [=](){
        QString accountNamefromCard = QString::fromStdString( accountCard->getAccountName() );
        this->editAccountFormUpdate();
        ui->accountNameComboBox_edit->setCurrentText(accountNamefromCard);
        ui->mainWindow->setCurrentIndex(4);
    });

    //  adding some animation effects
    QObject::connect(accountCard, &AccountCardWidget::MouseInSignal, this, [=](){
        effect_add->setOpacity(1);
        effect_edit->setOpacity(1);
    });

    QObject::connect(accountCard, &AccountCardWidget::MouseOutSignal, this, [=](){
        effect_add->setOpacity(0);
        effect_edit->setOpacity(0);
    });

    addButton->setCursor(Qt::PointingHandCursor);
    editButton->setCursor(Qt::PointingHandCursor);

    //      Adding all elements to main layout
    layout->addWidget(iconBG);
    layout->addLayout(dataLabelsLayout);
    layout->addWidget(addButton);
    layout->addWidget(editButton);


    //      Creating card to ui interface
    ui->AccList->layout()->addWidget( accountCard );


}

void MainWindow::fillNoteData()
{
    this->removeLayoutWidgets(ui->NoteList->layout());

    std::list<wlt::Note> note_lst = this->wallet->getNoteListLast(7);

    for(auto it = note_lst.begin(); it != note_lst.end(); it++)
    {
        ui->NoteList->layout()->addWidget( this->addNoteCard(it->getOperation(), it->getCategoryString(), it->getAccountName(), it->getAccountNameAddit(), it->getValue(),
                                                            it->getDetails(), it->getDay(), it->getMonth(), it->getYear(), it->getId()) );
    }

    QSpacerItem *vertical_spacer = new QSpacerItem(30, 40, QSizePolicy::Maximum, QSizePolicy::Expanding);
    ui->NoteList->layout()->addItem(vertical_spacer);

}

QWidget *MainWindow::addNoteCard(wlt::Operation operation, std::string category, std::string accName, std::string accNameAddit,
                              double value, std::string details, unsigned int day, unsigned int month, unsigned int year, unsigned long ID)
{
    //  Create Note Card example
    // QWidget *noteCard = new QWidget();
    NoteCardWidget *noteCard = new NoteCardWidget();
    noteCard->setID(ID);

    QHBoxLayout *layout = new QHBoxLayout(noteCard);

    noteCard->setMinimumSize(450, 80);
    noteCard->setMaximumSize(500, 80);
    noteCard->setCursor(Qt::PointingHandCursor);
    //noteCard->setStyleSheet("NoteCardWidget{ background-color:rgb(86, 86, 86); }");


    //  Creating left-side icon
    QWidget *iconBG = new QWidget();
    iconBG->setMaximumSize(60, 60);
    //iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");

    QLabel *icon = new QLabel();
    QPixmap iconImage;
    //QPixmap iconImage(":/icons/money-bill-solid.svg");
    //QPixmap iconImage = this->getWhiteIcon(":/icons/money-bill-solid.svg");


    //      Dynamic icon changing
        if(category == "Food")
        {
            iconImage = this->getWhiteIcon(":/icons/food.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #4A5A73;}");
        }
        else if(category == "Products")
        {
            iconImage = this->getWhiteIcon(":/icons/shopping-bag.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #009DFF;}");
        }
        else if(category == "Healthcare")
        {
            iconImage = this->getWhiteIcon(":/icons/med.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0FAE56;}");
        }
        else if(category == "Public Transport")
        {
            iconImage = this->getWhiteIcon(":/icons/bus.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #9B9489;}");
        }
        else if(category == "Vehicle")
        {
            iconImage = this->getWhiteIcon(":/icons/car.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #95005E;}");
        }
        else if(category == "Entertainment")
        {
            iconImage = this->getWhiteIcon(":/icons/smile.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #CC8B00;}");
        }
        else if(category == "Network")
        {
            iconImage = this->getWhiteIcon(":/icons/wifi.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");
        }
        else if(category ==  "Finance Expensies")
        {
            iconImage = this->getWhiteIcon(":/icons/money.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #AFBB02;}");
        }
        else if(category ==   "Transfer operation")
        {
            iconImage = this->getWhiteIcon(":/icons/money-trans.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #C95045;}");
        }
        else if(category ==  "Earnings")
        {
            iconImage = this->getWhiteIcon(":/icons/coins.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #C1B318;}");
        }
        else
        {
            iconImage = this->getWhiteIcon(":/icons/money-bill-solid.svg");
            iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");
        }



    icon->setPixmap(iconImage);
    icon->setScaledContents(true);
    icon->setGeometry(15, 15, 31, 31);
    icon->setParent(iconBG);

    //  Creating center info labels
    QVBoxLayout *centerDataLayout = new QVBoxLayout();
    QLabel *categoryLabel = new QLabel();
    QLabel *accountNameLabel = new QLabel();

    QString acc_name_correct = QString::fromStdString( accName );
    if(acc_name_correct.size() >= 18)
    {
        acc_name_correct.truncate(18);
        acc_name_correct.insert(19, '.');
        acc_name_correct.insert(20, '.');
        acc_name_correct.insert(21, '.');
    }

    categoryLabel->setText( QString::fromStdString(category) );
    accountNameLabel->setText( acc_name_correct );

    categoryLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");
    accountNameLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");

    centerDataLayout->addWidget(categoryLabel);
    centerDataLayout->addWidget(accountNameLabel);

    //  Creating right-side Count info labels
    QVBoxLayout *rightSideLayout = new QVBoxLayout();
    QLabel *valueLabel = new QLabel();
    QLabel *dateLabel = new QLabel();

    QString CurrencyType = QString::fromStdString( " " + wallet->getCurrencyFromAccName(accName) );
    QString MonthStr = this->fromIntToMonth(month);
    QString Year = QString::number(year);

    valueLabel->setText( QString::number(value, 'f', 1) + CurrencyType);
    dateLabel->setText( QString::number(day) + " " + MonthStr + " " + Year);

    valueLabel->setAlignment(Qt::AlignRight);
    dateLabel->setAlignment(Qt::AlignRight);

    switch(operation)
    {
        case wlt::Operation::EXPENSE:   valueLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#CE4D4D;}");
            break;
        case wlt::Operation::INCOME:    valueLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#54CE4D;}");
            break;
        case wlt::Operation::TRANSFER:  valueLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#CEBB4D;}");
            break;
        default:                        valueLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");
            break;
    }

    dateLabel->setStyleSheet("QLabel{font: 600 14pt \"Segoe UI\";color:#ECEBDF;}");

    rightSideLayout->addWidget(valueLabel);
    rightSideLayout->addWidget(dateLabel);

    //  Adding all elements to main layout
    layout->addWidget(iconBG);
    layout->addLayout(centerDataLayout);
    layout->addLayout(rightSideLayout);

    QObject::connect(noteCard, &NoteCardWidget::clickedSignal, this, [=](){
        this->editNoteFormUpdate(noteCard->getID());
        this->ID = noteCard->getID();
        ui->mainWindow->setCurrentIndex(3);
    });

    return noteCard;
    //  Creating card to ui interface
    //ui->NoteList->layout()->addWidget(noteCard);
}

void MainWindow::fillRatesData()
{
    this->removeLayoutWidgets(ui->RatesList->layout());

    std::list<std::string> rate_names = wallet->getRatesNames();
    std::list<double> rate_values = wallet->getRatesValues();

    std::list<std::string>::iterator names_iter = rate_names.begin();
    std::list<double>::iterator values_iter = rate_values.begin();

    std::string main_currency = wallet->getCurrencyType();

    for(; names_iter != rate_names.end(); names_iter++, values_iter++)
    {
        this->addRateCard(main_currency + "/" + *names_iter, *values_iter);
    }

    QSpacerItem *vertical_spacer = new QSpacerItem(30, 40, QSizePolicy::Maximum, QSizePolicy::Expanding);
    ui->RatesList->layout()->addItem(vertical_spacer);
}

void MainWindow::addRateCard(std::string Rate_cur_name, double rate_value)
{
    QWidget *RateWidget = new QWidget();
    QGridLayout *layout = new QGridLayout(RateWidget);

    RateWidget->setMinimumSize(QSize(90, 60));
    RateWidget->setMaximumSize(QSize(90, 60));
    RateWidget->setStyleSheet("QWidget{border:1.5px solid white;border-radius:10px;background-color:rgb(31, 32, 33);}");

    QLabel *rate_cur_label = new QLabel();
    rate_cur_label->setMinimumSize(QSize(50,20));
    rate_cur_label->setMaximumSize(QSize(80,40));
    rate_cur_label->setStyleSheet("QLabel{font: 600 12pt \"Segoe UI\";color:#ECEBDF;border:0px;}");
    rate_cur_label->setText(QString::fromStdString(Rate_cur_name));
    rate_cur_label->setAlignment(Qt::AlignHCenter);


    QLabel *rate_value_label = new QLabel();
    rate_value_label->setMinimumSize(QSize(50,20));
    rate_value_label->setMaximumSize(QSize(80,40));
    rate_value_label->setStyleSheet("QLabel{font: 600 11pt \"Segoe UI\";color:#BFBFBF;border:0px;}");
    rate_value_label->setText(QString::number(rate_value, 'f', 2));
    rate_value_label->setAlignment(Qt::AlignHCenter);

    layout->addWidget(rate_cur_label);
    layout->addWidget(rate_value_label);

    ui->RatesList->layout()->addWidget(RateWidget);

}

void MainWindow::updateStatPage()
{
    this->removeLayoutWidgets(ui->statScrollWidget->layout());

    //  Get import data
    std::vector<double> data_notes = this->wallet->getDataStat();
    std::vector<double> percent_data = this->getPercentFromVector(data_notes);

    //  Create pie chart

    QPieSlice *FoodSlice = new QPieSlice("Food", data_notes[0]);
    //FoodSlice->setLabelVisible();
    FoodSlice->setColor(QColor("#4A5A73"));
    FoodSlice->setLabelBrush(QBrush(QColor("#4A5A73")));
    FoodSlice->setBorderColor(QColor("#4A5A73"));

    QPieSlice *ProductSlice = new QPieSlice("Products", data_notes[1]);
    ProductSlice->setColor(QColor("#009DFF"));
    ProductSlice->setLabelBrush(QBrush(QColor("#009DFF")));
    ProductSlice->setBorderColor(QColor("#009DFF"));

    QPieSlice *HealthSlice = new QPieSlice("Healthcare", data_notes[2]);
    HealthSlice->setColor(QColor("#0FAE56"));
    HealthSlice->setLabelBrush(QBrush(QColor("#0FAE56")));
    HealthSlice->setBorderColor(QColor("#0FAE56"));

    QPieSlice *PublicSlice = new QPieSlice("Public Transport", data_notes[3]);
    PublicSlice->setColor(QColor("#9B9489"));
    PublicSlice->setLabelBrush(QBrush(QColor("#9B9489")));
    PublicSlice->setBorderColor(QColor("#9B9489"));

    QPieSlice *VehicleSlice = new QPieSlice("Vehicle", data_notes[4]);
    VehicleSlice->setColor(QColor("#95005E"));
    VehicleSlice->setLabelBrush(QBrush(QColor("#95005E")));
    VehicleSlice->setBorderColor(QColor("#95005E"));

    QPieSlice *EnterSlice = new QPieSlice("Entertainment", data_notes[5]);
    EnterSlice->setColor(QColor("#CC8B00"));
    EnterSlice->setLabelBrush(QBrush(QColor("#CC8B00")));
    EnterSlice->setBorderColor(QColor("#CC8B00"));

    QPieSlice *NetworkSlice = new QPieSlice("Network", data_notes[6]);
    NetworkSlice->setColor(QColor("#0B3CFF"));
    NetworkSlice->setLabelBrush(QBrush(QColor("#0B3CFF")));
    NetworkSlice->setBorderColor(QColor("#0B3CFF"));

    QPieSlice *FinExpSlice = new QPieSlice("Finance Expensies", data_notes[7]);
    FinExpSlice->setColor(QColor("#AFBB02"));
    FinExpSlice->setLabelBrush(QBrush(QColor("#AFBB02")));
    FinExpSlice->setBorderColor(QColor("#AFBB02"));

    QPieSlice *TransSlice = new QPieSlice("Transfer operation", data_notes[8]);
    TransSlice->setColor(QColor("#C95045"));
    TransSlice->setLabelBrush(QBrush(QColor("#C95045")));
    TransSlice->setBorderColor(QColor("#C95045"));

    QPieSeries *series = new QPieSeries();
    series->append(FoodSlice);
    series->append(ProductSlice);
    series->append(HealthSlice);
    series->append(PublicSlice);
    series->append(VehicleSlice);
    series->append(EnterSlice);
    series->append(NetworkSlice);
    series->append(FinExpSlice);
    series->append(TransSlice);
    series->setHoleSize(0.35);


    QChart *chart = new QChart();
    chart->addSeries(series);
    //chart->setTitle("Student marks");
    chart->legend()->hide();
    chart->setVisible(true);
    chart->setBackgroundBrush(QBrush(QColor(31, 32, 33) ) );


    QChartView *chartview = new QChartView(chart);
    chartview->setRenderHint(QPainter::Antialiasing);
    chartview->setMaximumSize(QSize(600,600));
    chartview->setMinimumSize(QSize(600,600));

    ui->statScrollWidget->layout()->addWidget(chartview);

    //  Create spacer
    QSpacerItem *vertical_spacer = new QSpacerItem(30, 150, QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->statScrollWidget->layout()->addItem(vertical_spacer);

    //  Create stat cards

    this->addInfoCard();
    this->addStatCard("Food", "", data_notes[0], percent_data[0]);
    this->addStatCard("Products", "", data_notes[1], percent_data[1]);
    this->addStatCard("Healthcare", "", data_notes[2], percent_data[2]);
    this->addStatCard("Public Transport", "", data_notes[3], percent_data[3]);
    this->addStatCard("Vehicle", "", data_notes[4], percent_data[4]);
    this->addStatCard("Entertainment", "", data_notes[5], percent_data[5]);
    this->addStatCard("Network", "", data_notes[6], percent_data[6]);
    this->addStatCard("Finance Expensies", "", data_notes[7], percent_data[7]);
    this->addStatCard("Transfer operation", "", data_notes[8], percent_data[8]);
}

void MainWindow::addStatCard(std::string category, std::string icon_color, double value, double percent )
{
    QWidget *statCard = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(statCard);

    statCard->setMinimumSize(450, 80);
    statCard->setMaximumSize(1000, 80);
    //statCard->setCursor(Qt::PointingHandCursor);
    //statCard->setStyleSheet("QWidget{ background-color:rgb(86, 86, 86); }");

    layout->setSpacing(30);


    //  Creating left-side icon
    QWidget *iconBG = new QWidget();
    iconBG->setMaximumSize(60, 60);
    iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");

    QLabel *icon = new QLabel();
    QPixmap iconImage;
    //QPixmap iconImage(":/icons/money-bill-solid.svg");
    //QPixmap iconImage = this->getWhiteIcon(":/icons/money-bill-solid.svg");


    //      Dynamic icon changing
    if(category == "Food")
    {
        iconImage = this->getWhiteIcon(":/icons/food.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #4A5A73;}");
    }
    else if(category == "Products")
    {
        iconImage = this->getWhiteIcon(":/icons/shopping-bag.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #009DFF;}");
    }
    else if(category == "Healthcare")
    {
        iconImage = this->getWhiteIcon(":/icons/med.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0FAE56;}");
    }
    else if(category == "Public Transport")
    {
        iconImage = this->getWhiteIcon(":/icons/bus.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #9B9489;}");
    }
    else if(category == "Vehicle")
    {
        iconImage = this->getWhiteIcon(":/icons/car.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #95005E;}");
    }
    else if(category == "Entertainment")
    {
        iconImage = this->getWhiteIcon(":/icons/smile.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #CC8B00;}");
    }
    else if(category == "Network")
    {
        iconImage = this->getWhiteIcon(":/icons/wifi.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");
    }
    else if(category ==  "Finance Expensies")
    {
        iconImage = this->getWhiteIcon(":/icons/money.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #AFBB02;}");
    }
    else if(category ==   "Transfer operation")
    {
        iconImage = this->getWhiteIcon(":/icons/money-trans.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #C95045;}");
    }
    else if(category ==  "Earnings")
    {
        iconImage = this->getWhiteIcon(":/icons/coins.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #C1B318;}");
    }
    else
    {
        iconImage = this->getWhiteIcon(":/icons/money-bill-solid.svg");
        iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");
    }



    icon->setPixmap(iconImage);
    icon->setScaledContents(true);
    icon->setGeometry(15, 15, 31, 31);
    icon->setParent(iconBG);

    //  Create labels
    QLabel *categLabel = new QLabel();
    categLabel->setStyleSheet("font: 600 12pt \"Segoe UI\";color:#ECEBDF;");
    categLabel->setText(QString::fromStdString(category) );
    categLabel->setMinimumSize(QSize(150, 50));

    QLabel *percentLabel = new QLabel();
    percentLabel->setStyleSheet("font: 600 13pt \"Segoe UI\";color:#ECEBDF;");
    percentLabel->setText(QString::number(percent, 'f', 1) + QString("%") );
    percentLabel->setMinimumSize(QSize(50,50));

    QLabel *valueLabel = new QLabel();
    valueLabel->setText(QString::number(value, 'f', 1) );
    valueLabel->setMinimumSize(QSize(100,50));

    if(valueLabel->text().length() >= 10)
        valueLabel->setStyleSheet("font: 600 10pt \"Segoe UI\";color:#ECEBDF;");
    else
        valueLabel->setStyleSheet("font: 600 13pt \"Segoe UI\";color:#ECEBDF;");

    layout->addWidget(iconBG);
    layout->addWidget(categLabel);
    layout->addWidget(percentLabel);
    layout->addWidget(valueLabel);

    ui->statScrollWidget->layout()->addWidget(statCard);

}

void MainWindow::addInfoCard()
{
    QWidget *statCard = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(statCard);

    statCard->setMinimumSize(450, 80);
    statCard->setMaximumSize(1000, 80);
    //statCard->setCursor(Qt::PointingHandCursor);
    //noteCard->setStyleSheet("NoteCardWidget{ background-color:rgb(86, 86, 86); }");

    layout->setSpacing(30);


    //  Creating left-side icon
    QWidget *iconBG = new QWidget();
    iconBG->setMaximumSize(60, 60);
    iconBG->setStyleSheet("QWidget{border-radius: 30px; background-color: #0B3CFF;}");

    QLabel *icon = new QLabel();
    QPixmap iconImage;
    //QPixmap iconImage(":/icons/money-bill-solid.svg");
    //QPixmap iconImage = this->getWhiteIcon(":/icons/money-bill-solid.svg");

    iconImage = this->getWhiteIcon(":/icons/info.svg");

    icon->setPixmap(iconImage);
    icon->setScaledContents(true);
    icon->setGeometry(23, 12, 15, 35);
    icon->setParent(iconBG);

    //  Create labels
    QLabel *categLabel = new QLabel();
    categLabel->setStyleSheet("font: 600 13pt \"Segoe UI\";color:#ECEBDF;");
    categLabel->setText( QString("Категория") );
     categLabel->setMinimumSize(QSize(150, 50));

    QLabel *percentLabel = new QLabel();
    percentLabel->setStyleSheet("font: 600 13pt \"Segoe UI\";color:#ECEBDF;");
    percentLabel->setText( QString("Процент")  );
    percentLabel->setMinimumSize(QSize(50, 50));

    QLabel *valueLabel = new QLabel();
    valueLabel->setStyleSheet("font: 600 13pt \"Segoe UI\";color:#ECEBDF;");
    valueLabel->setText( QString("Количество")  );
    valueLabel->setMinimumSize(QSize(100, 50));

    layout->addWidget(iconBG);
    layout->addWidget(categLabel);
    layout->addWidget(percentLabel);
    layout->addWidget(valueLabel);

    ui->statScrollWidget->layout()->addWidget(statCard);
}

std::vector<double> MainWindow::getPercentFromVector(std::vector<double> expenses_data)
{
    std::vector<double> percent_data = std::vector<double>(10, 0);
    double total = 0;

    for(int i = 0; i < expenses_data.size() - 1; i++)
    {
        total += expenses_data[i];
    }

    if(total == 0)
        return percent_data;


    for(int i = 0; i < expenses_data.size(); i++)
    {
        percent_data[i] = expenses_data[i] / total * 100;
    }

    return percent_data;
}

void MainWindow::fillFullNotesPage()
{
    this->removeLayoutWidgets(ui->fullNoteList->layout());

    //std::list<wlt::Note> note_lst = this->wallet->getNoteListLast(100);
    std::list<wlt::Note> note_lst = this->wallet->getNotesUptoDate(120);

    for(auto it = note_lst.begin(); it != note_lst.end(); it++)
    {
        ui->fullNoteList->layout()->addWidget( this->addNoteCard(it->getOperation(), it->getCategoryString(), it->getAccountName(), it->getAccountNameAddit(), it->getValue(),
                                                            it->getDetails(), it->getDay(), it->getMonth(), it->getYear(), it->getId()) );
    }

    QSpacerItem *vertical_spacer = new QSpacerItem(30, 40, QSizePolicy::Maximum, QSizePolicy::Expanding);
    ui->fullNoteList->layout()->addItem(vertical_spacer);
}

void MainWindow::updateSettingsForm()
{
    // !!!! Problem with efficiency !!!!!

    // ui->MainCurrencyComboBox->setCurrentText(QString::fromStdString(wallet->getCurrencyType() ) );
    // ui->languageComboBox->setCurrentText(QString::fromStdString(wallet->getLocalLanguage() ) );
}


//  Utility functions

void MainWindow::removeLayoutWidgets(QLayout* layout)
{
    QLayoutItem* child;
    while(layout->count()!=0)
    {
        child = layout->takeAt(0);
        if(child->layout() != 0)
        {
            this->removeLayoutWidgets(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}

QString MainWindow::fromIntToMonth(unsigned int monthNumber)
{
    QStringList months = {"января", "февраля", "марта", "апреля", "мая", "июня", "июля","августа","сентября","октября","ноября","декабря"};
    if (monthNumber >= 1 && monthNumber <= 12) {
        return months[monthNumber - 1];
    }
    return "";
}

QPixmap MainWindow::getWhiteIcon(std::string path)
{
    QImage image = QImage(QString::fromStdString(path) );
    QRgb oldColor = qRgb(0,0,0);
    QRgb newColor = qRgb(255,255,255);

    for(int y = 0; y < image.height(); y++)
    {
        for(int x = 0; x < image.width(); x++)
        {
            QRgb pixelColor = image.pixel(x,y);
            if(pixelColor == oldColor)
            {
                image.setPixel(x,y, newColor);
            }
        }
    }
    return QPixmap::fromImage(image);
}



