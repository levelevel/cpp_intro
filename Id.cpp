#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <iostream>
using namespace std;

void error(const char *msg) {
    cerr << "Error:" << msg << "\n";
    exit(1);
}

class IdData {      //動的配列版
friend class Id;
public:
    IdData(void);                   //デフォルトコンストラクタ
    ~IdData(void);                  //デストラクタ
    void operator=(IdData &src);    //代入演算子
    IdData(IdData &src);            //コピーコンストラクタ
public:
    void print(ostream *os);
    void set(const char *idStr);
private:
    void newVal(const char *val);
    void deleteVal(void);
private:
    char *value;
    unsigned int refCount;
};

IdData::IdData(void) {              //デフォルトコンストラクタ
    newVal("0");
    refCount = 0;
}
IdData::~IdData(void) {             //デストラクタ
    deleteVal();
}
void IdData::operator=(IdData &src) {//代入演算子
    deleteVal();
    newVal(src.value);
}
IdData::IdData(IdData &src) {       //コピーコンストラクタ
    newVal(src.value);
    refCount = src.refCount;
}
void IdData::print(ostream *os) {
    *os << "IdData{" << value << ", refCount=" << refCount << "}";
}
ostream &operator<<(ostream &os, IdData &theId) {
    theId.print(&os);
    return os;
}
void IdData::set(const char *idStr) {
    deleteVal();
    newVal(idStr);
}

void IdData::newVal(const char *idStr) {
    if (idStr==0) error("bad id");
    value = new char [strlen(idStr)+1];
    strcpy(value, idStr);
}
void IdData::deleteVal(void) {
    delete[] value;
}

class Id {
public:
    Id(void);                   //デフォルトコンストラクタ
    ~Id(void);                  //デストラクタ
    void operator=(Id &src);    //代入演算子
    Id(Id &src);                //コピーコンストラクタ
public:
    void print(ostream *os);
    void set(const char *idStr);
private:
    void attachData(IdData *theData);
    void detachData(void);
private:
    IdData *data;
};
Id::Id(void) {
    attachData(0);
}
Id::~Id(void) {
    detachData();
}
void Id::operator=(Id &src) {
    detachData();
    attachData(src.data);
}
Id::Id(Id &src) {
    attachData(src.data);
}
void Id::print(ostream *os) {
    *os << "Id={" << data << "-->";
    if (data==0) *os << "(null)";
    else         *os << *data;
    *os << "}";
}
ostream &operator<<(ostream &os, Id &theId) {
    theId.print(&os);
    return os;
}
void Id::set(const char *idStr) {
    if (data->refCount>1) {
        detachData();
        attachData(0);
    }
    data->set(idStr);
}
void Id::attachData(IdData *theData) {
    if (theData==0) {
        data = new IdData;
    } else if (theData->refCount==UINT_MAX) {
        data = new IdData;
        set(theData->value);
    } else {
        data = theData;
    }
    ++data->refCount;
}
void Id::detachData(void) {
    if (--data->refCount==0) delete data;
}

int main(int argc, char* argv[]) {
    Id id1;
    Id id2 = id1;
    cout << "id1=" << id1 << "\n";
    cout << "id2=" << id2 << "\n";

    id1.set("123");
    cout << "id1=" << id1 << "\n";
    cout << "id2=" << id2 << "\n";
    id2 = id1;
    cout << "id2=" << id2 << "\n";

    return 0;
}
