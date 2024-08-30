#ifndef EWALLET
#define EWALLET

#include "note.h"
#include "account.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QUrlQuery>
#include <QDate>

//#include <iostream>  // !! Delete after

#include <list>
#include <string>
#include <map>
#include <vector>


#include <sqlite3.h>
#include <nlohmann/json.hpp>


namespace wlt {

	class eWallet
	{
	private:
		std::list<wlt::Account> accounts;
		std::list<wlt::Note> notes;
		
		const std::string db_file_name = "mydbtest.db";
		
        std::string Currency_Type = "EUR";
        std::string LocalLanguage = "EN";
		unsigned short LocalMonth = 1;

		std::map<std::string, double> Rates;


		int _getAccount(const std::string &str);	

        //int request_GET_Rates();
        int fillRatesTables();
        std::string  http_request_GET();
		unsigned short getMonth();
		

		//		DB Query functions
		int SQLRequest(std::string Query, int callback(void* NotUsed, int argc, char** argv, char** azColName));

        void resetProgramDataFromDB();
		
		static int callback(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_settings(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_accounts(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_notes(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_rates(void* NotUsed, int argc, char** argv, char** azColName);

	public:
        //  Constructor & Descrtuctor
		eWallet();
		~eWallet();

        //  Account Functions
		int Create_Account(const std::string &Name, const double Value = 0, const std::string new_currency = "", bool addToDB = true);
		void Show_Accounts_All();
		int Show_Account(int index);
		int Remove_Account(int index);
        int Remove_Account(std::string accName);
        int Edit_Account(std::string accName, double Value, std::string new_currency, bool addToDB = true);
        wlt::Account getAccountByName(std::string accName);

        //  Note functions
		int Create_Note(Operation OperationType, Category CategoryType, int index, double value, std::string details="");
		int Create_Note(Operation OperationType, int index1, int index2, double value, std::string details="");
		int Add_Note_DB(Operation OperationType, Category CategoryType, int index, double value,  unsigned long long secs = 0,std::string details = "", unsigned long id = 0);
		int Add_Note_DB(Operation OperationType, int index1, int index2, double value, unsigned long long secs = 0, std::string details = "", unsigned long id=0);
		void Show_Notes_All();
		int Show_Note(int index);
		int Remove_Note(int index);
        std::list<wlt::Note>::iterator getNoteByID(unsigned long id);

        //  Form functions
        int Create_Note(Operation OperationType, Category CategoryType, std::string AccountName, double value, std::string details="");
        int Create_Note(Operation OperationType, Category CategoryType, std::string AccountName, std::string AccountNameAddit,double value, std::string details="");
        int Edit_Note(unsigned long id, Operation OperationType, Category CategoryType, std::string AccountName, double value, std::string details);
        int Edit_Note(unsigned long id, Operation OperationType, Category CategoryType, std::string AccountName, std::string AccountNameAddit,double value, std::string details);
        int Remove_Note(unsigned long id);// Done


        //  Utility functions
		std::string getCurrencyType();
		std::string getLocalLanguage();
        std::list<wlt::Account> getAccountList();
        std::list<wlt::Note> getNoteListAll();
        std::list<wlt::Note> getNoteListLast(int size=5);
        std::string getCurrencyFromAccName(std::string AccName);
        unsigned int AccountsCount();

        std::vector<double> getDataStat(unsigned int limit_days=30);
        std::list<wlt::Note> getNotesUptoDate(unsigned int days=30);

        //  Currency functions
        double getTotalCount();
        void setCurrencyType(std::string currency_type);

        //  Rates functions
		void addRate(std::string currency, double value);
        void showRates();
        void deleteRate(std::string key);
        std::list<std::string> getRatesNames();
        std::list<double> getRatesValues();
        int inRatesTable(std::string key);


	};

}

#endif
