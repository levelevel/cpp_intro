#include <iostream>
#include <iterator>
#include <algorithm>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class vector {
public:
    using value_type            = T;
    using pointer               = T*;
    using const_pointer         = const pointer;
    using reference             = value_type&;
    using const_reference       = const reference;
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
    void destroy_until(reverse_iterator rend) { //末尾(rbegin())から要素を破棄する
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
        std::cout << "Construct1\n";
        resize(size);
        std::cout << "Construct1 end\n";
    }
    vector(size_type size, const_reference value, const allocator_type & alloc = Allocator()) : vector(alloc) {
        std::cout << "Construct2\n";
        resize(size, value);
        std::cout << "Construct2 end\n";
    }
    ~vector() {
        std::cout << "Destruct\n";
        clear();        //要素を末尾から先頭に向かう順番で破棄
        deallocate();   //生のメモリーを開放する
        std::cout << "Destruct end\n";
    }
    vector(const vector &v);
    vector &operator=(const vector &v);
    void push_back(const T &v);
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
        auto ptr = allocate(sz);    //動的メモリ確保
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
    void resize(size_type sz) {
        std::cout << "Resize1\n";
        if (sz<size()) {
            auto diff = size() - sz;
            destroy_until(rbegin()+diff);
            last = first + sz;
        } else if (sz>size()) {
            reserve(sz);
            for (; last!=reserved_last; last++) construct(last);
        }
        std::cout << "Resize1 end\n";
    }
    void resize(size_type sz, const_reference value) {
        if (sz<size()) {
            auto diff = size() - sz;
            destroy_until(rbegin()+diff);
            last = first + sz;
        } else if (sz>size()) {
            reserve(sz, value);
            for (; last!=reserved_last; last++) construct(last);
        }
    }
};

int main() {
    std::vector<int> sv(10, 5);
    vector<int> v(10, 5);
}