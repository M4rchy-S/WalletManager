#include "eWallet.h"


wlt::eWallet::eWallet()
{
	this->accounts = std::list<wlt::Account>();
	this->notes = std::list<wlt::Note>();

	//	Try to load db
	if (this->SQLRequest("select * from Settings", this->callback_settings) == -1)
	{
		//	Create db manually
        //std::cout << "Creating db" << std::endl;

        QDir dir;
        if( !dir.exists(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)) )
        {
            dir.mkdir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
        }
		 
		const char *sql = "CREATE TABLE \"Accounts\" ( \
			\"Name\"	TEXT NOT NULL, \
			\"CurrencyType\"	TEXT NOT NULL, \
			\"Count\"	REAL NOT NULL \
			)";
		this->SQLRequest(sql, this->callback);
		

		sql = "CREATE TABLE \"Notes\" ( \
			\"Operation\"	INTEGER NOT NULL, \
			\"CategoryType\"	INTEGER NOT NULL, \
			\"AccountName\"	TEXT NOT NULL, \
			\"AccountNameAddit\"	BLOB NOT NULL, \
			\"Value\"	REAL NOT NULL, \
			\"Details\"	TEXT NOT NULL, \
			\"Time\"	INTEGER NOT NULL, \
			\"ID\"	INTEGER NOT NULL, \
			PRIMARY KEY(\"ID\" AUTOINCREMENT) \
			)";
		this->SQLRequest(sql, this->callback);

		sql = "CREATE TABLE \"Rates\" ( \
			\"Currency\"	TEXT NOT NULL, \
			\"Value\"	REAL NOT NULL \
			)";
		this->SQLRequest(sql, this->callback);


		sql = "CREATE TABLE \"Settings\" (	\
			\"BaseCurrency\"	TEXT NOT NULL, \
			\"LocalLanguage\"	TEXT NOT NULL, \
			\"Month\"	INTEGER NOT NULL   )";

		this->SQLRequest(sql, this->callback);

		//		Set default settings
		this->Currency_Type = "EUR";
		this->LocalLanguage = "EN";
		this->LocalMonth = this->getMonth();

		this->SQLRequest("INSERT INTO \"Settings\" VALUES(\"" + this->Currency_Type + "\", \"" + this->LocalLanguage + "\"" + ", " + std::to_string(this->LocalMonth) + ")", this->callback);

		//		Get Rates from API
        this->fillRatesTables();
        //      Delete BTC from Rates
        this->deleteRate("BTC");
        this->deleteRate("eur");
	}
	else
	{
		//	Getting all data from db
        //std::cout << "Adding data to memory from db" << std::endl;

        this->resetProgramDataFromDB();
        this->deleteRate("BTC");
        this->deleteRate("eur");
		//		Get Rates from API
        this->fillRatesTables();

        //if (this->LocalMonth != this->getMonth() || this->Rates.size() == 0) !!!!!!!!!!!!!!!!!!!!!
  //       if (this->LocalMonth != this->getMonth() || this->Rates["EUR"] == 0.00)
        // {
        // 	this->LocalMonth = this->getMonth();
        // 	this->SQLRequest("UPDATE Settings SET Month = " + std::to_string(this->LocalMonth), this->callback);
  //           this->fillRatesTables();

        // }
	}
	
}



wlt::eWallet::~eWallet()
{
	this->accounts.clear();
	this->notes.clear();
    this->Rates.clear();
}


int wlt::eWallet::Create_Account( const std::string& Name, double Value, std::string new_currency, bool addToDB)
{
	//	Check for same Name
	//	Similar names are prohibited
	for (auto it = this->accounts.begin(); it != this->accounts.end(); it++)
	{
		if (it->getName() == Name)
			return -1;
	}

	if (new_currency == "")
		new_currency = this->Currency_Type;

	this->accounts.push_back( wlt::Account(Name, new_currency, Value) );

	if(addToDB == true)
		this->SQLRequest("INSERT INTO Accounts VALUES(\"" + Name + "\", \"" + new_currency + "\", " + std::to_string(Value) + ")", this->callback);

    return 1;
}

void wlt::eWallet::Show_Accounts_All()
{
    // std::cout << "\t--= Accounts =--" << std::endl;
    // int i = 0;
    // for (auto it = this->accounts.begin(); it != this->accounts.end(); it++, i++)
    // {
    // 	std::cout << i+1 << ": " << it->debugInfo() << std::endl;
    // }
    // std::cout << std::endl;
}

int wlt::eWallet::Show_Account(int index)
{
    // if (index <= 0 || index > this->accounts.size())
    // 	return -1;

    // std::list<wlt::Account>::iterator it = this->accounts.begin();
    // std::advance(it, index - 1);

    // std::cout << "\t--= Account =--" << std::endl;
    // std::cout << index << ": " << it->debugInfo() << std::endl;

    // std::cout << std::endl;

    return 1;
}

int wlt::eWallet::Remove_Account(int index)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	std::list<wlt::Account>::iterator it = this->accounts.begin();
	std::advance(it, index - 1);

	this->SQLRequest("DELETE from Accounts WHERE Name=\"" + it->getName() + "\"", callback);
    this->SQLRequest("DELETE from Notes WHERE AccountName=\"" + it->getName() + "\"", callback);
	this->accounts.erase(it);

    this->resetProgramDataFromDB();

    return 1;
}

int wlt::eWallet::Remove_Account(std::string accName)
{
    int index = this->_getAccount(accName);
    if(index == -1)
        return -1;

    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, index - 1);

    this->SQLRequest("DELETE from Accounts WHERE Name=\"" + acc_iter->getName() + "\"", callback);
    this->SQLRequest("DELETE from Notes WHERE AccountName=\"" + acc_iter->getName() + "\"", callback);
    this->accounts.erase(acc_iter);

    this->resetProgramDataFromDB();

    return 1;

}

int wlt::eWallet::Edit_Account(std::string accName, double Value, std::string new_currency, bool addToDB)
{
    int index = this->_getAccount(accName);
    if(index == -1)
        return -1;

    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, index - 1);

    acc_iter->setCount(Value);
    acc_iter->setCurrencyType(new_currency);

    if(addToDB == true)
        this->SQLRequest("UPDATE Accounts SET CurrencyType='" + acc_iter->getCurrencyType() + "', Count='" + std::to_string(acc_iter->getCount()) + "' WHERE Name='" + acc_iter->getName() + "'", this->callback);

    return 1;
}

wlt::Account wlt::eWallet::getAccountByName(std::string accName)
{
    int index = this->_getAccount(accName);
    if(index == -1)
        return nullptr;

    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, index - 1);

    wlt::Account ret_account(acc_iter->getName(), acc_iter->getCurrencyType(), acc_iter->getCount());

    return ret_account;
}

std::list<wlt::Note>::iterator wlt::eWallet::getNoteByID(unsigned long id)
{
    std::list<wlt::Note>::iterator note_iterator = this->notes.begin();
    for(; note_iterator != this->notes.end(); note_iterator++)
    {
        if(note_iterator->getId() == id)
        {
            return note_iterator;
        }
    }
    throw std::out_of_range("Iterator out of range");
}

int wlt::eWallet::Create_Note(Operation OperationType, Category CategoryType, int index, double value, std::string details)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	auto it = this->accounts.begin();
	std::advance(it, index - 1);


	//	SQL Request
    unsigned long id;
    std::list<wlt::Note>::iterator note_iterator;

    if(this->notes.size() != 0)
    {
        note_iterator = this->notes.begin();
        //std::advance(note_iterator, this->notes.size() - 1);
        id = note_iterator->getId() + 1;
    }
    else
        id = 1;
	
    this->notes.push_front( wlt::Note(OperationType, CategoryType, it->getName(), value, id, details));

	note_iterator = this->notes.begin();
	std::advance(note_iterator, this->notes.size() - 1);
	time_t epochTime = note_iterator->getEpochTime();


	std::string sql_req = "INSERT INTO Notes VALUES(" + std::to_string(static_cast<int>(OperationType)) + ", " + std::to_string(static_cast<int>(CategoryType)) + ", \""
		+ it->getName() + "\", \"\", " + std::to_string(value) + ", \"" + details + "\", " + std::to_string(epochTime) + ", " + std::to_string(id ) + ")";
	this->SQLRequest(sql_req, this->callback);
	
	switch (OperationType)
	{
		case wlt::INCOME:	
			it->changeCount(value);
			sql_req = "UPDATE Accounts SET Count=" + std::to_string(it->getCount()) + " WHERE Name='" + it->getName() + "'";
			this->SQLRequest(sql_req, this->callback);
			break;
		case wlt::EXPENSE:
			it->changeCount(value * -1);
			sql_req = "UPDATE Accounts SET Count=" + std::to_string(it->getCount()) + " WHERE Name='" + it->getName() + "'";
			this->SQLRequest(sql_req, this->callback);
			break;
		case wlt::TRANSFER:
			break;
		default:
			break;
	}
    return 1;
}

int wlt::eWallet::Create_Note(Operation OperationType,int index1, int index2, double value, std::string details)
{
	if (index1 <= 0 || index1 > this->accounts.size() || index2 < 0 || index2 > this->accounts.size() )
		return -1;

	if (OperationType != wlt::Operation::TRANSFER)
		return -1;

	auto it1 = this->accounts.begin();
	auto it2 = this->accounts.begin();
	std::advance(it1, index1 - 1);
	std::advance(it2, index2 - 1);

    unsigned long id;
    std::list<wlt::Note>::iterator note_iterator;

    if(this->notes.size() != 0)
    {
        note_iterator = this->notes.begin();
        //std::advance(note_iterator, this->notes.size() - 1);
        id = note_iterator->getId() + 1;
    }
    else
        id = 1;

    this->notes.push_front(wlt::Note(OperationType, it1->getName(), it2->getName(), value, id, details));

	note_iterator = this->notes.begin();
	std::advance(note_iterator, this->notes.size() - 1);
	time_t epochTime = note_iterator->getEpochTime();

	
	std::string sql_req = "INSERT INTO Notes VALUES(" + std::to_string(static_cast<int>(OperationType)) + ", " + std::to_string(static_cast<int>(wlt::Operation::TRANSFER)) + ", \""
		+ it1->getName() + "\", \"" + it2->getName()+ "\", " + std::to_string(value) + ", \"" + details + "\", " + std::to_string(epochTime) + ", " + std::to_string(id) + ")";
	this->SQLRequest(sql_req, this->callback);

	it1->changeCount(value * -1);
	sql_req = "UPDATE Accounts SET Count=" + std::to_string(it1->getCount()) + " WHERE Name='" + it1->getName() + "'";
	this->SQLRequest(sql_req, this->callback);

	it2->changeCount(value);
	sql_req = "UPDATE Accounts SET Count=" + std::to_string(it2->getCount()) + " WHERE Name='" + it2->getName() + "'";
	this->SQLRequest(sql_req, this->callback);
    return 1;
}

int wlt::eWallet::Add_Note_DB(Operation OperationType, Category CategoryType, int index, double value, unsigned long long time  ,std::string details, unsigned long id)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;


    auto it = this->accounts.begin();
    std::advance(it, index - 1);
    this->notes.push_back(wlt::Note(OperationType, CategoryType, it->getName(), value, id, details, time));
    // this->accounts.push_back( wlt::Account(Name, new_currency, Value) );
    return 1;
}

int wlt::eWallet::Add_Note_DB(Operation OperationType, int index1, int index2, double value,  unsigned long long time , std::string details, unsigned long id)
{
	if (index1 <= 0 || index1 > this->accounts.size() || index2 < 0 || index2 > this->accounts.size())
		return -1;

	if (OperationType != wlt::Operation::TRANSFER)
		return -1;

	auto it1 = this->accounts.begin();
	auto it2 = this->accounts.begin();
	std::advance(it1, index1 - 1);
	std::advance(it2, index2 - 1);

	this->notes.push_back(wlt::Note(OperationType, it1->getName(), it2->getName(), value, id, details, time));
    return 1;
}


void wlt::eWallet::Show_Notes_All()
{
    // std::cout << "\t--= Notes =--" << std::endl;
    // int i = 0;
    // for (auto it = notes.begin(); it != notes.end(); it++, i++)
    // {
    // 	std::cout << i + 1 << ": ID[" << it->getId() << "] " << it->debugInfo() << "\tDate: " << it->getTimeInfo() << std::endl;
    // }
    // std::cout << std::endl;
}

int wlt::eWallet::Show_Note(int index)
{
    // if (index <= 0 || index > this->notes.size())
    // 	return -1;

    // auto it = this->notes.begin();
    // std::advance(it, index - 1);

    // std::cout << "\t--= Note =--" << std::endl;
    // std::cout << index << ": " << it->debugInfo() << "\tDate: " << it->getTimeInfo() << std::endl;

    // std::cout << std::endl;
    return 1;
}

int wlt::eWallet::Remove_Note(int index)
{
	if(index <= 0 || index > this->notes.size())
		return -1;

	std::list<wlt::Note>::iterator note_it = this->notes.begin();
	std::list<wlt::Account>::iterator acc_it;
	std::list<wlt::Account>::iterator acc_add_it;
	int acc_index;
	int acc_add_index;

	std::advance(note_it, index - 1);

	//		Change Account Value

	switch(note_it->getOperation())
	{
		case wlt::INCOME:
			acc_index = this->_getAccount( note_it->getAccountName() );
			acc_it = this->accounts.begin();
			std::advance(acc_it, acc_index - 1);
			
			acc_it->changeCount(-1 * note_it->getValue());

			this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
			break;
		case wlt::EXPENSE:
			acc_index = this->_getAccount(note_it->getAccountName());
			acc_it = this->accounts.begin();
			std::advance(acc_it, acc_index - 1);
			
			acc_it->changeCount(note_it->getValue());

			this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
			break;
		case wlt::TRANSFER:
			acc_index = this->_getAccount(note_it->getAccountName());
			acc_add_index = this->_getAccount(note_it->getAccountNameAddit());
			acc_it = this->accounts.begin();
			acc_add_it = this->accounts.begin();
			std::advance(acc_it, acc_index - 1);
			std::advance(acc_add_it, acc_add_index - 1);

			acc_it->changeCount(note_it->getValue());
			acc_add_it->changeCount(-1 * note_it->getValue());

			this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
			this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_add_it->getCount()) + " WHERE Name='" + acc_add_it->getName() + "'", this->callback);

			break;

		default:
			break;
	}

	//		Delete Note from database and memory

	this->SQLRequest("DELETE from Notes WHERE ID=" + std::to_string(note_it->getId()), this->callback);

	this->notes.erase(note_it);

    return 1;
}

int wlt::eWallet::Remove_Note(unsigned long id)
{
    std::list<wlt::Note>::iterator note_it;
    bool found_note = false;

    for(note_it = this->notes.begin(); note_it != this->notes.end(); note_it++)
    {
        if(note_it->getId() == id)
        {
            found_note = true;
            break;
        }
    }
    if(found_note == false)
        return -1;

    std::list<wlt::Account>::iterator acc_it;
    std::list<wlt::Account>::iterator acc_add_it;
    int acc_index;
    int acc_add_index;

    //		Change Account Value

    switch(note_it->getOperation())
    {
    case wlt::INCOME:
        acc_index = this->_getAccount( note_it->getAccountName() );
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::EXPENSE:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::TRANSFER:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_add_index = this->_getAccount(note_it->getAccountNameAddit());
        acc_it = this->accounts.begin();
        acc_add_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);
        std::advance(acc_add_it, acc_add_index - 1);

        acc_it->changeCount(note_it->getValue());
        acc_add_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_add_it->getCount()) + " WHERE Name='" + acc_add_it->getName() + "'", this->callback);

        break;

    default:
        break;
    }

    //		Delete Note from database and memory

    this->SQLRequest("DELETE from Notes WHERE ID=" + std::to_string(note_it->getId()), this->callback);

    this->notes.erase(note_it);

    return 1;

}

int wlt::eWallet::Create_Note(Operation OperationType, Category CategoryType, std::string AccountName, double value, std::string details)
{
    int index_acc = this->_getAccount(AccountName);
    if(index_acc == -1)
        return -1;

    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, index_acc - 1);

    //	SQL Request
    //  get correct id
    unsigned long id;
    std::list<wlt::Note>::iterator note_iterator;

    if(this->notes.size() != 0)
    {
        note_iterator = this->notes.begin();
        //std::advance(note_iterator, this->notes.size() - 1);
        id = note_iterator->getId() + 1;
    }
    else
        id = 1;

    //  add data to program memory
    this->notes.push_front( wlt::Note(OperationType, CategoryType, acc_iter->getName(), value, id, details));

    //  get epochTime from memory
    note_iterator = this->notes.begin();
    std::advance(note_iterator, this->notes.size() - 1);
    time_t epochTime = note_iterator->getEpochTime();

    //  Create sql query
    std::string sql_req = "INSERT INTO Notes VALUES(" + std::to_string(static_cast<int>(OperationType)) + ", " + std::to_string(static_cast<int>(CategoryType)) + ", \""
                          + acc_iter->getName() + "\", \"\", " + std::to_string(value) + ", \"" + details + "\", " + std::to_string(epochTime) + ", " + std::to_string(id ) + ")";
    this->SQLRequest(sql_req, this->callback);

    //  Change account count
    switch (OperationType)
    {
        case wlt::INCOME:
            acc_iter->changeCount(value);
            sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
            this->SQLRequest(sql_req, this->callback);
            break;
        case wlt::EXPENSE:
            acc_iter->changeCount(value * -1);
            sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
            this->SQLRequest(sql_req, this->callback);
            break;
        case wlt::TRANSFER:
            break;
        default:
            break;
    }
    return 1;
}

int wlt::eWallet::Create_Note(Operation OperationType, Category CategoryType, std::string AccountName, std::string AccountNameAddit,double value, std::string details)
{
    //  Operation check
    if (OperationType != wlt::Operation::TRANSFER)
        return -1;
    //  Account #1
    int index = this->_getAccount(AccountName);
    if(index == -1)
        return -1;
    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, index - 1);

    //  Account #2
    index = this->_getAccount(AccountNameAddit);
    if(index == -1)
        return -1;
    std::list<wlt::Account>::iterator acc_add_iter = this->accounts.begin();
    std::advance(acc_add_iter, index - 1);

    //  Get correct id
    unsigned long id;
    std::list<wlt::Note>::iterator note_iterator;

    if(this->notes.size() != 0)
    {
        note_iterator = this->notes.begin();
        //std::advance(note_iterator, this->notes.size() - 1);
        id = note_iterator->getId() + 1;
    }
    else
        id = 1;

    //  Add data to memory
    this->notes.push_front(wlt::Note(OperationType, acc_iter->getName(), acc_add_iter->getName(), value, id, details));

    //  Get epochTime
    note_iterator = this->notes.begin();
    std::advance(note_iterator, this->notes.size() - 1);
    time_t epochTime = note_iterator->getEpochTime();

    //  SQL Query
    std::string sql_req = "INSERT INTO Notes VALUES(" + std::to_string(static_cast<int>(OperationType)) + ", " + std::to_string(static_cast<int>(wlt::Operation::TRANSFER)) + ", \""
                          + acc_iter->getName() + "\", \"" + acc_add_iter->getName()+ "\", " + std::to_string(value) + ", \"" + details + "\", " + std::to_string(epochTime) + ", " + std::to_string(id) + ")";
    this->SQLRequest(sql_req, this->callback);

    //  Change accounts count
    acc_iter->changeCount(value * -1);
    sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
    this->SQLRequest(sql_req, this->callback);

    acc_add_iter->changeCount(value);
    sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_add_iter->getCount()) + " WHERE Name='" + acc_add_iter->getName() + "'";
    this->SQLRequest(sql_req, this->callback);

    return 1;
}

int wlt::eWallet::Edit_Note(unsigned long id, Operation OperationType, Category CategoryType, std::string AccountName, double value, std::string details)
{
    //  incorrect input check
    int new_acc_index = this->_getAccount(AccountName);
    if(new_acc_index == -1)
        return -1;
    //  Find note with same id
    std::list<wlt::Note>::iterator note_it;
    bool found_note = false;

    for(note_it = this->notes.begin(); note_it != this->notes.end(); note_it++)
    {
        if(note_it->getId() == id)
        {
            found_note = true;
            break;
        }
    }
    if(found_note == false)
        return -1;

    //  Create iterators for accounts
    std::list<wlt::Account>::iterator acc_it;
    std::list<wlt::Account>::iterator acc_add_it;
    int acc_index;
    int acc_add_index;

    //		unchange Account Value

    switch(note_it->getOperation())
    {
    case wlt::INCOME:
        acc_index = this->_getAccount( note_it->getAccountName() );
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::EXPENSE:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::TRANSFER:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_add_index = this->_getAccount(note_it->getAccountNameAddit());
        acc_it = this->accounts.begin();
        acc_add_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);
        std::advance(acc_add_it, acc_add_index - 1);

        acc_it->changeCount(note_it->getValue());
        acc_add_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_add_it->getCount()) + " WHERE Name='" + acc_add_it->getName() + "'", this->callback);

        break;

    default:
        break;
    }

    //		Update Note to database and memory
    note_it->setOperation(OperationType);
    note_it->setCategory(CategoryType);
    note_it->setAccountName(AccountName);
    note_it->setValue(value);
    note_it->setDetails(details);

    //  Create sql query
    std::string sql_req = "UPDATE Notes SET Operation='" + std::to_string(static_cast<int>( note_it->getOperation() ) ) + "', CategoryType='" +  std::to_string(static_cast<int>( note_it->getCategory() ) ) +
                          "', AccountName='" + note_it->getAccountName() + "', AccountNameAddit='', Value='" + std::to_string( note_it->getValue() ) + "', Details='" + note_it->getDetails() +
                          "', Time='" + std::to_string(note_it->getEpochTime()) + "' WHERE ID='" + std::to_string( note_it->getId() ) + "'";

    this->SQLRequest(sql_req, this->callback);

    //  Change accounts count
    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, new_acc_index - 1);

    switch (OperationType)
    {
        case wlt::INCOME:
            acc_iter->changeCount(value);
            sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
            this->SQLRequest(sql_req, this->callback);
            break;
        case wlt::EXPENSE:
            acc_iter->changeCount(value * -1);
            sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
            this->SQLRequest(sql_req, this->callback);
            break;
        case wlt::TRANSFER:
            break;
        default:
            break;
    }

    return 1;
}
int wlt::eWallet::Edit_Note(unsigned long id, Operation OperationType, Category CategoryType, std::string AccountName, std::string AccountNameAddit,double value, std::string details)
{
    //  Check for TRANSFER Operation
    if(OperationType != wlt::Operation::TRANSFER)
        return -1;
    //  incorrect input check
    int new_acc_index = this->_getAccount(AccountName);
    if(new_acc_index == -1)
        return -1;

    int new_acc_add_index = this->_getAccount(AccountNameAddit);
    if(new_acc_add_index == -1)
        return-1;
    //  Find note with same id
    std::list<wlt::Note>::iterator note_it;
    bool found_note = false;

    for(note_it = this->notes.begin(); note_it != this->notes.end(); note_it++)
    {
        if(note_it->getId() == id)
        {
            found_note = true;
            break;
        }
    }
    if(found_note == false)
        return -1;

    //  Create iterators for accounts
    std::list<wlt::Account>::iterator acc_it;
    std::list<wlt::Account>::iterator acc_add_it;
    int acc_index;
    int acc_add_index;

    //		unchange Account Value

    switch(note_it->getOperation())
    {
    case wlt::INCOME:
        acc_index = this->_getAccount( note_it->getAccountName() );
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::EXPENSE:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);

        acc_it->changeCount(note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        break;
    case wlt::TRANSFER:
        acc_index = this->_getAccount(note_it->getAccountName());
        acc_add_index = this->_getAccount(note_it->getAccountNameAddit());
        acc_it = this->accounts.begin();
        acc_add_it = this->accounts.begin();
        std::advance(acc_it, acc_index - 1);
        std::advance(acc_add_it, acc_add_index - 1);

        acc_it->changeCount(note_it->getValue());
        acc_add_it->changeCount(-1 * note_it->getValue());

        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_it->getCount()) + " WHERE Name='" + acc_it->getName() + "'", this->callback);
        this->SQLRequest("UPDATE Accounts SET Count=" + std::to_string(acc_add_it->getCount()) + " WHERE Name='" + acc_add_it->getName() + "'", this->callback);

        break;

    default:
        break;
    }

    //		Update Note to database and memory
    note_it->setOperation(OperationType);
    note_it->setCategory(CategoryType);
    note_it->setAccountName(AccountName);
    note_it->setAccountNameAddit(AccountNameAddit);
    note_it->setValue(value);
    note_it->setDetails(details);

    //  Create sql query
    std::string sql_req = "UPDATE Notes SET Operation='" + std::to_string(static_cast<int>( note_it->getOperation() ) ) + "', CategoryType='" +  std::to_string(static_cast<int>( note_it->getCategory() ) ) +
                          "', AccountName='" + note_it->getAccountName() + "', AccountNameAddit='" + note_it->getAccountNameAddit() +"', Value='" + std::to_string( note_it->getValue() ) + "', Details='" + note_it->getDetails() +
                          "', Time='" + std::to_string(note_it->getEpochTime()) + "' WHERE ID='" + std::to_string( note_it->getId() ) + "'";

    this->SQLRequest(sql_req, this->callback);

    //  Change accounts count
    std::list<wlt::Account>::iterator acc_iter = this->accounts.begin();
    std::advance(acc_iter, new_acc_index - 1);

    std::list<wlt::Account>::iterator acc_add_iter = this->accounts.begin();
    std::advance(acc_add_iter, new_acc_add_index - 1);

    //  Change accounts count
    acc_iter->changeCount(value * -1);
    sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_iter->getCount()) + " WHERE Name='" + acc_iter->getName() + "'";
    this->SQLRequest(sql_req, this->callback);

    acc_add_iter->changeCount(value);
    sql_req = "UPDATE Accounts SET Count=" + std::to_string(acc_add_iter->getCount()) + " WHERE Name='" + acc_add_iter->getName() + "'";
    this->SQLRequest(sql_req, this->callback);

    return 1;
}

int wlt::eWallet::_getAccount(const std::string& Name)
{
	auto iter = this->accounts.begin();
	int index = 0;
	for (; iter != this->accounts.end(); iter++)
	{
		if (iter->getName() == Name)
		{
			return index+1;
		}
		index++;
	}

	return -1;
}

std::string wlt::eWallet::getCurrencyType()
{
	return this->Currency_Type;
}

std::string wlt::eWallet::getLocalLanguage()
{
	return this->LocalLanguage;
}

int wlt::eWallet::setLocalLanguage(std::string lang)
{
    this->SQLRequest("UPDATE Settings SET LocalLanguage = \""+ lang + "\"" , this->callback);
    this->LocalLanguage = lang;
    return 1;
}

void wlt::eWallet::addRate(std::string currency, double value)
{
	this->Rates[currency] = value;
}

void wlt::eWallet::showRates()
{
    // std::cout << "\t--= Rates =--" << std::endl;
    // for (auto it = Rates.begin(); it != Rates.end(); it++)
    // {
    // 	std::cout << "[" << it->first << "] = " << it->second << std::endl;
    // }
}

void wlt::eWallet::deleteRate(std::string key)
{
    auto it = this->Rates.find(key);
    if(it != this->Rates.end())
        this->Rates.erase(it);
}

int wlt::eWallet::inRatesTable(std::string key)
{
    if( this->Rates.find(key) == this->Rates.end())
        return -1;
    return 0;
}

bool wlt::eWallet::isRatesEmpty()
{
    return (!this->isRateValidData);
}

std::list<std::string> wlt::eWallet::getRatesNames()
{
    std::list<std::string> ret_lst;
    for(auto it = Rates.begin(); it != Rates.end(); it++)
    {
        ret_lst.push_back( it->first );
    }
    return ret_lst;

}

std::list<double> wlt::eWallet::getRatesValues()
{
    std::list<double> ret_lst;
    for(auto it = Rates.begin(); it != Rates.end(); it++)
    {
        ret_lst.push_back( it->second );
    }
    return ret_lst;
}


//			DB Query Functions


int wlt::eWallet::SQLRequest(std::string Query, int callback(void* NotUsed, int argc, char** argv, char** azColName))
{
	sqlite3* db;
	char* zErrMsg = 0;
	int rc;

	rc = sqlite3_open(this->db_file_name.c_str(), &db);

	if (rc) {
		//fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	else {
		//fprintf(stderr, "Opened database successfully\n");
	}

	rc = sqlite3_exec(db, Query.c_str(), callback, (void *) this, &zErrMsg);
	if (rc != SQLITE_OK)
	{
		//std::cout << "Error in SQL" << std::endl;
		sqlite3_free(zErrMsg);
		return -1;
	}

	sqlite3_close(db);

	return 0;
}

int wlt::eWallet::callback(void* NotUsed, int argc, char** argv, char** azColName)
{
	wlt::eWallet *wallet = (wlt::eWallet*)NotUsed;
	
	int i;
    // for (i = 0; i < argc; i++)
    // {
    // 	std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
    // }
    // std::cout << "\n";
	return 0;
}



int wlt::eWallet::callback_settings(void* NotUsed, int argc, char** argv, char** azColName)
{
	wlt::eWallet* wallet = (wlt::eWallet*)NotUsed;
	wallet->Currency_Type = std::string(argv[0]);
	wallet->LocalLanguage = std::string(argv[1]);
	wallet->LocalMonth = (unsigned short) atoi(argv[2]);

	return 0;
}

int wlt::eWallet::callback_accounts(void* NotUsed, int argc, char** argv, char** azColName)
{
	wlt::eWallet* wallet = (wlt::eWallet*)NotUsed;
	double value;
	sscanf_s(argv[2], "%lf", &value);

	wallet->Create_Account(argv[0], value, argv[1], false);

	return 0;
}


int wlt::eWallet::callback_notes(void* NotUsed, int argc, char** argv, char** azColName)
{
    try
    {
        wlt::eWallet* wallet = (wlt::eWallet*)NotUsed;

        wlt::Operation operation = static_cast<wlt::Operation>( atoi(argv[0]) );
        wlt::Category category = static_cast<wlt::Category>( atoi(argv[1]) );
        double value = atof(argv[4]);
        unsigned long long time = atoll(argv[6]);
        unsigned long id = atol(argv[7]);

        if (std::string(argv[3]) == "")
        {
            wallet->Add_Note_DB(operation, category, wallet->_getAccount(argv[2]), value, time, argv[5], id);
        }
        else
        {
            wallet->Add_Note_DB(operation, wallet->_getAccount(argv[2]), wallet->_getAccount(argv[3]),value, time, argv[5], id);
        }
        return 0;
    }
    catch(const std::exception &e)
    {
        return -1;
    }
}

int wlt::eWallet::callback_rates(void* NotUsed, int argc, char** argv, char** azColName)
{
	wlt::eWallet* wallet = (wlt::eWallet*)NotUsed;
	double value = atof(argv[1]);
	wallet->addRate(argv[0], value);
	
	return 0;
}

//			JSON API Functions

unsigned short wlt::eWallet::getMonth()
{
	time_t t = time(NULL);
	struct tm buf;
	_localtime64_s(&buf, &t);
	return (unsigned short) (buf.tm_mon + 1);

}

// std::string  wlt::eWallet::http_request_GET()
// {
//     //      HTTP API Request
//     QEventLoop eventLoop;


//     QNetworkAccessManager mgr;

//     QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)),
//                      &eventLoop, SLOT(quit()));


//     std::string url = "https://tradingeconomics.com/currencies?base=";
//     // std::string access_key = "${{secrets.DATAFIXER_API}}";
//     //std::string base = "EUR";   //  EDIT!!!
//     std::string base = this->Currency_Type;
//     std::string args = url + base;

//     QNetworkRequest req( QUrl(QString::fromStdString(args)) );

//     QNetworkReply *reply = mgr.get(req);
//     eventLoop.exec();

//     if(reply->error() == QNetworkReply::NoError)
//     {
//         //qDebug() << "Success" << reply->readAll().toStdString() ;
//         std::string ret = reply->readAll().toStdString();
//         delete reply;
//         return ret;
//     }
//     else{
//         //qDebug() << "Failure" << reply->errorString().toStdString();
//         std::string ret = "";
//         delete reply;
//         return ret;
//     }
// }

int wlt::eWallet::fillRatesTables()
{
    //std::string json_string = this->http_request_GET();

    QEventLoop eventLoop;

    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)),
                     &eventLoop, SLOT(quit()));

    std::string url = "https://tradingeconomics.com/currencies?base=";
    std::string base = this->Currency_Type;
    std::string args = url + base;
    QNetworkRequest req( QUrl(QString::fromStdString(args)) );

    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        //qDebug() << "Success";
        std::string ret = reply->readAll().toStdString();
        delete reply;
        this->isRateValidData = true;

        this->Rates.clear();
        this->SQLRequest("DELETE FROM Rates", this->callback);

        std::smatch match;
        std::regex reg(base + "(\\w\\w\\w)\\D*(\\d+.\\d+)\\s");

        // Поиск совпадений
        std::string::const_iterator searchStart(ret.cbegin());
        while (regex_search(searchStart, ret.cend(), match, reg)) {
            // Выводим найденные значения
            // std::cout << "Match 1: " << match[1].str() << ", Match 2: " << match[2].str() << std::endl;
            //qDebug() << "Match 1: " << match[1].str() << ", Match 2: " << std::stod(  match[2].str() );
            this->addRate(match[1].str(), std::stod(  match[2].str() ));
            this->SQLRequest("INSERT INTO Rates VALUES('" + match[1].str() + "', " + match[2].str() + ")", this->callback);
            // Смещаем итератор, чтобы искать дальше
            searchStart = match.suffix().first;
        }
        return 1;
    }
    else
    {
        //qDebug() << "Failure";
        delete reply;

        std::list<std::string> currencies = {
            "AED", // United Arab Emirates Dirham
            "AFN", // Afghan Afghani
            "ALL", // Albanian Lek
            "AMD", // Armenian Dram
            "ANG", // Netherlands Antillean Guilder
            "AOA", // Angolan Kwanza
            "ARS", // Argentine Peso
            "AUD", // Australian Dollar
            "AWG", // Aruban Florin
            "AZN", // Azerbaijani Manat
            "BAM", // Bosnia and Herzegovina Convertible Mark
            "BBD", // Barbadian Dollar
            "BDT", // Bangladeshi Taka
            "BGN", // Bulgarian Lev
            "BHD", // Bahraini Dinar
            "BIF", // Burundian Franc
            "BMD", // Bermudian Dollar
            "BND", // Brunei Dollar
            "BOB", // Bolivian Boliviano
            "BRL", // Brazilian Real
            "BSD", // Bahamian Dollar
            "BTN", // Bhutanese Ngultrum
            "BWP", // Botswanan Pula
            "BYN", // Belarusian Ruble
            "BZD", // Belize Dollar
            "CAD", // Canadian Dollar
            "CDF", // Congolese Franc
            "CHF", // Swiss Franc
            "CLP", // Chilean Peso
            "CNY", // Chinese Yuan
            "COP", // Colombian Peso
            "CRC", // Costa Rican Colón
            "CUC", // Cuban Convertible Peso
            "CUP", // Cuban Peso
            "CVE", // Cape Verdean Escudo
            "CZK", // Czech Koruna
            "DJF", // Djiboutian Franc
            "DKK", // Danish Krone
            "DOP", // Dominican Peso
            "DZD", // Algerian Dinar
            "EGP", // Egyptian Pound
            "ERN", // Eritrean Nakfa
            "ETB", // Ethiopian Birr
            "EUR", // Euro
            "FJD", // Fijian Dollar
            "FKP", // Falkland Islands Pound
            "FOK", // Faroese Króna
            "GBP", // British Pound Sterling
            "GEL", // Georgian Lari
            "GHS", // Ghanaian Cedi
            "GIP", // Gibraltar Pound
            "GMD", // Gambian Dalasi
            "GNF", // Guinean Franc
            "GTQ", // Guatemalan Quetzal
            "GYD", // Guyanaese Dollar
            "HKD", // Hong Kong Dollar
            "HNL", // Honduran Lempira
            "HRK", // Croatian Kuna
            "HTG", // Haitian Gourde
            "HUF", // Hungarian Forint
            "IDR", // Indonesian Rupiah
            "ILS", // Israeli New Shekel
            "INR", // Indian Rupee
            "IQD", // Iraqi Dinar
            "IRR", // Iranian Rial
            "ISK", // Icelandic Króna
            "JMD", // Jamaican Dollar
            "JOD", // Jordanian Dinar
            "JPY", // Japanese Yen
            "KES", // Kenyan Shilling
            "KGS", // Kyrgystani Som
            "KHR", // Cambodian Riel
            "KID", // Kiribati Dollar
            "KMF", // Comorian Franc
            "KRW", // South Korean Won
            "KWD", // Kuwaiti Dinar
            "KYD", // Cayman Islands Dollar
            "KZT", // Kazakhstani Tenge
            "LAK", // Laotian Kip
            "LBP", // Lebanese Pound
            "LKR", // Sri Lankan Rupee
            "LRD", // Liberian Dollar
            "LSL", // Lesotho Loti
            "LYD", // Libyan Dinar
            "MAD", // Moroccan Dirham
            "MDL", // Moldovan Leu
            "MGA", // Malagasy Ariary
            "MKD", // Macedonian Denar
            "MMK", // Myanma Kyat
            "MNT", // Mongolian Tugrik
            "MOP", // Macanese Pataca
            "MRO", // Mauritanian Ouguiya
            "MUR", // Mauritian Rupee
            "MVR", // Maldivian Rufiyaa
            "MWK", // Malawian Kwacha
            "MXN", // Mexican Peso
            "MYR", // Malaysian Ringgit
            "MZN", // Mozambican Metical
            "NAD", // Namibian Dollar
            "NGN", // Nigerian Naira
            "NIO", // Nicaraguan Córdoba
            "NOK", // Norwegian Krone
            "NPR", // Nepalese Rupee
            "NZD", // New Zealand Dollar
            "OMR", // Omani Rial
            "PAB", // Panamanian Balboa
            "PEN", // Peruvian Nuevo Sol
            "PGK", // Papua New Guinean Kina
            "PHP", // Philippine Peso
            "PKR", // Pakistani Rupee
            "PLN", // Polish Zloty
            "PYG", // Paraguayan Guarani
            "QAR", // Qatari Rial
            "RON", // Romanian Leu
            "RSD", // Serbian Dinar
            "RUB", // Russian Ruble
            "RWF", // Rwandan Franc
            "SAR", // Saudi Riyal
            "SBD", // Solomon Islands Dollar
            "SCR", // Seychellois Rupee
            "SDG", // Sudanese Pound
            "SEK", // Swedish Krona
            "SGD", // Singapore Dollar
            "SHP", // Saint Helena Pound
            "SLL", // Sierra Leonean Leone
            "SOS", // Somali Shilling
            "SRD", // Surinamese Dollar
            "SSP", // South Sudanese Pound
            "STN", // São Tomé and Príncipe Dobra
            "SYP", // Syrian Pound
            "SZL", // Swazi Lilangeni
            "THB", // Thai Baht
            "TJS", // Tajikistani Somoni
            "TMT", // Turkmenistani Manat
            "TND", // Tunisian Dinar
            "TOP", // Tongan Paʻanga
            "TRY", // Turkish Lira
            "TTD", // Trinidad and Tobago Dollar
            "TVD", // Tuvaluan Dollar
            "TWD", // New Taiwan Dollar
            "TZS", // Tanzanian Shilling
            "UAH", // Ukrainian Hryvnia
            "UGX", // Ugandan Shilling
            "USD", // United States Dollar
            "UYU", // Uruguayan Peso
            "UZS", // Uzbekistani Som
            "VES", // Venezuelan Bolívar Soberano
            "VND", // Vietnamese Dong
            "VUV", // Vanuatu Vatu
            "WST", // Samoan Tala
            "XAF", // Central African CFA Franc
            "XAG", // Silver Ounce
            "XAU", // Gold Ounce
            "XCD", // East Caribbean Dollar
            "XDR", // International Monetary Fund (IMF) Special Drawing Rights
            "XOF", // West African CFA Franc
            "XPF", // CFP Franc
            "YER", // Yemeni Rial
            "ZAR", // South African Rand
            "ZMK", // Zambian Kwacha (before 2013)
            "ZMW", // Zambian Kwacha (since 2013)
            "ZWL"  // Zimbabwean Dollar
        };
        this->Rates.clear();
        this->Currency_Type = "EUR";

        for(auto iter = currencies.begin(); iter != currencies.end(); iter++)
        {
            this->addRate(*iter, 0);
            this->SQLRequest("INSERT INTO Rates VALUES('" + *iter + "', " + std::to_string(0.0) + ")", this->callback);
        }
        this->isRateValidData = false;
        return -1;
    }

    // try{

    //     this->Rates.clear();
    //     this->SQLRequest("DELETE FROM Rates", this->callback);


    //         for (auto it = rates.begin(); it != rates.end(); it++)
    //         {

    //             this->addRate(it.key(), (double)it.value());

    //             this->SQLRequest("INSERT INTO Rates VALUES('" + it.key() + "', " + std::to_string((double)it.value()) + ")", this->callback);
    //         }
    //         return 1;
    //     }
    //     else
    //     {
    //         std::list<std::string> currencies = {
    //             "AED", // United Arab Emirates Dirham
    //             "AFN", // Afghan Afghani
    //             "ALL", // Albanian Lek
    //             "AMD", // Armenian Dram
    //             "ANG", // Netherlands Antillean Guilder
    //             "AOA", // Angolan Kwanza
    //             "ARS", // Argentine Peso
    //             "AUD", // Australian Dollar
    //             "AWG", // Aruban Florin
    //             "AZN", // Azerbaijani Manat
    //             "BAM", // Bosnia and Herzegovina Convertible Mark
    //             "BBD", // Barbadian Dollar
    //             "BDT", // Bangladeshi Taka
    //             "BGN", // Bulgarian Lev
    //             "BHD", // Bahraini Dinar
    //             "BIF", // Burundian Franc
    //             "BMD", // Bermudian Dollar
    //             "BND", // Brunei Dollar
    //             "BOB", // Bolivian Boliviano
    //             "BRL", // Brazilian Real
    //             "BSD", // Bahamian Dollar
    //             "BTN", // Bhutanese Ngultrum
    //             "BWP", // Botswanan Pula
    //             "BYN", // Belarusian Ruble
    //             "BZD", // Belize Dollar
    //             "CAD", // Canadian Dollar
    //             "CDF", // Congolese Franc
    //             "CHF", // Swiss Franc
    //             "CLP", // Chilean Peso
    //             "CNY", // Chinese Yuan
    //             "COP", // Colombian Peso
    //             "CRC", // Costa Rican Colón
    //             "CUC", // Cuban Convertible Peso
    //             "CUP", // Cuban Peso
    //             "CVE", // Cape Verdean Escudo
    //             "CZK", // Czech Koruna
    //             "DJF", // Djiboutian Franc
    //             "DKK", // Danish Krone
    //             "DOP", // Dominican Peso
    //             "DZD", // Algerian Dinar
    //             "EGP", // Egyptian Pound
    //             "ERN", // Eritrean Nakfa
    //             "ETB", // Ethiopian Birr
    //             "EUR", // Euro
    //             "FJD", // Fijian Dollar
    //             "FKP", // Falkland Islands Pound
    //             "FOK", // Faroese Króna
    //             "GBP", // British Pound Sterling
    //             "GEL", // Georgian Lari
    //             "GHS", // Ghanaian Cedi
    //             "GIP", // Gibraltar Pound
    //             "GMD", // Gambian Dalasi
    //             "GNF", // Guinean Franc
    //             "GTQ", // Guatemalan Quetzal
    //             "GYD", // Guyanaese Dollar
    //             "HKD", // Hong Kong Dollar
    //             "HNL", // Honduran Lempira
    //             "HRK", // Croatian Kuna
    //             "HTG", // Haitian Gourde
    //             "HUF", // Hungarian Forint
    //             "IDR", // Indonesian Rupiah
    //             "ILS", // Israeli New Shekel
    //             "INR", // Indian Rupee
    //             "IQD", // Iraqi Dinar
    //             "IRR", // Iranian Rial
    //             "ISK", // Icelandic Króna
    //             "JMD", // Jamaican Dollar
    //             "JOD", // Jordanian Dinar
    //             "JPY", // Japanese Yen
    //             "KES", // Kenyan Shilling
    //             "KGS", // Kyrgystani Som
    //             "KHR", // Cambodian Riel
    //             "KID", // Kiribati Dollar
    //             "KMF", // Comorian Franc
    //             "KRW", // South Korean Won
    //             "KWD", // Kuwaiti Dinar
    //             "KYD", // Cayman Islands Dollar
    //             "KZT", // Kazakhstani Tenge
    //             "LAK", // Laotian Kip
    //             "LBP", // Lebanese Pound
    //             "LKR", // Sri Lankan Rupee
    //             "LRD", // Liberian Dollar
    //             "LSL", // Lesotho Loti
    //             "LYD", // Libyan Dinar
    //             "MAD", // Moroccan Dirham
    //             "MDL", // Moldovan Leu
    //             "MGA", // Malagasy Ariary
    //             "MKD", // Macedonian Denar
    //             "MMK", // Myanma Kyat
    //             "MNT", // Mongolian Tugrik
    //             "MOP", // Macanese Pataca
    //             "MRO", // Mauritanian Ouguiya
    //             "MUR", // Mauritian Rupee
    //             "MVR", // Maldivian Rufiyaa
    //             "MWK", // Malawian Kwacha
    //             "MXN", // Mexican Peso
    //             "MYR", // Malaysian Ringgit
    //             "MZN", // Mozambican Metical
    //             "NAD", // Namibian Dollar
    //             "NGN", // Nigerian Naira
    //             "NIO", // Nicaraguan Córdoba
    //             "NOK", // Norwegian Krone
    //             "NPR", // Nepalese Rupee
    //             "NZD", // New Zealand Dollar
    //             "OMR", // Omani Rial
    //             "PAB", // Panamanian Balboa
    //             "PEN", // Peruvian Nuevo Sol
    //             "PGK", // Papua New Guinean Kina
    //             "PHP", // Philippine Peso
    //             "PKR", // Pakistani Rupee
    //             "PLN", // Polish Zloty
    //             "PYG", // Paraguayan Guarani
    //             "QAR", // Qatari Rial
    //             "RON", // Romanian Leu
    //             "RSD", // Serbian Dinar
    //             "RUB", // Russian Ruble
    //             "RWF", // Rwandan Franc
    //             "SAR", // Saudi Riyal
    //             "SBD", // Solomon Islands Dollar
    //             "SCR", // Seychellois Rupee
    //             "SDG", // Sudanese Pound
    //             "SEK", // Swedish Krona
    //             "SGD", // Singapore Dollar
    //             "SHP", // Saint Helena Pound
    //             "SLL", // Sierra Leonean Leone
    //             "SOS", // Somali Shilling
    //             "SRD", // Surinamese Dollar
    //             "SSP", // South Sudanese Pound
    //             "STN", // São Tomé and Príncipe Dobra
    //             "SYP", // Syrian Pound
    //             "SZL", // Swazi Lilangeni
    //             "THB", // Thai Baht
    //             "TJS", // Tajikistani Somoni
    //             "TMT", // Turkmenistani Manat
    //             "TND", // Tunisian Dinar
    //             "TOP", // Tongan Paʻanga
    //             "TRY", // Turkish Lira
    //             "TTD", // Trinidad and Tobago Dollar
    //             "TVD", // Tuvaluan Dollar
    //             "TWD", // New Taiwan Dollar
    //             "TZS", // Tanzanian Shilling
    //             "UAH", // Ukrainian Hryvnia
    //             "UGX", // Ugandan Shilling
    //             "USD", // United States Dollar
    //             "UYU", // Uruguayan Peso
    //             "UZS", // Uzbekistani Som
    //             "VES", // Venezuelan Bolívar Soberano
    //             "VND", // Vietnamese Dong
    //             "VUV", // Vanuatu Vatu
    //             "WST", // Samoan Tala
    //             "XAF", // Central African CFA Franc
    //             "XAG", // Silver Ounce
    //             "XAU", // Gold Ounce
    //             "XCD", // East Caribbean Dollar
    //             "XDR", // International Monetary Fund (IMF) Special Drawing Rights
    //             "XOF", // West African CFA Franc
    //             "XPF", // CFP Franc
    //             "YER", // Yemeni Rial
    //             "ZAR", // South African Rand
    //             "ZMK", // Zambian Kwacha (before 2013)
    //             "ZMW", // Zambian Kwacha (since 2013)
    //             "ZWL"  // Zimbabwean Dollar
    //         };
    //         this->Rates.clear();
    //         this->Currency_Type = "EUR";

    //         for(auto iter = currencies.begin(); iter != currencies.end(); iter++)
    //         {
    //             this->addRate(*iter, 0);
    //             this->SQLRequest("INSERT INTO Rates VALUES('" + *iter + "', " + std::to_string(0.0) + ")", this->callback);
    //         }
    //         this->isRateValidData = false;
    //         return -1;
    //     }

    // }
    // catch (const nlohmann::json::exception& e)
    // {
    //     std::list<std::string> currencies = {
    //         "AED", // United Arab Emirates Dirham
    //         "AFN", // Afghan Afghani
    //         "ALL", // Albanian Lek
    //         "AMD", // Armenian Dram
    //         "ANG", // Netherlands Antillean Guilder
    //         "AOA", // Angolan Kwanza
    //         "ARS", // Argentine Peso
    //         "AUD", // Australian Dollar
    //         "AWG", // Aruban Florin
    //         "AZN", // Azerbaijani Manat
    //         "BAM", // Bosnia and Herzegovina Convertible Mark
    //         "BBD", // Barbadian Dollar
    //         "BDT", // Bangladeshi Taka
    //         "BGN", // Bulgarian Lev
    //         "BHD", // Bahraini Dinar
    //         "BIF", // Burundian Franc
    //         "BMD", // Bermudian Dollar
    //         "BND", // Brunei Dollar
    //         "BOB", // Bolivian Boliviano
    //         "BRL", // Brazilian Real
    //         "BSD", // Bahamian Dollar
    //         "BTN", // Bhutanese Ngultrum
    //         "BWP", // Botswanan Pula
    //         "BYN", // Belarusian Ruble
    //         "BZD", // Belize Dollar
    //         "CAD", // Canadian Dollar
    //         "CDF", // Congolese Franc
    //         "CHF", // Swiss Franc
    //         "CLP", // Chilean Peso
    //         "CNY", // Chinese Yuan
    //         "COP", // Colombian Peso
    //         "CRC", // Costa Rican Colón
    //         "CUC", // Cuban Convertible Peso
    //         "CUP", // Cuban Peso
    //         "CVE", // Cape Verdean Escudo
    //         "CZK", // Czech Koruna
    //         "DJF", // Djiboutian Franc
    //         "DKK", // Danish Krone
    //         "DOP", // Dominican Peso
    //         "DZD", // Algerian Dinar
    //         "EGP", // Egyptian Pound
    //         "ERN", // Eritrean Nakfa
    //         "ETB", // Ethiopian Birr
    //         "EUR", // Euro
    //         "FJD", // Fijian Dollar
    //         "FKP", // Falkland Islands Pound
    //         "FOK", // Faroese Króna
    //         "GBP", // British Pound Sterling
    //         "GEL", // Georgian Lari
    //         "GHS", // Ghanaian Cedi
    //         "GIP", // Gibraltar Pound
    //         "GMD", // Gambian Dalasi
    //         "GNF", // Guinean Franc
    //         "GTQ", // Guatemalan Quetzal
    //         "GYD", // Guyanaese Dollar
    //         "HKD", // Hong Kong Dollar
    //         "HNL", // Honduran Lempira
    //         "HRK", // Croatian Kuna
    //         "HTG", // Haitian Gourde
    //         "HUF", // Hungarian Forint
    //         "IDR", // Indonesian Rupiah
    //         "ILS", // Israeli New Shekel
    //         "INR", // Indian Rupee
    //         "IQD", // Iraqi Dinar
    //         "IRR", // Iranian Rial
    //         "ISK", // Icelandic Króna
    //         "JMD", // Jamaican Dollar
    //         "JOD", // Jordanian Dinar
    //         "JPY", // Japanese Yen
    //         "KES", // Kenyan Shilling
    //         "KGS", // Kyrgystani Som
    //         "KHR", // Cambodian Riel
    //         "KID", // Kiribati Dollar
    //         "KMF", // Comorian Franc
    //         "KRW", // South Korean Won
    //         "KWD", // Kuwaiti Dinar
    //         "KYD", // Cayman Islands Dollar
    //         "KZT", // Kazakhstani Tenge
    //         "LAK", // Laotian Kip
    //         "LBP", // Lebanese Pound
    //         "LKR", // Sri Lankan Rupee
    //         "LRD", // Liberian Dollar
    //         "LSL", // Lesotho Loti
    //         "LYD", // Libyan Dinar
    //         "MAD", // Moroccan Dirham
    //         "MDL", // Moldovan Leu
    //         "MGA", // Malagasy Ariary
    //         "MKD", // Macedonian Denar
    //         "MMK", // Myanma Kyat
    //         "MNT", // Mongolian Tugrik
    //         "MOP", // Macanese Pataca
    //         "MRO", // Mauritanian Ouguiya
    //         "MUR", // Mauritian Rupee
    //         "MVR", // Maldivian Rufiyaa
    //         "MWK", // Malawian Kwacha
    //         "MXN", // Mexican Peso
    //         "MYR", // Malaysian Ringgit
    //         "MZN", // Mozambican Metical
    //         "NAD", // Namibian Dollar
    //         "NGN", // Nigerian Naira
    //         "NIO", // Nicaraguan Córdoba
    //         "NOK", // Norwegian Krone
    //         "NPR", // Nepalese Rupee
    //         "NZD", // New Zealand Dollar
    //         "OMR", // Omani Rial
    //         "PAB", // Panamanian Balboa
    //         "PEN", // Peruvian Nuevo Sol
    //         "PGK", // Papua New Guinean Kina
    //         "PHP", // Philippine Peso
    //         "PKR", // Pakistani Rupee
    //         "PLN", // Polish Zloty
    //         "PYG", // Paraguayan Guarani
    //         "QAR", // Qatari Rial
    //         "RON", // Romanian Leu
    //         "RSD", // Serbian Dinar
    //         "RUB", // Russian Ruble
    //         "RWF", // Rwandan Franc
    //         "SAR", // Saudi Riyal
    //         "SBD", // Solomon Islands Dollar
    //         "SCR", // Seychellois Rupee
    //         "SDG", // Sudanese Pound
    //         "SEK", // Swedish Krona
    //         "SGD", // Singapore Dollar
    //         "SHP", // Saint Helena Pound
    //         "SLL", // Sierra Leonean Leone
    //         "SOS", // Somali Shilling
    //         "SRD", // Surinamese Dollar
    //         "SSP", // South Sudanese Pound
    //         "STN", // São Tomé and Príncipe Dobra
    //         "SYP", // Syrian Pound
    //         "SZL", // Swazi Lilangeni
    //         "THB", // Thai Baht
    //         "TJS", // Tajikistani Somoni
    //         "TMT", // Turkmenistani Manat
    //         "TND", // Tunisian Dinar
    //         "TOP", // Tongan Paʻanga
    //         "TRY", // Turkish Lira
    //         "TTD", // Trinidad and Tobago Dollar
    //         "TVD", // Tuvaluan Dollar
    //         "TWD", // New Taiwan Dollar
    //         "TZS", // Tanzanian Shilling
    //         "UAH", // Ukrainian Hryvnia
    //         "UGX", // Ugandan Shilling
    //         "USD", // United States Dollar
    //         "UYU", // Uruguayan Peso
    //         "UZS", // Uzbekistani Som
    //         "VES", // Venezuelan Bolívar Soberano
    //         "VND", // Vietnamese Dong
    //         "VUV", // Vanuatu Vatu
    //         "WST", // Samoan Tala
    //         "XAF", // Central African CFA Franc
    //         "XAG", // Silver Ounce
    //         "XAU", // Gold Ounce
    //         "XCD", // East Caribbean Dollar
    //         "XDR", // International Monetary Fund (IMF) Special Drawing Rights
    //         "XOF", // West African CFA Franc
    //         "XPF", // CFP Franc
    //         "YER", // Yemeni Rial
    //         "ZAR", // South African Rand
    //         "ZMK", // Zambian Kwacha (before 2013)
    //         "ZMW", // Zambian Kwacha (since 2013)
    //         "ZWL"  // Zimbabwean Dollar
    //     };
    //     this->Rates.clear();
    //     this->Currency_Type = "EUR";

    //     for(auto iter = currencies.begin(); iter != currencies.end(); iter++)
    //     {
    //         this->addRate(*iter, 0);
    //         this->SQLRequest("INSERT INTO Rates VALUES('" + *iter + "', " + std::to_string(0.0) + ")", this->callback);
    //     }
    //     this->isRateValidData = false;
    //     return -1;
    // }
}

void wlt::eWallet::resetProgramDataFromDB()
{
    this->accounts.clear();
    this->notes.clear();
    this->Rates.clear();

    this->SQLRequest("select * from Accounts", this->callback_accounts);
    this->SQLRequest("select * from Notes", this->callback_notes);
    this->notes.reverse();
    this->SQLRequest("select * from Rates", this->callback_rates);
}

std::list<wlt::Account> wlt::eWallet::getAccountList()
{
    std::list<wlt::Account> ret_list;
    for(auto it = this->accounts.begin(); it != this->accounts.end(); it++)
    {
        ret_list.push_back(*it);
    }
    return ret_list;
}

std::list<wlt::Note> wlt::eWallet::getNoteListAll()
{
    std::list<wlt::Note> ret_list;
    for(auto it = this->notes.begin(); it != this->notes.end(); it++)
    {
        ret_list.push_back(*it);
    }
    return ret_list;
}

std::list<wlt::Note> wlt::eWallet::getNoteListLast(int size)
{
    std::list<wlt::Note> ret_list;
    std::list<wlt::Note>::iterator it = this->notes.begin();

    for(int i = 0; i < size && it != this->notes.end(); i++)
    {
        ret_list.push_back(*it);
        it++;
    }
    return ret_list;

}

std::string wlt::eWallet::getCurrencyFromAccName(std::string AccName)
{
    int index = this->_getAccount(AccName);
    if(index == -1)
        return "";

    std::list<wlt::Account>::iterator iter = this->accounts.begin();
    std::advance(iter, index-1);

    return iter->getCurrencyType();

}

unsigned int wlt::eWallet::AccountsCount()
{
    return this->accounts.size();
}

unsigned int wlt::eWallet::NotesCount()
{
    return this->notes.size();
}

double wlt::eWallet::getTotalCount()
{
    double total = 0;
    for(auto it = this->accounts.begin(); it != this->accounts.end(); it++)
    {
        if(this->Currency_Type == it->getCurrencyType())
        {
            total += it->getCount();
        }
        else
        {
            if(this->isRateValidData)
                total += it->getCount() / this->Rates[it->getCurrencyType()];
        }
    }

    return total;
}

std::vector<double> wlt::eWallet::getDataStat(unsigned int limit_days)
{
    std::vector<double> values = std::vector<double>(10, 0);
    QDate currentDate = QDate::currentDate();

    for(auto iter = this->notes.begin(); iter != this->notes.end(); iter++)
    {
        QDate noteDate(iter->getYear(), iter->getMonth(), iter->getDay());

        if( noteDate.daysTo(currentDate) > limit_days)
            break;

        if(static_cast<int>(iter->getOperation() ) != 1 && static_cast<int>(iter->getOperation() ) != 0 )
            continue;

        double data_value = iter->getValue();
        wlt::Account note_account = this->getAccountByName(iter->getAccountName());

        if(note_account.getCurrencyType() != this->Currency_Type)
        {
            if(this->isRateValidData)
                data_value = data_value / this->Rates[note_account.getCurrencyType()];
            else
                continue;
        }



        switch(static_cast<int>(iter->getCategory() ) )
        {
            case 0: values[0] = values[0] + data_value;
                break;
            case 1: values[1] = values[1] + data_value;
                break;
            case 2: values[2] = values[2] + data_value;
                break;
            case 3: values[3] = values[3] + data_value;
                break;
            case 4: values[4] = values[4] + data_value;
                break;
            case 5: values[5] = values[5] + data_value;
                break;
            case 6: values[6] = values[6] + data_value;
                break;
            case 7: values[7] = values[7] + data_value;
                break;
            case 8: values[8] = values[8] + data_value;
                break;
            case 9: values[9] = values[9] + data_value;
                break;
            default: ;
                break;
        }
    }

    return values;

}

std::list<wlt::Note> wlt::eWallet::getNotesUptoDate(unsigned int days)
{
    std::list<wlt::Note> ret_list;

    QDate currentDate = QDate::currentDate();

    for(auto iter = this->notes.begin(); iter != this->notes.end(); iter++)
    {
        QDate noteDate(iter->getYear(), iter->getMonth(), iter->getDay());

        if( noteDate.daysTo(currentDate) > days)
            break;

       ret_list.push_back(*iter);
    }

    return ret_list;
}

void wlt::eWallet::setCurrencyType(std::string currency_type)
{
    std::string prev_cur = this->Currency_Type;

    this->Currency_Type = currency_type;

    if(this->Rates.find(this->Currency_Type) == this->Rates.end())
        this->Currency_Type = prev_cur;
    else
    {
        this->SQLRequest("UPDATE Settings SET BaseCurrency='" + currency_type + "'" , this->callback);
        this->fillRatesTables();
    }
}

