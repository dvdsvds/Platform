#include "system.h"

using namespace std;

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

int CREATE_ACCOUNT::SignUP(sqlite3* db, const string& newName, const string& newID, const string& newPW) {
	sqlite3_stmt* stmt;
	string hashed_password = sha256(newPW);
	string ID = newID, PW = newPW, UserName = newName;

	cout << "type your name : ";
	getline(cin, UserName);
	cout << "ID : ";
	getline(cin, ID);
	cout << "PW : ";
	getline(cin, PW);


	const char* sql = "INSERT INTO Accounts (USERNAME, ID, PASSWORD) VALUES (?, ?, ?);";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
		cout << "Error preparing statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	sqlite3_bind_text(stmt, 1, UserName.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, ID.c_str(), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, hashed_password.c_str(), -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) != SQLITE_DONE) {
		cout << "Error inserting data : " << sqlite3_errmsg(db) << endl;
		sqlite3_finalize(stmt);
		return 1;
	}

	sqlite3_finalize(stmt);
	return 0;
}

int CREATE_ACCOUNT::show(sqlite3* db, const string& ID) {
	sqlite3_stmt* stmt;
	const char* sql = "SELECT USERNAME, ID, PASSWORD FROM Accounts WHERE ID = ?;";

	if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
		cout << "Error preparing select statement : " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	sqlite3_bind_text(stmt, 1, ID.c_str(), -1, SQLITE_STATIC);  // ID로 검색

	if (sqlite3_step(stmt) == SQLITE_ROW) {
		const char* username = (const char*)sqlite3_column_text(stmt, 0);  // USERNAME 값
		const char* id = (const char*)sqlite3_column_text(stmt, 1);        // ID 값
		const char* password = (const char*)sqlite3_column_text(stmt, 2);  // PASSWORD 값 (해시된 비밀번호)

		cout << "Username: " << username << endl;
		cout << "ID: " << id << endl;
		cout << "Password (hashed): " << password << endl;  // 해시된 비밀번호 출력
	}
	else {
		cout << "No account found with the given ID." << endl;
	}

	sqlite3_finalize(stmt);
	return 0;
}
