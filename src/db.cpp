#include "db.h"
#include <sqlite3.h>
#include <sstream>
#include <list>

namespace db {
static void insert_ref_value_core(void);
static void insert_decl_value_core(void);
static void insert_overriden_value_core(void);

static sqlite3 *db;
static std::list<std::string > insert_list_ref;
static std::list<std::string > insert_list_decl;
static std::list<std::string > insert_list_overriden;

void init(std::string file_name)
{
    char *err=NULL;
    remove(file_name.c_str());
    if(sqlite3_open(file_name.c_str(), &db ) != SQLITE_OK) {
        printf("ERROR: failt to open db");
        exit(1);
    }
    // begin transaction
    sqlite3_exec(db, "begin transaction;", NULL, NULL, NULL);
    // db_info
    sqlite3_exec(db, "create table db_info(db_format integer);", NULL, NULL, &err);
    // ref
    sqlite3_exec(db, "create table ref(usr text,name text, file_name text, line integer, col integer, kind text, ref_file_name text, ref_line integer, ref_col integer);", NULL, NULL, &err);
    // decl
    sqlite3_exec(db, "create table decl(usr text,name text, file_name text, line integer, col integer, kind text, val integer, is_virtual integer, is_def integer);", NULL, NULL, &err);
    // overriden
    sqlite3_exec(db, "create table overriden(usr text,name text, file_name text, line integer, col integer, kind text, usr_overrider text, is_def integer);", NULL, NULL, &err);

    std::ostringstream os;
    os << "insert into db_info values(" << DB_VER << ");";
    sqlite3_exec(db, os.str().c_str(), NULL, NULL, &err);
}

void insert_ref_value(std::string usr, std::string name, std::string file_name, int32_t line, int32_t col, std::string kind, std::string ref_file_name, int ref_line, int ref_col)
{
    std::ostringstream os;
    os << "('" << usr << "','" << name << "', '" << file_name
        << "', " << line << ", " << col << ", '" << kind << "', '" << ref_file_name
        << "', " << ref_line << ", " << ref_col << ")";
    insert_list_ref.push_back(os.str());
    if(insert_list_ref.size() == INSERT_LIST_MAX) {
        insert_ref_value_core();
        insert_list_ref.clear();
    }
}

static void insert_ref_value_core(void)
{
    char *err=NULL;
    std::ostringstream os;
    std::ostringstream os_values;
    for(std::list<std::string >::iterator itr = insert_list_ref.begin();
        itr != insert_list_ref.end();
        itr++) {
        os << "insert into ref values " << *itr << ";";
        //printf("INSERT_REF: %s\n", values.c_str());
    }
    sqlite3_exec(db, os.str().c_str(), NULL, NULL, &err);
    if(err != SQLITE_OK) {
        printf("ERROR: SQLITE3: %s\n", sqlite3_errmsg(db));
    }
}

void insert_decl_value(std::string usr, std::string name, std::string file_name, int32_t line, int32_t col, std::string entity_kind, int val, int is_virtual, int is_def)
{
    std::ostringstream os;
    os << "('" << usr << "','" << name << "', '" << file_name
        << "', " << line << ", " << col << ", '" << entity_kind << "', " << val << ", "
        << is_virtual << ", " << is_def << ")";
    insert_list_decl.push_back(os.str().c_str());
    if(insert_list_decl.size() == INSERT_LIST_MAX) {
        insert_decl_value_core();
        insert_list_decl.clear();
    }
}

static void insert_decl_value_core(void)
{
    char *err=NULL;
    std::ostringstream os;
    std::ostringstream os_values;
    for(std::list<std::string >::iterator itr = insert_list_decl.begin();
        itr != insert_list_decl.end();
        itr++) {
        os << "insert into decl values " << *itr << ";";
    }
    //printf("INSERT_DECL: %s\n", values.c_str());
    sqlite3_exec(db, os.str().c_str(), NULL, NULL, &err);
    if(err != SQLITE_OK) {
        printf("ERROR: SQLITE3: %s\n", sqlite3_errmsg(db));
    }
}

void insert_overriden_value(std::string usr, std::string name, std::string file_name, int32_t line, int32_t col, std::string entity_kind, std::string usr_overrider, int is_def)
{
    std::ostringstream os;
    os << "('" << usr << "','" << name << "', '" << file_name
        << "', " << line << ", " << col << ", '" << entity_kind << "', '" << usr_overrider 
        << "', " << is_def << ")";
    insert_list_overriden.push_back(os.str().c_str());
    if(insert_list_overriden.size() == INSERT_LIST_MAX) {
        insert_overriden_value_core();
        insert_list_overriden.clear();
    }
}

static void insert_overriden_value_core(void)
{
    char *err=NULL;
    std::ostringstream os;
    std::ostringstream os_values;
    for(std::list<std::string >::iterator itr = insert_list_overriden.begin();
        itr != insert_list_overriden.end();
        itr++) {
        os << "insert into overriden values " << *itr << ";";
    }
    //printf("INSERT_OVERRIDEN: %s\n", values.c_str());
    sqlite3_exec(db, os.str().c_str(), NULL, NULL, &err);
    if(err != SQLITE_OK) {
        printf("ERROR: SQLITE3: %s\n", sqlite3_errmsg(db));
    }
}

void fin(void)
{
    // flush buffers
    if(insert_list_ref.size()) {
        insert_ref_value_core();
    }
    if(insert_list_decl.size()) {
        insert_decl_value_core();
    }
    if(insert_list_overriden.size()) {
        insert_overriden_value_core();
    }
    // end transaction
    sqlite3_exec(db, "end transaction;", NULL, NULL, NULL);
    if(SQLITE_OK != sqlite3_close(db)) {
        fprintf(stderr, "ERROR: db couldn't close\n");
        fprintf(stderr, "%s\n", sqlite3_errmsg(db));
        exit(1);
    }
}
};
