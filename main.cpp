#include "system.h"
using namespace std;

int main() {
	sqlite3* db;
	CREATE_ACCOUNT ca;

	int exit = sqlite3_open("Accounts.db", &db);
	if (exit) {
		cerr << "Error open DB : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

    CREATE_ACCOUNT account; // CREATE_ACCOUNT ��ü ����

    string command;

    while (true) {
		cout << "> ";
		getline(cin, command);
		if (command == "signin" || command == "si") {
			int result = ca.SignIN(db);
			if (result == 1) {
				cout << "Login successfully" << endl;
			}
			else {
				cout << "ID or Password is not correct" << endl;
			}
		}
			
		if (command == "signup" || command == "su") {
			if (ca.SignUP(db)) {
				if (ca.SignUP(db) == 0) {  // ȸ������ ���� ��
					cout << "Account created successfully." << endl;
				}
				else {
					cout << "Failed to create account." << endl;
				}
			}
		}
    }
    sqlite3_close(db); // �����ͺ��̽� �ݱ�
	
	return 0;
}