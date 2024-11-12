#include "system.h"

using namespace std;

#define O 1
#define X 0

string CREATE_ACCOUNT::sha256(const string& str) {
	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
	const EVP_MD* md = EVP_sha256();
	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int len = 0;

	if (mdctx == nullptr) {
		cerr << "Error creating EVP context." << endl;
		return "";
	}

	if (EVP_DigestInit_ex(mdctx, md, nullptr) != 1) {
		cerr << "Error initializing digest." << endl;
		EVP_MD_CTX_free(mdctx);
		return "";
	}

	if (EVP_DigestUpdate(mdctx, str.c_str(), str.length()) != 1) {
		cerr << "Error updating digest." << endl;
		EVP_MD_CTX_free(mdctx);
		return "";
	}

	if (EVP_DigestFinal_ex(mdctx, hash, &len) != 1) {
		cerr << "Error finalizing digest." << endl;
		EVP_MD_CTX_free(mdctx);
		return "";
	}

	EVP_MD_CTX_free(mdctx);

	stringstream ss;
	for (unsigned int i = 0; i < len; i++) {
		ss << hex << (int)hash[i];
	}

	return ss.str();
	}

void CREATE_ACCOUNT::initDataBase() {
	sqlite3* db;
	char* errMsg = 0;

	int exit = sqlite3_open("Accounts.db", &db);
	if (exit) {
		cerr << "Error open DB : " << sqlite3_errmsg(db) << endl;
		return;
	}

	string sql = "CREATE TABLE IF NOT EXISTS Accounts (USERNAME TEXT PRIMARY KEY, ID TEXT, PASSWORD TEXT);";

	exit = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
	if (exit != SQLITE_OK) {
		cerr << "SQL error : " << errMsg << endl;
		sqlite3_free(errMsg);
	}

	sqlite3_close(db);


}
CREATE_ACCOUNT::CREATE_ACCOUNT() {
	initDataBase();
}

int CREATE_ACCOUNT::SignUP(sqlite3* db) {
	sqlite3_stmt* stmt;

	cout << "type your name : ";
	getline(cin, NEWUSERNAME);
	cout << "ID : ";
	getline(cin, NEWID);
	cout << "PW : ";
	getline(cin, NEWPW);

	if (checkName(db, NEWUSERNAME) == 1) {
		cout << "USERNAME already exist." << endl;
		return 1;
	}

	if (checkID(db, NEWID) == 1) {
		cout << "ID already exist." << endl;
		return 1;
	}

	string hashed_password = sha256(NEWPW);

	const char* sql = "INSERT INTO Accounts (USERNAME, ID, PASSWORD) VALUES (?, ?, ?);";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
		cout << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	sqlite3_bind_text(stmt, 1, NEWUSERNAME.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, NEWID.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, hashed_password.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		cout << "Error inserting data : " << sqlite3_errmsg(db) << endl;
		sqlite3_finalize(stmt);
		return 1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

int CREATE_ACCOUNT::SignIN(sqlite3* db) {

	sqlite3_stmt* stmt;

	cout << "ID : ";
	getline(cin, ID);
	cout << "PW : ";
	getline(cin, PW);

	if (checkID(db, ID) == 0) {
		cout << "ID does not exist." << endl;
		return 1;
	}

	string hashedPW = sha256(PW); const char* sql = "SELECT COUNT(*) FROM Accounts WHERE ID = ? AND PASSWORD = ?;";
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}
	
	sqlite3_bind_text(stmt, 1, ID.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, hashedPW.c_str(), -1, SQLITE_STATIC);

	int result = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		if (count > 0) {
			result = 1; // 로그인 성공
		}
		else {
			cout << "PW is not correct" << endl;
			result = 0;
		}
	}
	else {
		cout << "Error executing statement : " << sqlite3_errmsg(db) << endl;
	}

	sqlite3_finalize(stmt);
	return result;
}

int CREATE_ACCOUNT::checkName(sqlite3* db, const string& name) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT COUNT(*) FROM Accounts WHERE USERNAME = ?;";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

	int duplicate = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		if (count > 0) {
			duplicate = 1;
		}
		else {
			cerr << "Error executing statement : " << sqlite3_errmsg(db) << endl;
			duplicate = -1;
		}

	}
	sqlite3_finalize(stmt);
	return duplicate;
} 

int CREATE_ACCOUNT::checkID(sqlite3* db, const string& ID) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT COUNT(*) FROM Accounts WHERE ID = ?;";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	sqlite3_bind_text(stmt, 1, ID.c_str(), -1, SQLITE_STATIC);

	int duplicate = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		duplicate = (count > 0) ? 1 : 0; // 아이디 존재 - 1 / 존재하지 않으면 0
	}
	else {
		cerr << "Error executing statement : " << sqlite3_errmsg(db) << endl;
		duplicate = -1;
	
	}
	sqlite3_finalize(stmt);
	return duplicate;
}

int CREATE_ACCOUNT::checkPW(sqlite3* db, const string& PW) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT COUNT(*) FROM Accounts WHERE PASSWORD = ?;";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
		cerr << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1; // id does not exist - return 1
	}

	sqlite3_bind_text(stmt, 1, PW.c_str(), -1, SQLITE_STATIC);

	int duplicate = 0;
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		int count = sqlite3_column_int(stmt, 0);
		duplicate = (count > 0) ? 1 : 0;
	}
	else {
		cerr << "Error executing statement : " << sqlite3_errmsg(db) << endl;
		duplicate = -1;
	
	}
	sqlite3_finalize(stmt);
	return duplicate;
}
