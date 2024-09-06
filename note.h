#ifndef NOTE
#define NOTE

#include <string>
#include <ctime>



namespace wlt {

	enum Operation { INCOME, EXPENSE , TRANSFER};
    enum Category { FOOD, PRODUCT, HEALTHCARE, TRANSPORT, CAR, ENTERTAINMENT, NETWORK, FINANCE_EXPENSIES, TRANSFER_OPERATION, EARNINGS};


	class Note {

	private:
		Operation OperationType;
		Category CategoryType;
		std::string AccountName;
		std::string AccountNameAddit;
		double Value;
		std::string Details;

		unsigned long ID;

		//	Time data
		time_t EpochTime;

		unsigned int minutes;
		unsigned int hours;
		unsigned int day;
		unsigned int month;
		unsigned int year;

		void fillTimeData(unsigned long long seconds=0);
		
		
	public:

		Note(Operation, Category, std::string Acc1, double val, unsigned long id, std::string details="", unsigned long long seconds=0);
		Note(Operation, std::string Acc1, std::string Acc2, double val, unsigned long id, std::string details = "", unsigned long long seconds = 0);
		Note(Operation, Category, const char* str1, double val, unsigned long id, std::string details = "", unsigned long long seconds = 0);
		Note(Operation, const char* str1, const char* str2, double val, unsigned long id, std::string details = "", unsigned long long seconds = 0);

		~Note();

		Operation getOperation();
		void setOperation(Operation);

		Category getCategory();
		void setCategory(Category);

		std::string getAccountName();
		void setAccountName(std::string &);

		std::string getAccountNameAddit();
		void setAccountNameAddit(std::string&);

		double getValue();
		void setValue(double);

        std::string getDetails();
        void setDetails(std::string new_details);

        unsigned int getDay();
        unsigned int getMonth();
        unsigned int getYear();

		unsigned long getId();

		std::string debugInfo();
		std::string getTimeInfo();

		time_t getEpochTime();

        std::string getCategoryString();

	};



}


#endif
