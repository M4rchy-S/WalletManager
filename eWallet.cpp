#include "eWallet.h"


wlt::eWallet::eWallet()
{
	this->accounts = std::list<wlt::Account>();
	this->notes = std::list<wlt::Note>();

	//	Try to load db
	if (this->SQLRequest("select * from Settings", this->callback_settings) == -1)
	{
		//	Create db manually
		std::cout << "Creating db" << std::endl;

		 
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
	}
	else
	{
		//	Getting all data from db
		std::cout << "Adding data to memory from db" << std::endl;

		this->SQLRequest("select * from Accounts", this->callback_accounts);
		this->SQLRequest("select * from Notes", this->callback_notes);
		this->SQLRequest("select * from Rates", this->callback_rates);

		//		Get Rates from API

		if (this->LocalMonth != this->getMonth())
		{
			this->LocalMonth = this->getMonth();
			this->SQLRequest("UPDATE Settings SET Month = " + std::to_string(this->LocalMonth), this->callback);
            this->fillRatesTables();
		}
	}
	
}



wlt::eWallet::~eWallet()
{
	this->accounts.clear();
	this->notes.clear();
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

	return 0;
}

void wlt::eWallet::Show_Accounts_All()
{
	std::cout << "\t--= Accounts =--" << std::endl;
	int i = 0;
	for (auto it = this->accounts.begin(); it != this->accounts.end(); it++, i++)
	{
		std::cout << i+1 << ": " << it->debugInfo() << std::endl;
	}
	std::cout << std::endl;
}

int wlt::eWallet::Show_Account(int index)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	std::list<wlt::Account>::iterator it = this->accounts.begin();
	std::advance(it, index - 1);

	std::cout << "\t--= Account =--" << std::endl;
	std::cout << index << ": " << it->debugInfo() << std::endl;

	std::cout << std::endl;

	return 0;
}

int wlt::eWallet::Remove_Account(int index)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	std::list<wlt::Account>::iterator it = this->accounts.begin();
	std::advance(it, index - 1);

	this->SQLRequest("DELETE from Accounts WHERE Name=\"" + it->getName() + "\"", callback);
	this->accounts.erase(it);

	return 0;
}

int wlt::eWallet::Create_Note(Operation OperationType, Category CategoryType, int index, double value, std::string details)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	auto it = this->accounts.begin();
	std::advance(it, index - 1);


	//	SQL Request
	auto note_iterator = this->notes.begin();
	std::advance(note_iterator, this->notes.size() - 1);
	int id = note_iterator->getId() + 1;
	
	this->notes.push_back( wlt::Note(OperationType, CategoryType, it->getName(), value, id, details));

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

	auto note_iterator = this->notes.begin();
	std::advance(note_iterator, this->notes.size() - 1);
	int id = note_iterator->getId() + 1;

	this->notes.push_back(wlt::Note(OperationType, it1->getName(), it2->getName(), value, id, details));

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
}

int wlt::eWallet::Add_Note_DB(Operation OperationType, Category CategoryType, int index, double value, unsigned long long time  ,std::string details, unsigned long id)
{
	if (index <= 0 || index > this->accounts.size())
		return -1;

	auto it = this->accounts.begin();
	std::advance(it, index - 1);

	this->notes.push_back(wlt::Note(OperationType, CategoryType, it->getName(), value, id, details, time));

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

}


void wlt::eWallet::Show_Notes_All()
{
	std::cout << "\t--= Notes =--" << std::endl;
	int i = 0;
	for (auto it = notes.begin(); it != notes.end(); it++, i++)
	{
		std::cout << i + 1 << ": ID[" << it->getId() << "] " << it->debugInfo() << "\tDate: " << it->getTimeInfo() << std::endl;
	}
	std::cout << std::endl;
}

int wlt::eWallet::Show_Note(int index)
{
	if (index <= 0 || index > this->notes.size())
		return -1;

	auto it = this->notes.begin();
	std::advance(it, index - 1);

	std::cout << "\t--= Note =--" << std::endl;
	std::cout << index << ": " << it->debugInfo() << "\tDate: " << it->getTimeInfo() << std::endl;

	std::cout << std::endl;
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

void wlt::eWallet::addRate(std::string currency, double value)
{
	this->Rates[currency] = value;
}

void wlt::eWallet::showRate()
{
	std::cout << "\t--= Rates =--" << std::endl;
	for (auto it = Rates.begin(); it != Rates.end(); it++)
	{
		std::cout << "[" << it->first << "] = " << it->second << std::endl;
	}
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
	for (i = 0; i < argc; i++)
	{
		std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << "\n";
	}
	std::cout << "\n";
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

std::string  wlt::eWallet::http_request_GET()
{
    //      HTTP API Request
    QEventLoop eventLoop;


    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)),
                     &eventLoop, SLOT(quit()));


    std::string url = "http://data.fixer.io";
    std::string access_key = "2e9d372d0563e0d1a409f40f72917696";
    //std::string base = "EUR";   //  EDIT!!!
    std::string base = this->Currency_Type;
    std::string args = "/api/latest?access_key=" + access_key + "&base=" + base;

    QNetworkRequest req( QUrl(QString::fromStdString(url + args)) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        //qDebug() << "Success" << reply->readAll().toStdString() ;
        std::string ret = reply->readAll().toStdString();
        delete reply;
        return ret;
    }
    else{
        //qDebug() << "Failure" << reply->errorString().toStdString();
        std::string ret = "";
        delete reply;
        return ret;
    }
}

int wlt::eWallet::fillRatesTables()
{
    std::string json_string = this->http_request_GET();
    try{
        nlohmann::json data = nlohmann::json::parse(json_string);

        if (data["success"].dump() == "true") {

            nlohmann::json rates = data["rates"];
            this->Rates.clear();
            this->SQLRequest("DELETE FROM Rates", this->callback);


            for (auto it = rates.begin(); it != rates.end(); it++)
            {
                //std::cout << it.key() << ": " << it.value() << std::endl;
                this->addRate(it.key(), (double)it.value());
                //std::string sql = "INSERT INTO Rates VALUES('" + it.key() + "', " + std::to_string((double)it.value()) + ")";
                this->SQLRequest("INSERT INTO Rates VALUES('" + it.key() + "', " + std::to_string((double)it.value()) + ")", this->callback);
            }
            return 1;
        }
        else
        {
            std::cout << "Unsuccessful request GET " << std::endl;
            return -1;
        }

        //std::cout << rates << std::endl;
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "Error while parsing: " << e.what() << std::endl;
        return -1;
    }
}

/*

int wlt::eWallet::request_GET_Rates()
{
	//			API Request

	std::string url = "http://data.fixer.io";
	std::string access_key = "2e9d372d0563e0d1a409f40f72917696";
	std::string base = this->Currency_Type;
	std::string args = "/api/latest?access_key=" + access_key + "&base=" + base;
	//std::string args = "/api/latest";		//		For testing


	httplib::Client cli(url);
	auto response = cli.Get(args);

	//		JSON Parsing

	try {
		nlohmann::json data = nlohmann::json::parse(response->body);

		if (data["success"].dump() == "true") {

			nlohmann::json rates = data["rates"];
			this->Rates.clear();
			this->SQLRequest("DELETE FROM Rates", this->callback);


			for (auto it = rates.begin(); it != rates.end(); it++)
			{
				//std::cout << it.key() << ": " << it.value() << std::endl;
				this->addRate(it.key(), (double)it.value());
				//std::string sql = "INSERT INTO Rates VALUES('" + it.key() + "', " + std::to_string((double)it.value()) + ")";
				this->SQLRequest("INSERT INTO Rates VALUES('" + it.key() + "', " + std::to_string((double)it.value()) + ")", this->callback);
			}
		}
		else
		{
			std::cout << "Unsuccessful request GET " << std::endl;
		}

		//std::cout << rates << std::endl;
	}
	catch (const nlohmann::json::exception& e)
	{
		std::cerr << "Error while parsing: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
*/
