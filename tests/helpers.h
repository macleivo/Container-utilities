#include <iostream>
#include <memory>
#include <vector>

struct Base {
    int operator()() const {
        return 0xB;
    }
};

struct Derived : public Base {
    int operator()() const {
        return 0xD;
    }
};

template <typename ContainerT1, typename ContainerT2>
bool cmp(const ContainerT1& c1, const ContainerT2& c2) {
    if (c1.size() != c2.size())
        return false;
    for (size_t i = 0; i < c1.size(); ++i) {
        if (c1[i] != c2[i])
            return false;
    }
    return true;
}

struct move_only_type {
    explicit move_only_type(int val) : m_val(std::make_unique<int>(val)) {
    }
    ~move_only_type() = default;
    move_only_type(const move_only_type&) = delete;
    move_only_type& operator=(const move_only_type&) = delete;
    move_only_type(move_only_type&&) = default;
    move_only_type& operator=(move_only_type&&) = default;

    std::unique_ptr<int> m_val;

    friend bool operator<(const move_only_type& lhs, const move_only_type& rhs) {
        return *lhs.m_val < *rhs.m_val;
    }

    friend bool operator==(const move_only_type& lhs, const move_only_type& rhs) {
        return *lhs.m_val == *rhs.m_val;
    }
};

struct copy_only_type {
    explicit copy_only_type(int val) : m_val(val) {
    }
    ~copy_only_type() = default;
    copy_only_type(const copy_only_type&) = default;
    copy_only_type& operator=(const copy_only_type&) = default;
    copy_only_type(copy_only_type&&) = delete;
    copy_only_type& operator=(copy_only_type&&) = delete;

    int m_val;

    friend bool operator<(const copy_only_type& lhs, const copy_only_type& rhs) {
        return lhs.m_val < rhs.m_val;
    }

    friend bool operator==(const copy_only_type& lhs, const copy_only_type& rhs) {
        return lhs.m_val == rhs.m_val;
    }

    friend void swap(copy_only_type& lhs, copy_only_type& rhs) {
        using std::swap;
        swap(lhs.m_val, rhs.m_val);
    }
};

struct VerboseIntVector {
    VerboseIntVector(std::initializer_list<int> v = {}) : m_v(std::move(v)) {
        std::cout << "ctor verbose vector...\n";
    };
    ~VerboseIntVector() {
        std::cout << "dtor verbose vector...\n";
    }
    VerboseIntVector(const VerboseIntVector& rhs) : m_v(rhs.m_v) {
        std::cout << "copy ctor verbose vector...\n";
    }
    VerboseIntVector& operator=(const VerboseIntVector& rhs) {
        std::cout << "copy ass verbose vector...\n";
        auto tmp = rhs;
        tmp.swap(*this);
        return *this;
    }
    VerboseIntVector(VerboseIntVector&& rhs) noexcept : m_v(std::move(rhs.m_v)) {
        std::cout << "move ctor verbose vector...\n";
    }
    VerboseIntVector& operator=(VerboseIntVector&& rhs) noexcept {
        std::cout << "move ass verbose vector...\n";
        auto tmp = std::move(rhs);
        tmp.swap(*this);
        return *this;
    }

    friend void swap(VerboseIntVector& lhs, VerboseIntVector& rhs) {
        lhs.swap(rhs);
    }

    void swap(VerboseIntVector& rhs) {
        using std::swap;
        swap(m_v, rhs.m_v);
    }

    auto size() const {
        return m_v.size();
    }

    auto begin() {
        return m_v.begin();
    }
    auto end() {
        return m_v.end();
    }

    auto begin() const {
        return m_v.begin();
    }
    auto end() const {
        return m_v.end();
    }

    auto cbegin() const {
        return m_v.cbegin();
    }

    auto cend() const {
        return m_v.cend();
    }

    decltype(auto) operator[](size_t i) {
        return m_v[i];
    }

    decltype(auto) operator[](size_t i) const {
        return m_v[i];
    }

    std::vector<int> m_v;
};
