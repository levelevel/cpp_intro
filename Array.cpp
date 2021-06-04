#include <iostream>
#include <iterator>
#include <cstdlib>
#include <array>
#include <algorithm>
//using namespace std;

template <typename T, size_t N>
struct Array {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using iterator = T*;
    using const_iterator = const T*;

    value_type storage[N];  //データ本体

    pointer       data()       noexcept { return storage; }
    const_pointer data() const noexcept { return storage; }
    size_type     size() const noexcept { return N; }
    reference       operator[](size_t n)       noexcept { return storage[n]; }
    const_reference operator[](size_t n) const noexcept { return storage[n]; }
    reference at (size_t n) {
        if (n>=size()) throw std::out_of_range("Error: Out of Range");
         return storage[n];
    }
    const_reference at (size_t n) const {
        if (n>=size()) throw std::out_of_range("Error: Out of Range");
         return storage[n];
    }
    reference       front()       noexcept { return storage[0]; }
    reference       back ()       noexcept { return storage[N-1]; }
    const_reference front() const noexcept { return storage[0]; }
    const_reference back () const noexcept { return storage[N-1]; }
    void print(std::ostream *os) const {
        *os << "{";
        for (std::size_t i=0; i<size(); ++i) {
            if (i) *os << ", ";
            *os << storage[i];
        }
        *os << "}";
    }
    void fill(const value_type& n) {
        for (std::size_t i=0; i<size(); ++i) {
            storage[i] = n;
        }
    }
    iterator       begin ()       noexcept { return storage; }
    iterator       end   ()       noexcept { return storage+N; }
    const_iterator begin () const noexcept { return storage; }
    const_iterator end   () const noexcept { return storage+N; }
    const_iterator cbegin() const noexcept { return storage; }
    const_iterator cend  () const noexcept { return storage+N; }
};

template <typename T, size_t N>
std::ostream &operator<<(std::ostream &os, const Array<T, N> &s) {
    s.print(&os);
    return os;
}

int main() {
    using int_array10 = Array<int, 10>;
    int_array10 a = {0,1,2,3,4,5,6,7,8,9};
    a[3] = 30;
    std::cout << a << std::endl;
    std::for_each(a.begin(), a.end(), [](auto x){std::cout << x << ", ";});
    std::cout << std::endl;

    auto b = a.begin();
    b++; b--; b+=2; b=b+1; *b=99;
    std::cout << *(b+1) << std::endl;

    auto cb = a.cbegin();
    cb++; cb--; cb+=2; cb=cb+1; //*cb=99;
    std::cout << *(cb+1) << std::endl;

    try { 
        a.at(100);
    }
    catch(std::out_of_range& e) {
        std::cout << e.what() << std::endl;
    }

    std::ostream_iterator<int> out(std::cout);
    std::copy(std::begin(a), std::end(a), out);

    return 0;
}
