// Account.cpp 

#include "account.h"



wlt::Account::Account(const std::string& Name, std::string CurrencyType, double Count)
{
	this->Name = Name;
	this->CurrencyType = CurrencyType;
	this->Count = Count;
}

wlt::Account::Account(const char* Name, std::string CurrencyType, double Count)
{
	std::string s(Name);
	this->Name = s;
	this->CurrencyType = CurrencyType;
	this->Count = Count;
}


wlt::Account::~Account()
{
	
}


void wlt::Account::setName(const std::string &str)
{
	this->Name = str;
}
std::string wlt::Account::getName()
{
	return this->Name;
}

void wlt::Account::setCurrencyType(const std::string& str)
{
	this->CurrencyType = str;
}
std::string wlt::Account::getCurrencyType()
{
	return this->CurrencyType;
}

void wlt::Account::setCount(double c)
{
	this->Count = c;
}
double wlt::Account::getCount()
{
	return this->Count;
}

std::string wlt::Account::debugInfo()
{
	std::string str = "Name: \'" + this->Name + "\'\tValue: " + std::to_string( this->Count ) + " " + this->CurrencyType;
	return str;
}

void wlt::Account::changeCount(double val)
{
	this->Count = this->Count + val;

}
