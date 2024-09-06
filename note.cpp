//	note.cpp

#include "eWallet.h"
#include "note.h"
#include <ctime>


wlt::Note::Note(Operation OperationType, Category CategoryType, std::string Acc_1, double Value, unsigned long id, std::string details, unsigned long long seconds )
{
	this->OperationType = OperationType;
	this->CategoryType = CategoryType;
	this->AccountName = Acc_1;
	this->Value = Value;
	this->Details = details;
	this->ID = id;

	this->fillTimeData(seconds);
}

wlt::Note::Note(Operation OperationType, std::string Acc1, std::string Acc2, double val, unsigned long id, std::string details, unsigned long long seconds )
{
	if (OperationType != wlt::Operation::TRANSFER)
		throw std::error_condition();

	this->OperationType = OperationType;
	this->CategoryType = wlt::Category::TRANSFER_OPERATION;
	this->AccountName = Acc1;
	this->AccountNameAddit = Acc2;
	this->Value = val;
	this->Details = details;
	this->ID = id;

	this->fillTimeData(seconds);
}

wlt::Note::Note(Operation OperationType, Category CategoryType, const char* str1, double Value, unsigned long id, std::string details, unsigned long long seconds )
{
	std::string Acc1(str1);
	this->OperationType = OperationType;
	this->CategoryType = CategoryType;
	this->AccountName = Acc1;
	this->Value = Value;
	this->Details = details;
	this->ID = id;

	this->fillTimeData(seconds);
}

wlt::Note::Note(Operation, const char* str1, const char* str2, double val, unsigned long id, std::string details, unsigned long long seconds )
{
	if (OperationType != wlt::Operation::TRANSFER)
		throw std::error_condition();

	std::string Acc1(str1);
	std::string Acc2(str2);
	this->OperationType = OperationType;
	//this->CategoryType = CategoryType;
	this->AccountName = Acc1;
	this->AccountNameAddit = Acc2;
	this->Value = Value;
	this->Details = details;
	this->ID = id;

	this->fillTimeData(seconds);
}

wlt::Note::~Note()
{

}

wlt::Operation wlt::Note::getOperation()
{
	return this->OperationType;
}
void wlt::Note::setOperation(wlt::Operation OperationType)
{
	this->OperationType = OperationType;
}

wlt::Category wlt::Note::getCategory()
{
	return this->CategoryType;
}
void wlt::Note::setCategory(wlt::Category CatType)
{
	this->CategoryType = CatType;
}

std::string wlt::Note::getAccountName()
{
	return this->AccountName;
}
void wlt::Note::setAccountName(std::string& NewName)
{
	this->AccountName = NewName;
}

double wlt::Note::getValue()
{
	return this->Value;
}
void wlt::Note::setValue(double NewValue)
{
	this->Value = NewValue;
}

unsigned long wlt::Note::getId()
{
	return this->ID;
}

std::string wlt::Note::debugInfo()
{
	std::string Oper;
	std::string Categ;


	switch(this->OperationType)
	{
		case 0: Oper = "Income";
			break;
		case 1: Oper = "Expense";
			break;
		case 2: Oper = "Transfer";
			break;
		default:
			Oper = "";
			break;
	}

	switch (this->CategoryType)
	{
		case 0: Categ = "Food";
			break;
		case 1:Categ = "Products";
			break;
        case 2: Categ = "Healthcare";
			break;
        case 3: Categ = "Public Transport";
			break;
        case 4: Categ = "Vehicle";
			break;
		case 5:	Categ = "Entertainment";
			break;
		case 6:	Categ = "Network";
			break;
		case 7:	Categ = "Finance Expensies";
			break;
        case 8: Categ = "Transfer operation";
            break;
        case 9: Categ = "Earnings";
            break;
		default:
			Categ = "";
			break;
	}

	std::string str;
	if (AccountNameAddit.empty())
		 str = "Operation: \'" + Oper + "\'\tCategory: \'" + Categ + "\'\tAccount Name: \'" + this->AccountName + "\' Value: " + std::to_string(this->Value);
	else
		 str = "Operation: \'" + Oper + "\'" + "\tFrom Account: \'" + this->AccountName + "\' To Account: \'" + this->AccountNameAddit + "\' Value: " + std::to_string(this->getValue());
	
	return str;

}


void wlt::Note::fillTimeData(unsigned long long seconds)
{
	time_t t;
	if (seconds == 0)
		t = time(NULL);
	else
		t = seconds;

	this->EpochTime = t;

	struct tm buf;

	_localtime64_s(&buf, &t);


	this->minutes = buf.tm_min;
	this->hours = buf.tm_hour;
	this->day = buf.tm_mday;
	this->month = buf.tm_mon + 1;
	this->year = buf.tm_year + 1900;
}

std::string wlt::Note::getTimeInfo()
{
	char buffer[25];
	sprintf_s(buffer,"%02d:%02d %02d.%02d.%d",this->hours, this->minutes, this->day, this->month, this->year);

	return std::string(buffer);
}

time_t wlt::Note::getEpochTime()
{
	return this->EpochTime;
}

std::string wlt::Note::getAccountNameAddit()
{
	return this->AccountNameAddit;
}


void wlt::Note::setAccountNameAddit(std::string& newName)
{
	this->AccountNameAddit = newName;
}

std::string wlt::Note::getDetails()
{
    return this->Details;
}

void wlt::Note::setDetails(std::string new_detail)
{
    this->Details = new_detail;
}

unsigned int wlt::Note::getDay()
{
    return this->day;
}

unsigned int wlt::Note::getMonth()
{
    return this->month;
}

unsigned int wlt::Note::getYear()
{
    return this->year;
}


std::string wlt::Note::getCategoryString()
{
    switch (this->CategoryType)
    {
    case 0: return "Food";
        break;
    case 1:return "Products";
        break;
    case 2: return "Healthcare";
        break;
    case 3: return "Public Transport";
        break;
    case 4: return "Vehicle";
        break;
    case 5:	return "Entertainment";
        break;
    case 6:	return "Network";
        break;
    case 7:	return "Finance Expensies";
        break;
    case 8: return "Transfer operation";
        break;
    case 9: return "Earnings";
        break;
    default:
        return "";
        break;
    }
}



