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

    CREATE_ACCOUNT account; // CREATE_ACCOUNT 객체 생성

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
				if (ca.SignUP(db) == 0) {  // 회원가입 성공 시
					cout << "Account created successfully." << endl;
				}
				else {
					cout << "Failed to create account." << endl;
				}
			}
		}
    }
    sqlite3_close(db); // 데이터베이스 닫기
	
	return 0;
}