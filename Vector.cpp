#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <array>

template <typename T, typename Allocator = std::allocator<T>>
class vector {
public:
    using value_type            = T;
    using pointer               = T*;
    using const_pointer         = const T*;
    using reference             = value_type&;
//  using const_reference       = const reference;
    using const_reference       = const value_type&;
    using allocator_type        = Allocator;
    using size_type             = std::size_t;
    using difference_type       = std::ptrdiff_t;
    using iterator              = pointer;
    using const_iterator        = const_pointer;
    using reverse_iterator      = std::reverse_iterator<iterator>;
    using const_reverse_iterator= std::reverse_iterator<const_iterator>;

private:
    pointer first = nullptr;            //先頭の要素のポインタ
    pointer last  = nullptr;            //最後の要素の一つ前方のポインタ
    pointer reserved_last = nullptr;    //確保したストレージの終端の一つ前方のポインタ
    allocator_type alloc;

private:
    using traits = std::allocator_traits<allocator_type>;
    pointer allocate(size_type n) { return traits::allocate(alloc, n); }
    void deallocate() { traits::deallocate(alloc, first, capacity()); }
    void construct(pointer ptr) { traits::construct(alloc, ptr); }
    void construct(pointer ptr, const_reference value) { traits::construct(alloc, ptr, value); }
    void construct(pointer ptr, value_type &&value) { traits::construct(alloc, ptr, std::move(value)); }
    void destroy(pointer ptr) { traits::destroy(alloc, ptr); }
    void destroy_until(reverse_iterator rend) { //末尾(rbegin())から指定の要素までを破棄する
        for (auto riter=rbegin(); riter!=rend; ++riter, --last) {
            //簡易vector<T>のiteratorは単なるT *だが、
            //riterはリバースイテレーターなのでポインターではない。
            //ポインターを取るために*riterでまずT &を得て、
            //そこに&を適用することでT *を得ている。
            destroy(&*riter);
        }
    }
    void clear() noexcept {
        destroy_until(rend());
    }

public:
    vector(const allocator_type & alloc) noexcept : alloc(alloc) {}
    vector() : vector(allocator_type()) {}
    vector(size_type size, const allocator_type & alloc = Allocator()) : vector(alloc) {
        std::cout << "Default Construct\n";
        resize(size);
    }
    vector(size_type size, const_reference value, const allocator_type & alloc = Allocator()) : vector(alloc) {
        std::cout << "Copy Construct\n";
        resize(size, value);
    }
    template<typename InputIterator, typename = std::_RequireInputIter<InputIterator> >
    vector(InputIterator first, InputIterator last, const allocator_type & alloc = Allocator()) : vector(alloc) {
        std::cout << "Construct3\n";
        //reserve(std::distance(first, last));
        reserve(last-first);
        for (auto i=first; i!=last; ++i) push_back(*i);
    }
    vector(std::initializer_list<value_type> init, const Allocator & = Allocator())
        : vector(std::begin(init), std::end(init), alloc) {}
    ~vector() {
        std::cout << "Destruct\n";
        clear();        //要素を末尾から先頭に向かう順番で破棄
        deallocate();   //生のメモリーを開放する
    }
    vector(const vector &r) : alloc(traits::select_on_container_copy_construction(r.alloc)) {
        reserve(r.size());
        for (auto dest=first, src=r.begin(), last=r.end(); src!=last; ++dest, ++src)
            construct(dest, *src);
        last = first + r.size();
    }
    vector &operator=(const vector &r) {
        if (this==&r) return *this;
        if (size()==r.size()) {
            std::copy(r.begin(), r.end(), begin());
        } else {
            destroy_until(rend());
            reserve(r.size()) ;
            for (auto dest=first, src=r.begin(), last=r.end(); src!=last; ++dest, ++src)
                construct(dest, *src);
            last = first + r.size();
        }
        return *this;
    }
    reference       operator[](size_type i)       noexcept { return first[i]; }
    const_reference operator[](size_type i) const noexcept { return first[i]; }
    reference at(size_type i) {
        if (i>=size()) throw std::out_of_range("index is out of range");
        return first[i];
    }
    const_reference at(size_type i) const {
        if (i>=size()) throw std::out_of_range("index is out of range");
        return first[i];
    }
    reference       front()       noexcept { return first; }
    reference       back()        noexcept { return last-1; }
    const_reference front() const noexcept { return first; }
    const_reference back()  const noexcept { return last-1; }
    pointer         data()        noexcept { return first; }
    const_pointer   data()  const noexcept { return first; }

    iterator       begin ()       noexcept { return first; }
    iterator       end   ()       noexcept { return last; }
    const_iterator begin () const noexcept { return first; }
    const_iterator end   () const noexcept { return last; }
    const_iterator cbegin() const noexcept { return first; }
    const_iterator cend  () const noexcept { return last; }
    reverse_iterator       rbegin()        noexcept { return reverse_iterator{last}; } //lastであることに注意
    reverse_iterator       rend  ()        noexcept { return reverse_iterator{first}; }
    const_reverse_iterator rbegin()  const noexcept { return const_reverse_iterator{last}; }
    const_reverse_iterator rend  ()  const noexcept { return const_reverse_iterator{first}; }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator{last}; }
    const_reverse_iterator crend  () const noexcept { return const_reverse_iterator{first}; }
    size_type size()     const noexcept { return end() - begin(); }
    size_type capacity() const noexcept { return reserved_last - first; }
    bool empty() const noexcept { return begin()==end(); }
    void reserve(size_type sz) {
        if (sz<=capacity()) return;
        auto ptr = allocate(sz);    //指定されたサイズの動的メモリ確保
        auto old_first    = first;
        auto old_last     = last;
        auto old_capacity = capacity();
        first = ptr;
        last  = first;
        reserved_last = first + sz;
        //古いストレージから新しいストレージに要素をムーブ
        for (auto old_iter=old_first; old_iter!=old_last; ++old_iter, ++last) {
            construct(last, std::move(*old_iter));
        }
        //古いストレージの値を破棄
        for (auto riter=reverse_iterator(old_last), rend=reverse_iterator(old_first);
            riter!=rend; ++riter ) {
                destroy(&*riter);
            }
        traits::deallocate(alloc, old_first, old_capacity);
    }
    void resize(size_type sz) { //要素数を変更する
        if (sz<size()) {        //要素が減る場合、末尾から破棄する
            auto diff = size() - sz;
            destroy_until(rbegin()+diff);
            last = first + sz;
        } else if (sz>size()) { //要素が増える場合、末尾に要素を追加する
            reserve(sz);
            for (; last!=reserved_last; last++) construct(last);
        }
    }
    void resize(size_type sz, const_reference value) {
        if (sz<size()) {
            auto diff = size() - sz;
            destroy_until(rbegin()+diff);
            last = first + sz;
        } else if (sz>size()) {
            reserve(sz);
            for (; last!=reserved_last; last++) construct(last, value);
        }
    }
    void shrink_to_fit() {      //メモリを要素数切り詰める
        if (size()==capacity()) return;
        auto ptr = allocate(size());
        auto current_size = size();
        for (auto raw_ptr=ptr, iter=begin(); iter!=end(); ++raw_ptr, ++iter) construct(raw_ptr, *iter);
        clear();        //要素を末尾から先頭に向かう順番で破棄
        deallocate();   //生のメモリーを開放する
        first = ptr;
        last  = ptr + current_size;
        reserved_last = last;
    }
    void push_back(const_reference value) {
        auto cap = capacity();
        if (size()+1 > cap) {
            cap = cap?cap*2:8;
            reserve(cap);
        }
        construct(last, value);
        ++last;
    }
    void print(std::ostream *os) const {
        *os << "{";
        for (std::size_t i=0; i<size(); ++i) {
            if (i) *os << ", ";
            *os << first[i];
        }
        *os << "}";
    }
};

template <typename T>
std::ostream &operator<<(std::ostream &os, const vector<T> &v) {
    v.print(&os);
    return os;
}

int main() {
    vector<int> v2(10,99);
    std::cout << "v2" << v2 << std::endl;
    v2.resize(5);
    std::cout << "v2" << v2 << std::endl;
    for (std::size_t i=0; i<10; ++i) v2.push_back(i+5);
    std::cout << "v2" << v2 << std::endl;

    v2.reserve(100);
    v2.shrink_to_fit();

    std::array<int,5> a3 = {1,2,3,4,5};
    vector<int> v3(std::begin(a3), std::end(a3));
    std::cout << "v2" << v3 << std::endl;

    vector<int> v4 = {1,2,3,4,5,6};
    std::cout << "v4" << v4 << std::endl;

    vector<int> v5(v4);
    std::cout << "v5" << v5 << std::endl;

    vector<int> v6a(10,0);
    std::cout << "v6a" << v6a << std::endl;
    v6a = v5;
    std::cout << "v6a" << v6a << std::endl;

    vector<int> v6b(3,0);
    std::cout << "v6b" << v6b << std::endl;
    v6b = v5;
    std::cout << "v6b" << v6b << std::endl;
}
