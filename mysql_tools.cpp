#include "include/mysql.h"
#include <iostream>
#include <fstream>
using namespace std;

const int SQL_MAX = 1000;
string sql_password = "123456";
string sql_database_name = "FEATURESTRING";
string sql_table_name = "strings";

string sub_str_1 = " ";
string new_str_1 = "%20";
string sub_str_2 = "|";
string new_str_2 = "%7C";

void subreplace(string &resource_str, string sub_str, string new_str)
{
    string dst_str = resource_str;
    string::size_type pos = 0;
    while((pos = dst_str.find(sub_str)) != std::string::npos)   //替换所有指定子串
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    resource_str = dst_str;
}

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

bool addTableData(char *name, char *str, char* p1, char*p2) {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "INSERT INTO %s VALUES('%s', '%s', '%s', '%s');", sql_table_name.data(), name, str, p1, p2);
	printf("sql insert command: %s\n", sql);		
	int ret = mysql_real_query(&mysql, sql, (unsigned long)strlen(sql));		
	printf("running sql insert command, result: %d\n", ret);
	if (ret) {
		printf("insert failed, error:%s\n", mysql_error(&mysql));
		mysql_close(&mysql);
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

bool altTableData(char *name, char *str,  char* p1, char*p2) {
	MYSQL mysql;
	char sql[SQL_MAX];
	if (!connectDB(mysql)) {
		return false;
	}
	snprintf(sql, SQL_MAX, "UPDATE %s SET string = '%s', port_source = '%s', port_destination = '%s' WHERE name = '%s';",sql_table_name.data(), str, p1, p2, name);
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
	if(select_all)snprintf(sql, SQL_MAX, "SELECT name, string, port_source, port_destination FROM %s;", sql_table_name.data());
	else if(select_name)snprintf(sql, SQL_MAX, "SELECT name, string, port_source, port_destination FROM %s where name = '%s';", sql_table_name.data(), name);
	else if(select_string)snprintf(sql, SQL_MAX, "SELECT name, string, port_source, port_destination FROM %s where string = '%s';", sql_table_name.data(), str);
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
		printf("%s	", row[1]);	
		printf("%s	", row[2]);
		printf("%s\n", row[3]);
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
	cout<<"i <string> <string> <port1> <port2>: insert a string"<<endl;
	cout<<"d <string>: delete a string"<<endl;
	cout<<"u <string> <string> <port1> <port2>: update a string"<<endl;
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
			string name_str, string_str, port_source_str="", port_dest_str="";
        	int pos = tempstr.find('#');
			name_str = tempstr.substr(0, pos);
			tempstr.erase(0, pos + 1);
			pos = tempstr.find('#');
			if(pos<=9);
			else if(!strcmp(tempstr.substr(0,9).data(),"$HOME_NET"))
				port_source_str = tempstr.substr(10,pos-10);
			else if(!strcmp(tempstr.substr(0,13).data(),"$EXTERNAL_NET"))
				port_dest_str = tempstr.substr(14,pos-14);
			tempstr.erase(0, pos+1);
			pos = tempstr.find('#');
			if(pos<=9);
			else if(!strcmp(tempstr.substr(0,9).data(),"$HOME_NET"))
				port_source_str = tempstr.substr(10,pos-10);
			else if(!strcmp(tempstr.substr(0,13).data(),"$EXTERNAL_NET"))
				port_dest_str = tempstr.substr(14,pos-14);
			tempstr.erase(0, pos+1);
			string_str = tempstr;
			subreplace(string_str,sub_str_1,new_str_1);
			subreplace(string_str,sub_str_2,new_str_2);
			if(!strcmp(port_source_str.data(),""))port_source_str = "any";
			if(!strcmp(port_dest_str.data(),""))port_dest_str = "any";
			addTableData((char *)name_str.data(),(char *)string_str.data(),(char *)port_source_str.data(),(char *)port_dest_str.data());
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
	string p1,p2;
	help();
	do
	{
		cin>>c;
		if(c=='h')help();
		else if(c=='p')queTableData(true,false,false);
		else if(c=='i'){
			cin>>name>>str>>p1>>p2;
			subreplace(str,sub_str_1,new_str_1);
			subreplace(str,sub_str_2,new_str_2);
			addTableData((char *)name.data(),(char *)str.data(),(char *)p1.data(),(char *)p2.data());
			}
		else if(c=='d'){cin>>name;delTableData((char *)name.data());}
		else if(c=='u'){
			cin>>name>>str>>p1>>p2;
			subreplace(str,sub_str_1,new_str_1);
			subreplace(str,sub_str_2,new_str_2);
			altTableData((char *)name.data(),(char *)str.data(),(char *)p1.data(),(char *)p2.data());
			}
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