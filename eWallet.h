#ifndef EWALLET
#define EWALLET

#include "note.h"
#include "account.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QEventLoop>
#include <QUrlQuery>


#include <iostream>		


#include <list>
#include <string>
#include <map>

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
		std::string LocalLanguage = "en";
		unsigned short LocalMonth = 1;

		std::map<std::string, double> Rates;


		int _getAccount(const std::string &str);	

        //int request_GET_Rates();
        int fillRatesTables();
        std::string  http_request_GET();
		unsigned short getMonth();
		

		//		DB Query functions
		int SQLRequest(std::string Query, int callback(void* NotUsed, int argc, char** argv, char** azColName));
		
		static int callback(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_settings(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_accounts(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_notes(void* NotUsed, int argc, char** argv, char** azColName);
		static int callback_rates(void* NotUsed, int argc, char** argv, char** azColName);

	public:
		eWallet();
		~eWallet();

		int Create_Account(const std::string &Name, const double Value = 0, const std::string new_currency = "", bool addToDB = true);
		void Show_Accounts_All();
		int Show_Account(int index);
		int Remove_Account(int index);
		
		int Create_Note(Operation OperationType, Category CategoryType, int index, double value, std::string details="");
		int Create_Note(Operation OperationType, int index1, int index2, double value, std::string details="");
		int Add_Note_DB(Operation OperationType, Category CategoryType, int index, double value,  unsigned long long secs = 0,std::string details = "", unsigned long id = 0);
		int Add_Note_DB(Operation OperationType, int index1, int index2, double value, unsigned long long secs = 0, std::string details = "", unsigned long id=0);
		void Show_Notes_All();
		int Show_Note(int index);
		int Remove_Note(int index);

		std::string getCurrencyType();
		std::string getLocalLanguage();

		void addRate(std::string currency, double value);
		void showRate();


	};

}

#endif
