/*
Waitman Gobble
waitman@waitman.net
gcc -o wma2010 wma2010.c -I/usr/local/include -L/usr/local/lib -lfcgi -lsqlite3 -ljson

usage: http://www.emkdesign.com/bin/wma2010?age=AGE&key=KEY&sex=male|female
example http://www.emkdesign.com/bin/wma2010?age=40&key=c1&sex=male

returns json object

Idea: cmod 444 wma2010.db
*/
#include <fcgi_stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>
#include <sqlite3.h>


#define ToHex(Y) (Y>='0'&&Y<='9'?Y-'0':Y-'A'+10)

char InputData[4096];
json_object * jobj;

void getAllParams() {
	if( getenv( "REQUEST_METHOD" ) == 0 ) {
		printf("No REQUEST_METHOD, must be running in DOS mode");
		return;
	} else if (strcmp( getenv("REQUEST_METHOD"), "POST") == 0) {
		char *endptr;
		char *len1 = getenv("CONTENT_LENGTH");
		int contentlength = strtol(len1, &endptr, 10);
		fread(InputData , contentlength, 1, stdin);
	} else {
		strcpy(InputData, getenv("QUERY_STRING"));
	}
}

void getParam(const char *Name, char *Value) {
	char *pos1 = strstr(InputData, Name);
	if (pos1) {
		pos1 += strlen(Name);
		if (*pos1 == '=') { 
			pos1++;
			while (*pos1 && *pos1 != '&') {
				if (*pos1 == '%') { 
					*Value++ = (char)ToHex(pos1[1]) * 16 + ToHex(pos1[2]);
					pos1 += 3;
				} else if( *pos1=='+' ) { 
					*Value++ = ' ';
					pos1++;
				} else {
					*Value++ = *pos1++; 
				}
			}
			*Value++ = '\0';
			return;
		}
	}
	strcpy(Value, "undefine"); 
	return;
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
  int i;
  NotUsed=0;


  for(i=0; i<argc; i++){
	json_object *jstring = json_object_new_string(argv[i] ? argv[i] : "NULL");
	json_object_object_add(jobj,azColName[i],jstring);
  }
  return 0;
}

static int callback_final (void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	NotUsed=0;
	for(i=0; i<argc; i++){
       		json_object *jstring = json_object_new_string(argv[i] ? argv[i] : "NULL");

 	      	 json_object_object_add(jobj,azColName[i],jstring);
  	}
	printf ("%s\n",json_object_to_json_string(jobj));
	jobj = json_object_new_object();
	return 0;
}
int main( int argc, char *argv[] )
{

jobj = json_object_new_object();
char Age[3] = "";
char Key[3] = "";
char Sex[5] = "";
char Distance[10] = "";


sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

   while( FCGI_Accept() >= 0 ) {
      printf( "Content-Type: text/plain\n\n" );
	getAllParams();
	getParam("age", Age);
	getParam("key", Key);
	getParam("sex", Sex);
	getParam("distace", Distance);





rc = sqlite3_open("wma2010.db", &db);
  if( rc ){
    printf("Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
  }

  char sql [128];

char table [7];
if (Key[0]=='c') {
	strcpy(table,"_road");
} else {
	strcpy(table,"_track");
}


	sprintf(sql,"SELECT %s FROM %s%s WHERE age=%s",Key,Sex,table,Age);

  rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

  if( rc!=SQLITE_OK ){
    printf("SQL error: %s\n", zErrMsg);
  }

	sprintf(sql,"SELECT * FROM %s_key WHERE keycode='%s'",Sex,Key);

  rc = sqlite3_exec(db, sql, callback_final, 0, &zErrMsg);

  if( rc!=SQLITE_OK ){
    printf("SQL error: %s\n", zErrMsg);
  }
  sqlite3_close(db);

   }
   return 0;
}
