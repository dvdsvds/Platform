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

    // 기본 아이디와 비밀번호 값 설정
    string newID = ""; // ID를 SignUP 함수 내에서 입력받도록 설정
    string newPW = ""; // PW를 SignUP 함수 내에서 입력받도록 설정
    string newName = "";
    

    // 회원가입 함수 호출
    if (account.SignUP(db, newName, newID, newPW) != 0) {
        std::cerr << "Failed to create account." << std::endl;
    }
    else {
        std::cout << "Account created successfully." << std::endl;
    }

    sqlite3_close(db); // 데이터베이스 닫기
	
	return 0;
}