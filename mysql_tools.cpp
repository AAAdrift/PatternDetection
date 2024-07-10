#include "include/mysql.h"
#include <iostream>
#include <fstream>
using namespace std;

const int SQL_MAX = 100;
string sql_password = "123456";
string sql_database_name = "FEATURESTRING";
string sql_table_name = "strings";

bool connectDB(MYSQL &mysql) {
	mysql_init(&mysql);
	mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");
	MYSQL *ret = mysql_real_connect(&mysql,"localhost","root",sql_password.data(),sql_database_name.data(),3306,NULL,0);
	if (ret == NULL) {
		printf("database connection fail, error:%s\n", mysql_error(&mysql));
		return false;
	}
	printf("database connection succeed\n");
	int res = mysql_select_db(&mysql, sql_database_name.data());
	if (res) {
		printf("database selection fail, error:%s\n", mysql_error(&mysql));
		return false;
	}
	printf("database selection succeed\n");
	return true;
}

bool addTableData(char *name, char *str) {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "INSERT INTO %s VALUES('%s', '%s');", sql_table_name.data(), name, str);
	printf("sql insert command: %s\n", sql);		
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));		
	printf("running sql insert command, result: %d\n", ret);
	if (ret) {
		printf("insert failed, error:%s\n", mysql_error(&mysql));
		return false;
	}
	printf("insert succeed\n");
	mysql_close(&mysql);
	return true;
}

bool delTableData(char *name) {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "DELETE FROM %s WHERE name = '%s';", sql_table_name.data(),name);
	printf("sql delete command: %s\n", sql);
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));
	printf("running sql delete command, result: %d\n", ret);
	if (ret) {
		printf("delete failed, error: %s\n", mysql_error(&mysql));
		return false;

	}
	printf("delete succeed\n");
	mysql_close(&mysql);
	return true;
}

bool altTableData(char *name, char *str) {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "UPDATE %s SET string = '%s' WHERE name = '%s';",sql_table_name.data(), str, name);
	printf("update sql command: %s\n", sql);
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));
	printf("running update sql command, result: %d\n", ret);
	if (ret) {
		printf("update failed,error: %s\n", mysql_error(&mysql));
		return false;
	}
	printf("update succeed\n");
	mysql_close(&mysql);
	return true;
}

bool queTableData(bool select_all, bool select_name, bool select_string, char *name = (char *)"", char *str = (char *)"") {
	MYSQL mysql;
	MYSQL_RES* res;
	MYSQL_ROW row;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	if(select_all)snprintf(sql, SQL_MAX, "SELECT name, string FROM %s;", sql_table_name.data());
	else if(select_name)snprintf(sql, SQL_MAX, "SELECT name, string FROM %s where name = '%s';", sql_table_name.data(), name);
	else if(select_string)snprintf(sql, SQL_MAX, "SELECT name, string FROM %s where string = '%s';", sql_table_name.data(), str);
	else return false;
	printf("query sql command: %s\n", sql);
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));
	printf("running query sql command, result: %d\n", ret);

	if (ret) {
		printf("query failed, error: %s\n", mysql_error(&mysql));
		return false;
	}
	printf("query succeed\n");
	res = mysql_store_result(&mysql);
	while (row = mysql_fetch_row(res)) {
		printf("%s  ", row[0]);
		printf("%s\n", row[1]);	
	}
	mysql_free_result(res);
	mysql_close(&mysql);
	return true;
}

bool clearTableData() {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "truncate table %s", sql_table_name.data());
	printf("sql clear command: %s\n", sql);
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));
	printf("running sql clear command, result: %d\n", ret);
	if (ret) {
		printf("clear failed, error: %s\n", mysql_error(&mysql));
		return false;

	}
	printf("clear succeed\n");
	mysql_close(&mysql);
	return true;
}

void help()
{
	cout<<"h: help"<<endl;
	cout<<"p: print table"<<endl;
	cout<<"i <string> <string>: insert a string"<<endl;
	cout<<"d <string>: delete a string"<<endl;
	cout<<"u <string> <string>: update a string"<<endl;
	cout<<"q <int> <string>:"<<endl<<"first int:type(0:print all 1:query with name 2:query with string)"<<endl;
	cout<<"query data with name or string"<<endl;
	cout<<"f <filename>: insert strings in a file"<<endl;
	cout<<"c: clear table"<<endl;
	cout<<"z: quit"<<endl;
}

void read_file(string filename)
{
	ifstream file(filename);
	string tempstr;
    if (!file) {
		cout << "Error opening file"<<endl;
    }
	else{
		while (getline(file,tempstr)) {
        	cout<<tempstr<<endl;
			int len = tempstr.find('#');
			int len_str = tempstr.size();
			string str1 = tempstr.substr(0,len);
			string str2 = tempstr.substr(len+1,len_str-len-1);
			addTableData((char *)str1.data(),(char *)str2.data());
    	}
	}
	file.close();
}

int main()
{
	char c;
	int x;
	string name;
	string str;
	string filename;
	help();
	do
	{
		cin>>c;
		if(c=='h')help();
		else if(c=='p')queTableData(true,false,false);
		else if(c=='i'){cin>>name>>str;addTableData((char *)name.data(),(char *)str.data());}
		else if(c=='d'){cin>>name;delTableData((char *)name.data());}
		else if(c=='u'){cin>>name>>str;altTableData((char *)name.data(),(char *)str.data());}
		else if(c=='q'){
			cin>>x;
			if(x==0)queTableData(true,false,false);
			else if(x==1){cin>>name;queTableData(false,true,false,(char *)name.data());}
			else if(x==2){cin>>str;queTableData(false,false,true,(char *)name.data(),(char *)str.data());}}
		else if(c=='f'){
			cin>>filename;
			read_file(filename);
		}
		else if(c=='c'){
			clearTableData();
		}
		else if(c!='z'){cout<<"wrong input, input h to see help"<<endl;}
	} while (c!='z');
}