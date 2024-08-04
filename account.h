#ifndef ACCOUNT
#define ACCOUNT

#include <string>

namespace wlt {


	class Account {

	private:
		std::string Name;
		std::string CurrencyType;
		double Count;

	public:

		Account(const std::string& Name, std::string CurrencyType = " USD ", double Count = 0);
		Account(const char* Name, std::string CurrencyType = " USD ", double Count = 0);
		~Account();

		void setName(const std::string &str);
		std::string getName();

		void setCurrencyType(const std::string &str);
		std::string getCurrencyType();

		void setCount(double c);
		double getCount();
		void changeCount(double val);

		std::string debugInfo();


	};

}


#endif
