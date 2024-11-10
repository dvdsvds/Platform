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

    // �⺻ ���̵�� ��й�ȣ �� ����
    string newID = ""; // ID�� SignUP �Լ� ������ �Է¹޵��� ����
    string newPW = ""; // PW�� SignUP �Լ� ������ �Է¹޵��� ����
    string newName = "";
    

    // ȸ������ �Լ� ȣ��
    if (account.SignUP(db, newName, newID, newPW) != 0) {
        std::cerr << "Failed to create account." << std::endl;
    }
    else {
        std::cout << "Account created successfully." << std::endl;
    }

    sqlite3_close(db); // �����ͺ��̽� �ݱ�
	
	return 0;
}