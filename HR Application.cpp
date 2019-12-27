#define _CRT_SECURE_NO_WARNINGS
#include <mysql.h>
#include <iostream>
#include <string>
using namespace std;

struct Employee {
	int employeeNumber;
	char lastName[50];
	char firstName[50];
	char email[100];
	char phone[50];
	char extension[10];
	char reportsTo[100];
	char jobTitle[50];
	char city[50];
};

int readEmployeeNumber() {
	int valid = false;
	int employNumber;
	cout << "EmployNumber: ";
	do {
		cin >> employNumber;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(2000, '\n');
			cout << "Invalid input, enter EmployeeNumber: ";
		}
		else {
			valid = true;
		}
	} while (!valid);
	return employNumber;
}

int menu() {
	int option;
	bool valid = false;
	cout << "*********************HR Menu*********************" << endl;
	cout << "1) Find Employee\n2) Employees Reprot\n3) Add Employee\n4) Update Employee\n5) Remove Emplouee\n0) Exit" << endl;
	do {
		cout << "< ";
		cin >> option;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(2000, '\n');
			cout << "Invalid input, enter 0 - 5: ";
		}
		else if (option < 0 || option > 5) {
			cout << "Invalid input, enter 0 - 5: ";
		}
		else {
			valid = true;
		}
	} while (!valid);
	return option;
}

int findEmployee(MYSQL* conn, int employeeNumber, struct Employee* emp) {
	MYSQL_RES* res;
	MYSQL_ROW row;
	int exequery;
	int result = 0;
	string query = "select distinct e1.employeeNumber, e1.lastName, "\
		"e1.firstName, e1.email, phone, e1.extension, "\
		"coalesce(concat(e2.firstName, ' ', e2.lastname), ' ') as reportsTo, "\
		"e1.jobTitle, city from employees as e1 join offices as o on e1.officeCode = o.officeCode "\
		"left join employees as e2 on e1.reportsTo = e2.employeeNumber "\
		"where e1.employeeNumber = ";
	query.append(to_string(employeeNumber));
	query.append(";");
	const char* q = query.c_str();
	exequery = mysql_query(conn, q);
	if (!exequery) {
		res = mysql_store_result(conn);
		if (res == nullptr) {
			result = 0;
		}
		while (row = mysql_fetch_row(res)) {
			emp->employeeNumber = stoi(row[0]);
			strcpy(emp->lastName, row[1]);
			strcpy(emp->firstName, row[2]);
			strcpy(emp->email, row[3]);
			strcpy(emp->phone, row[4]);
			strcpy(emp->extension, row[5]);
			strcpy(emp->reportsTo, row[6]);
			strcpy(emp->jobTitle, row[7]);
			strcpy(emp->city, row[8]);
			result = 1;
		}
	}
	else {
		cout << "Error message: " << mysql_error(conn) << ":" << mysql_errno(conn) << endl;
	}

	return result;
}

void displayEmployee(MYSQL* conn, struct Employee emp) {
	int employNumber;
	employNumber = readEmployeeNumber();
	if (findEmployee(conn, employNumber, &emp)) {
		cout << "EmployeeNumber = " << emp.employeeNumber << "\n";
		cout << "LastName = " << emp.lastName << "\n";
		cout << "FirstName = " << emp.firstName << "\n";
		cout << "Email = " << emp.email << "\n";
		cout << "Phone = " << emp.phone << "\n";
		cout << "Extension = " << emp.extension << "\n"; 
		cout << "ReportsTo = " << emp.reportsTo << "\n";
		cout << "JobTitile = " << emp.jobTitle << "\n";
		cout << "City = " << emp.city << endl;
	}
	else {
		cout << "Employee " << employNumber << " dose not exit." << endl;
	}

}

void displayAllEmployees(MYSQL* conn) {
	MYSQL_RES* res;
	MYSQL_ROW row;
	int exequery;
	string query = "select distinct e1.employeeNumber, concat(e1.firstName, ' ', e1.lastName), "\
		"e1.email, phone, e1.extension, coalesce(concat(e2.firstName, ' ', e2.lastname), ' ') as reportsTo "\
		"from employees as e1 join offices as o on e1.officeCode = o.officeCode "\
		"left join employees as e2 on e1.reportsTo = e2.employeeNumber;";
	const char* q = query.c_str();
	exequery = mysql_query(conn, q);
	if (!exequery) {
		res = mysql_store_result(conn);
		if (res == nullptr) {
			cout << "There is no employees' information to be displayed." << endl;
		}
		else {
			cout << "E      EmployeeName         Email                                    Phone                Ext       Manager" << endl;
			cout << "---------------------------------------------------------------------------------------------------------------------" << endl;
		}
		while (row = mysql_fetch_row(res)) {
			printf("%-6s %-20s %-40s %-20s %-10s %-20s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
		}
	}
	else {
		cout << "Error message: " << mysql_error(conn) << ":" << mysql_errno(conn) << endl;
	}

}

void insertEmployee(MYSQL* conn, struct Employee emp) {
	int employNumber;
	employNumber = readEmployeeNumber();
	if (findEmployee(conn, employNumber, &emp)) {
		cout << "An employee with the same employee number exists." << endl;
	}
	else {
		emp.employeeNumber = employNumber;
		cout << "Last Name: ";
		cin.ignore();
		cin.getline(emp.lastName, 50);
		cout << "First Name: ";
		cin.getline(emp.firstName, 50);
		cout << "Email: ";
		cin.getline(emp.email, 100);
		cout << "extension: ";
		cin.getline(emp.extension, 10);
		cout << "Job Title: ";
		cin.getline(emp.jobTitle, 50);
		int exequery;
		string query = "insert into employees values (" + to_string(emp.employeeNumber) + "," + "\"" + emp.lastName
			+ "\"" + "," + "\"" + emp.firstName + "\"" + "," + "\"" + emp.extension + "\"" + "," + "\""
			+ emp.email + "\"" + ", \"1\",1002,\"" + emp.jobTitle + "\");";
		const char* q = query.c_str();
		exequery = mysql_query(conn, q);
		if (!exequery) {
			cout << "The new employee is added successfully." << endl;
		}
		else {
			cout << "Error message: " << mysql_error(conn) << ":" << mysql_errno(conn) << endl;
		}
		
	}
}

void updateEmployee(MYSQL* conn, int employeeNumber) {
	Employee emp = {};
	if (findEmployee(conn, employeeNumber, &emp)) {
		cin.ignore();
		cout << "New Extension: ";
		cin.getline(emp.extension, 10);
		int exequery;
		string query = "update employees set extension = \"";
		query.append(emp.extension);
		query.append("\" where employeeNumber = " + to_string(employeeNumber) + ";");
		const char* q = query.c_str();
		exequery = mysql_query(conn, q);
		if (!exequery) {
			cout << "Extension updated!" << endl;
		}
		else {
			cout << "Error message: " << mysql_error(conn) << ":" << mysql_errno(conn) << endl;
		}
	}
	else {
		cout << "Employee " << employeeNumber << " dose not exit." << endl;
	}
}

void deleteEmployee(MYSQL* conn, int employeeNumber) {
	Employee emp = {};
	if (findEmployee(conn, employeeNumber, &emp)) {
		int exequery;
		string query = "delete from employees where employeeNumber = " + to_string(employeeNumber) + ";";
		const char* q = query.c_str();
		exequery = mysql_query(conn, q);
		if (!exequery) {
			cout << "The employee is deleted." << endl;
		}
		else {
			cout << "Error message: " << mysql_error(conn) << ":" << mysql_errno(conn) << endl;
		}
	}
	else {
		cout << "The employee does not exist." << endl;
	}
}


int main(void) {
	int employeeNumber;
	Employee emp = {};
	MYSQL* conn;
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "mymysql.senecacollege.ca", "db_mnuhaque", "|password here|", "db_mnuhaque", 3306, nullptr, 0);
	if (conn) {
		int option;
		do {
			option = menu();
			switch (option) {
			case 1:
				displayEmployee(conn, emp);
				break;
			case 2:
				displayAllEmployees(conn);
				break;
			case 3:
				insertEmployee(conn, emp);
				break;
			case 4:
				employeeNumber = readEmployeeNumber();
				updateEmployee(conn, employeeNumber);
				break;
			case 5:
				employeeNumber = readEmployeeNumber();
				deleteEmployee(conn, employeeNumber);
				break;
			case 0:
				cout << "Bye!" << endl;
				break;
			default:
				cout << "Invalid Input" << endl;
			}
		} while (option != 0);
	}
	else {
		cout << "Connection Failed" << mysql_error(conn) << endl;
	}
	mysql_close(conn);
	return 0;
}