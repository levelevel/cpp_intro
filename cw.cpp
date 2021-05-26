#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <new>
#include <string>
using namespace std;

#define MAX_WORD_SIZE 256

void oops(const char* msg) {
    cerr << msg << endl;
    exit(1);
}

void out_of_mem(void) {
    cerr << "Out of memory\n";
    abort();
}

///////////////////////////////////////////////////////////////////////////////
// String: wrapper class for array of characters
///////////////////////////////////////////////////////////////////////////////
class String {
public:
    String();                       //default construcutor
    ~String();                      //destructor
    void operator=(const String &s);//assignment operator
    String(const String &s);        //copy constructor
    String(const char *s);          //copy constructor

    void set(const char *s);        //文字列を設定する
    const char *s();                //文字列を返す
    char c(size_t pos);             //posの位置の文字を返す
    size_t length();                //文字列の長さを返す

    void print(ostream *os);        //print
    bool read(FILE *ifp);           //ファイルから文字列を読み込む

private:
    char *str;
    size_t len;
};

//default construcutor
String::String() {
    str = 0;
    set("");
}

//destructor
String::~String() {
    delete[] str;
}

//assignment operator
void String::operator=(const String &s) {
    if (this==&s) return;       // 自分自身には代入しない
    set(s.str);
}

//copy constructor
String::String(const String &s) {
    str = 0;
    set(s.str);
}
String::String(const char *s) {
    str = 0;
    set(s);
}

//文字列を設定する
void String::set(const char *s) {
    delete[] str;
    len = strlen(s);
    str = new char[len+1];
    strcpy(str, s);
}

//文字列を返す
const char *String::s() {
    return str;
}

//posの位置の文字を返す
char String::c(size_t pos) {
    if (pos>=len) oops("string index error");
    return str[pos];
}

//文字列の長さを返す
size_t String::length() {
    return len;
}

//print
void String::print(ostream *os) {
    *os << str;
}

ostream &operator<<(ostream &os, String &s) {
    s.print(&os);
    return os;
}

bool String::read(FILE *ifp) {
    char buf[MAX_WORD_SIZE];
    clearerr(ifp);
    fgets(buf, MAX_WORD_SIZE, ifp);
    if (feof(ifp)) return false;
    if (ferror(ifp)) oops("file read error");
    int len = strlen(buf);
    if (buf[len]=='\n') buf[len] = '\0';
    set(buf);
    return true;
}
//ファイルから文字列を読み込む

int main(int argc, char* argv[]) {
    set_new_handler(out_of_mem);
    ios::sync_with_stdio();
    String s = "abc";
    cout << s << endl;
}
