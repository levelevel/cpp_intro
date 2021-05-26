///////////////////////////////////////////////////////////////////////////////
// Crossword Puzzle Helper
//
// From Chapter 15 of:
// "C++: The Core Language" by Gregory Satir and Doug Brown.
// O'Reilly & Associates, Inc. Sebastopol, CA. 1995.
///////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <new>
#include <string>
using namespace std;

const int MAX_WORD_SIZE = 256;

// error exit
void oops(const char* msg) {
    cerr << msg << endl;
    exit(1);
}

// called by new if insufficient memory
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
    const char *s() const;          //文字列を返す
    char c(size_t pos) const;       //posの位置の文字を返す
    char operator[](size_t pos) const;
    size_t length() const;          //文字列の長さを返す

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
const char *String::s() const {
    return str;
}

//posの位置の文字を返す
char String::c(size_t pos) const {
    if (pos>=len) oops("string index error");
    return str[pos];
}
char String::operator[](size_t pos) const {
    return c(pos);
}

//文字列の長さを返す
size_t String::length() const {
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

//ファイルから文字列を読み込む
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

///////////////////////////////////////////////////////////////////////////////
// Rule: class for rules that decide if a word matches some criterion.
// This class defines the interface for the acceptance rule.
// It has no default for accepts() so all the rules that can be instantiated
// are derived from this.
///////////////////////////////////////////////////////////////////////////////
class Rule {
public:
    //単語を検査し、基準に合っていれば真を返す。
    virtual bool accept(String word) const = 0;
};

///////////////////////////////////////////////////////////////////////////////
// CwRule: class for crossword puzzle rules
///////////////////////////////////////////////////////////////////////////////
class CwRule: public Rule {
public:
    bool accepts(String &dword) const;
    void set(String &cmd);
private:
    String cword;
};

// Test a word to see if it matches a crossword puzzle rule.
// The rule is stored as a String with ?'s where any character
// can match, and the rest of the characters must match exactly.
bool CwRule::accepts(String &dword) const {
    int len = dword.length();
    if (len!=(int)cword.length()) return false;
    for (int i=0; i<len; i++) {
        char c = cword[i];
        if (c!='?' && c!=dword[i]) return false;
    }
    return true;
}

// set crossword puzzle rule
void CwRule::set(String &cmd) {
    cword = cmd;
}

///////////////////////////////////////////////////////////////////////////////
// HwRule: class for hexword puzzle rules
///////////////////////////////////////////////////////////////////////////////
class HwRule: public Rule {
public:
    bool accepts(String &dword) const;
    void set(String &cmd);
private:
    String hword;
    const int hex = 6;
};

// Test a word to see if it matches a hexword puzzle rule.
// The rule is stored as a String with ?'s where any character
// can match, and the rest of the characters must match exactly.
// Unlike the crossword puzzle word, the hexword puzzle word doesn't
// have to match the size of the rule.  The word must be exactly
// 6 characters long, and the rule must match consecutive characters
// in the dictionary word, going in either direction, possibly
// wrapping around the end or beginning.
bool HwRule::accepts(String &dword) const {
    if ((int)dword.length()!=hex) return false;  //辞書の単語は6文字でなければならない
    // For every starting point d in the dictionary word,
    // we scan forward and backward from d to see if the rule matches.
    // Instead of d going from 0 to 6 it goes from 6 to 12 so we
    // can add or subtract (to go forward or backward) then take the
    // modulo 6 to get a character position in the dictionary word.
    for (int d=hex; d<2*hex; d++) {
        bool fwd = true, bwd = true;
        int hlen = (int)hword.length();
        for (int h=0; h<hlen; h++) {
            char c = hword[h];
            fwd = fwd && (c=='?' || c==dword[(d+h)%hex]);
            bwd = bwd && (c=='?' || c==dword[(d-h)%hex]);
            if (!fwd && !bwd) break;
        }
        if (fwd || bwd) return true;
    }
    return false;
}

// set crossword puzzle rule
void HwRule::set(String &cmd) {
    hword = cmd;
}

///////////////////////////////////////////////////////////////////////////////
// Scanner: Class to scan the dictionary searching for words that match
// a given rule.
///////////////////////////////////////////////////////////////////////////////
class Scanner {
public:
    Scanner();
    ~Scanner();
    void dict(const String &name);          // set dictionary file
    void scan(const Rule &the_rule) const;  // scan the dictionary with a rule
private:
    void operator=(Scanner &s);
    Scanner(Scanner &s);
    FILE *dictionary;
};

Scanner::Scanner() {
    dictionary = 0;
}
Scanner::~Scanner() {
    if (dictionary) fclose(dictionary);
}

// set dictionary file
void Scanner::dict(const String &name) {
    dictionary = fopen(name.s(), "r");
    if (dictionary==0) oops("can't open dictionary");
}

// scan the dictionary with a rule
void Scanner::scan(const Rule &the_rule) const {
    if (!dictionary) oops("no dictionary to serch");
    rewind(dictionary);
    String word;
    while (word.read(dictionary)) {
        if (the_rule.accept(word)) {
            cout << word << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    set_new_handler(out_of_mem);
    ios::sync_with_stdio();
    String s = "abc";
    cout << s << endl;
}
