#include <stdlib.h>
#include <iostream>
using namespace std;

void error(const char *msg) {
    cerr << "Error:" << msg << "\n";
    exit(1);
}

class Id {
public:
    Id();                       //デフォルトコンストラクタ
    //~Id();                      //デストラクタ
    //void operator=(Id &src);    //代入演算子
    //Id(Id &src);                //コピーコンストラクタ
public:
    void print(ostream *os);
    void set(const char *idStr);
private:
    int value;
};

Id::Id() {
    value = 0;
}
void Id::print(ostream *os) {
    *os << "Id{" << value << "}";
}
ostream &operator<<(ostream &os, Id &theId) {
    theId.print(&os);
    return os;
}
void Id::set(const char *idStr) {
    if (idStr==0) error("Bad idStr");
    value = atoi(idStr);
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

    id2.set(0);

    return 0;
}
