#include <iostream>

template <typename T>
class unique_ptr {
private:
    T *ptr = nullptr;
public:
    unique_ptr() {}
    explicit unique_ptr(T *ptr) : ptr(ptr) {}
    ~unique_ptr() { delete ptr; }
    //コピーは禁止
    unique_ptr(const unique_ptr &) = delete;
    unique_ptr & operator=(const unique_ptr &) = delete;
    //ムーブ
    unique_ptr(unique_ptr &&r) : ptr(r.ptr) { r.ptr = nullptr; }
    unique_ptr &operator=(unique_ptr &&r) {
        delete ptr;
        ptr = r.ptr;
        r.ptr = nullptr;
        return *this;
    }
    T &operator* () noexcept { return *ptr; }
    T *operator->() noexcept { return ptr; }
    T *get()        noexcept { return ptr; }
};

template <typename T>
class shared_ptr {
private:
    T *ptr = nullptr;
    std::size_t *count = nullptr;
    void release() {
        if (count==nullptr) return;
        --*count;
        if (*count==0) {
            delete ptr;
            delete count;
            ptr   = nullptr;
            count = nullptr;
        }
    }
public:
    shared_ptr() {}
    explicit shared_ptr(T *ptr) : ptr(ptr), count(new std::size_t(1)) {}
    ~shared_ptr() { release(); }
    //コピー
    shared_ptr(const shared_ptr &r) : ptr(r.ptr), count(r.count) { ++*count; }
    shared_ptr & operator=(const shared_ptr &r) {
        if (this==&r) return *this;
        release();
        ptr   = r.ptr;
        count = r.count;
        ++*count;
        return *this;
    }
    //ムーブ
    shared_ptr(shared_ptr &&r) : ptr(r.ptr), count(r.count) {
        r.ptr  = nullptr;
        r.count = nullptr;
    }
    shared_ptr &operator=(shared_ptr &&r) {
        release();
        ptr   = r.ptr;
        count = r.count;
        r.ptr   = nullptr;
        r.count = nullptr;
        return *this;
    }
    T &operator* () noexcept { return *ptr; }
    T *operator->() noexcept { return ptr; }
    T *get()        noexcept { return ptr; }
    void print(std::ostream *os) const {
        *os << "{" << ptr << ", count=" << (count?*count:0) << "}";
    }
};
template <typename T>
std::ostream &operator<<(std::ostream &os, const shared_ptr<T> &v) {
    v.print(&os);
    return os;
}

template <typename T>
unique_ptr<T> make_unique(T val) {
    unique_ptr<T> p(new T);
    *p = val;
    return p;
}

template <typename T>
shared_ptr<T> make_shared(T val) {
    shared_ptr<T> p(new T);
    *p = val;
    return p;
}

int main() {
    unique_ptr<int> up1(new int);
    *up1 = 5;
    std::cout << *up1 << std::endl;

    auto up2 = make_unique<int>(10);
    std::cout << *up2 << std::endl;

    shared_ptr<int> sp1(new int);
    *sp1 = 3;
    shared_ptr<int> sp2;
    sp2 = sp1;
    std::cout << "sp1" << sp1 << std::endl;
    std::cout << "sp2" << sp2 << std::endl;

    sp2 = std::move(sp1);
    std::cout << "sp1" << sp1 << std::endl;
    std::cout << "sp2" << sp2 << std::endl;

    return 0;
}