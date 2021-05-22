#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

void error(const char *msg) {
    cerr << "Error:" << msg << "\n";
    exit(1);
}

class Id {      //動的配列版
public:
    Id(void);                   //デフォルトコンストラクタ
    ~Id(void);                  //デストラクタ
    void operator=(Id &src);    //代入演算子
    Id(Id &src);                //コピーコンストラクタ
public:
    void print(ostream *os);
    void set(const char *idStr);
private:
    void newVal(const char *val);
    void deleteVal(void);
private:
    char *value;
};

Id::Id(void) {
    newVal("0");
}
Id::~Id(void) {
    deleteVal();
}
void Id::operator=(Id &src) {
    deleteVal();
    newVal(src.value);
}
Id::Id(Id &src) {     //コピーコンストラクタ
    newVal(src.value);
}
void Id::print(ostream *os) {
    *os << "Id{" << value << "}";
}
ostream &operator<<(ostream &os, Id &theId) {
    theId.print(&os);
    return os;
}
void Id::set(const char *idStr) {
    deleteVal();
    newVal(idStr);
}

void Id::newVal(const char *idStr) {
    if (idStr==0) error("bad id");
    value = new char [strlen(idStr)+1];
    strcpy(value, idStr);
}
void Id::deleteVal(void) {
    delete[] value;
}

int main(int argc, char* argv[]) {
    Id id1;
    Id id2 = id1;
    cout << "id1=" << id1 << "\n";
    cout << "id2=" << id2 << "\n";

    id1.set("123");
    cout << "id1=" << id1 << "\n";
    id2 = id1;
    cout << "id2=" << id2 << "\n";

    return 0;
}
