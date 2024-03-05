#include <algorithm>
#include <iostream>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  template <bool Isconst>
  class common_iterator;
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using allocator_type = Allocator;
  using value_type = T;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using alloc_traits = std::allocator_traits<Allocator>;
  using t_alloc = typename alloc_traits::template rebind_alloc<T*>;
  using t_alloc_traits = typename alloc_traits::template rebind_traits<T*>;

  Deque()
      : size_(0),
        capacity_(1),
        map_(nullptr),
        row_l_(0),
        row_f_(0),
        column_l_(0),
        column_f_(0),
        alloc_{},
        alloc_t_{} {
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
    } catch (...) {
      cleanup(count_e, 0);
      throw;
    }
  }

  Deque(const Allocator& my_alloc)
      : size_(0),
        capacity_(1),
        map_(nullptr),
        row_l_(0),
        row_f_(0),
        column_l_(0),
        column_f_(0),
        alloc_(my_alloc),
        alloc_t_(my_alloc) {
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
    } catch (...) {
      cleanup(count_e, 0);
      throw;
    }
  }
  Allocator get_allocator() const noexcept { return alloc_; }

  Deque(const Deque& other)
      : size_(0),
        capacity_(other.capacity_),
        map_(nullptr),
        row_l_(other.row_f_),
        row_f_(other.row_f_),
        column_l_(other.column_f_),
        column_f_(other.column_f_),
        alloc_(
            alloc_traits::select_on_container_copy_construction(other.alloc_)),
        alloc_t_(t_alloc_traits::select_on_container_copy_construction(
            other.alloc_t_)) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
      while (row_l_ != other.row_l_ || column_l_ != other.column_l_) {
        alloc_traits::construct(alloc_, &map_[row_l_][column_l_],
                                other.map_[row_l_][column_l_]);
        int_alloc(count_i);
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }
  Deque(Deque&& other) noexcept
      : size_(other.size_),
        capacity_(other.capacity_),
        map_(other.map_),
        row_l_(other.row_l_),
        row_f_(other.row_f_),
        column_l_(other.column_l_),
        column_f_(other.column_f_),
        alloc_(
            alloc_traits::select_on_container_copy_construction(other.alloc_)),
        alloc_t_(t_alloc_traits::select_on_container_copy_construction(
            other.alloc_t_)) {
    set_to_default(std::move(other));
  }

  Deque(size_t count, const T& value, const Allocator& alloc = Allocator())
      : size_(0),
        capacity_(count / kBlockSize + 1),
        map_(nullptr),
        row_f_(0),
        row_l_(0),
        column_l_(0),
        column_f_(0),
        alloc_(alloc),
        alloc_t_(alloc) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
      while (size_ != count) {
        alloc_traits::construct(alloc_, &map_[row_l_][column_l_], value);
        int_alloc(count_i);
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }

  Deque(size_t count, const Allocator& alloc = Allocator())
      : size_(0),
        capacity_(count / kBlockSize + 1),
        map_(nullptr),
        row_f_(0),
        row_l_(0),
        column_l_(0),
        column_f_(0),
        alloc_(alloc),
        alloc_t_(alloc) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
      while (size_ != count) {
        alloc_traits::construct(alloc_, &map_[row_l_][column_l_]);
        int_alloc(count_i);
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }

  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : size_(0),
        capacity_(init.size()),
        map_(nullptr),
        row_f_(0),
        row_l_(0),
        column_l_(0),
        column_f_(0),
        alloc_(alloc),
        alloc_t_(alloc) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      ex_alloc(count_e);
      for (const T& value : init) {
        alloc_traits::construct(alloc_, &map_[row_l_][column_l_], value);
        int_alloc(count_i);
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }

  ~Deque() {
    while (row_f_ != row_l_ || column_f_ != column_l_) {
      alloc_traits::destroy(alloc_, &map_[row_f_][column_f_]);
      ++column_f_;
      row_f_ += (column_f_ / kBlockSize);
      column_f_ %= kBlockSize;
    }
    for (size_t i = 0; i < capacity_; ++i) {
      alloc_traits::deallocate(alloc_, map_[i], kBlockSize);
    }
    t_alloc_traits::deallocate(alloc_t_, map_, capacity_);
  }

  Deque& operator=(const Deque& other) {
    if constexpr (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
      alloc_t_ = other.alloc_t_;
    }
    auto tmp(other);
    swap(*this, tmp);
    return *this;
  }

  Deque& operator=(Deque&& other) {
    auto tmp(std::move(other));
    swap(*this, tmp);
    if (alloc_traits::propagate_on_container_move_assignment::value) {
      alloc_ = other.alloc_;
    }
    if (t_alloc_traits::propagate_on_container_move_assignment::value) {
      alloc_t_ = other.alloc_t_;
    }
    return *this;
  }

  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }

  T& operator[](size_t index) {
    size_t row = (index / kBlockSize) + row_f_ +
                 (index % kBlockSize + column_f_) / kBlockSize;
    size_t col = (index + column_f_) % kBlockSize;
    return map_[row][col];
  }

  const T& operator[](size_t index) const {
    size_t row = (index / kBlockSize) + row_f_ +
                 (index % kBlockSize + column_f_) / kBlockSize;
    size_t col = (index + column_f_) % kBlockSize;
    return map_[row][col];
  }

  T& at(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("Deque index out of range");
    }
    return (*this)[index];
  }

  const T& at(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("Deque index out of range");
    }
    return (*this)[index];
  }
  void push_back(const T& value) { emplace_back(value); }

  void push_back(T&& value) { emplace_back(std::move(value)); }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    size_t prev_column_l = column_l_;
    try {
      ++size_;
      if (row_l_ >= capacity_) {
        make_more_space();
      }
      alloc_traits::construct(alloc_, &map_[row_l_][column_l_],
                              std::forward<Args>(args)...);
      ++column_l_;
      row_l_ += (column_l_ / kBlockSize);
      column_l_ %= kBlockSize;
    } catch (...) {
      --size_;
      row_l_ -= (column_l_ / kBlockSize);
      column_l_ = prev_column_l;
      --column_l_;
      alloc_traits::destroy(alloc_, &map_[row_l_][column_l_]);
      throw;
    }
  }

  void pop_back() {
    --size_;
    if (column_l_ == 0) {
      column_l_ = kBlockSize - 1;
      --row_l_;
    } else {
      --column_l_;
    }
    map_[row_l_][column_l_].~T();
  }

  void push_front(T&& value) { emplace_front(std::move(value)); }

  void push_front(const T& value) { emplace_front(value); }
  template <typename... Args>
  void emplace_front(Args&&... args) {
    size_t prev_column = column_f_;
    size_t prev_row = row_f_;
    try {
      ++size_;
      if (row_f_ == 0 && column_f_ == 0) {
        make_more_space();
      }
      size_t future_row_f;
      size_t future_column_f;
      if (column_f_ == 0) {
        future_column_f = kBlockSize - 1;
        future_row_f = row_f_ - 1;
      } else {
        future_column_f = column_f_ - 1;
        future_row_f = row_f_;
      }
      alloc_traits::construct(alloc_, &map_[future_row_f][future_column_f],
                              std::forward<Args>(args)...);
      row_f_ = future_row_f;
      column_f_ = future_column_f;
    } catch (...) {
      row_f_ = prev_row;
      column_f_ = prev_column;
      alloc_traits::destroy(alloc_, &map_[row_f_][column_l_]);
      --size_;
      throw;
    }
  }

  void pop_front() {
    --size_;
    map_[row_f_][column_f_].~T();
    ++column_f_;
    row_f_ += (column_f_ / kBlockSize);
    column_f_ %= kBlockSize;
  }

  template <typename... Args>
  void insert(iterator pos, Args&&... args) {
    emplace(pos, std::forward<Args>(args)...);
  }
  template <typename... Args>
  void emplace(iterator pos, Args&&... args) {
    size_t prev_size = size_;
    T tmp(std::forward<Args>(args)...);
    try {
      while (pos != end()) {
        std::swap(tmp, *pos);
        ++pos;
      }
      push_back(std::move(tmp));
    } catch (...) {
      size_ = prev_size;
      tmp.~T();
      throw;
    }
  }

  void erase(iterator pos) {
    try {
      while (pos != (end() - 1)) {
        std::swap(*pos, *(pos + 1));
        ++pos;
      }
      pop_back();
    } catch (...) {
      iterator prev = pos;
      --prev;
      while (prev != end()) {
        std::swap(*prev, *pos);
        ++prev;
        ++pos;
      }
      --size_;
      throw;
    }
  }

  iterator begin() noexcept { return iterator(column_f_, map_ + row_f_); }
  const_iterator begin() const noexcept {
    return const_iterator(column_f_, const_cast<const T**>(map_ + row_f_));
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(column_f_, const_cast<const T**>(map_ + row_f_));
  }
  iterator end() noexcept { return iterator(column_l_, map_ + row_l_); }
  const_iterator end() const noexcept {
    return const_iterator(column_l_, const_cast<const T**>(map_ + row_l_));
  }
  const_iterator cend() const noexcept {
    return const_iterator(column_l_, const_cast<const T**>(map_ + row_l_));
  }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator crbegin() const noexcept {
    return const_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_iterator(cbegin());
  }

 private:
  void make_more_space() {
    T** new_map = t_alloc_traits::allocate(alloc_t_, (capacity_ * 2 + 1));
    for (size_t i = 0; i < capacity_ * 2 + 1; ++i) {
      new_map[i] = nullptr;
    }
    for (size_t i = 0; i < capacity_; ++i) {
      new_map[i + (capacity_ / 4 + 1)] = map_[i];
    }
    for (size_t i = 0; i < capacity_ * 2 + 1; ++i) {
      if (new_map[i] == nullptr) {
        new_map[i] = alloc_traits::allocate(alloc_, kBlockSize);
      }
    }
    t_alloc_traits::deallocate(alloc_t_, map_, capacity_);
    row_f_ += (capacity_ / 4 + 1);
    row_l_ += (capacity_ / 4 + 1);
    map_ = new_map;
    capacity_ *= 2;
    capacity_ += 1;
  }

  void cleanup(size_t e_count, size_t i_count) {
    for (size_t i = row_f_; i < capacity_; ++i) {
      for (size_t j = 0; j < kBlockSize; ++j) {
        if (i_count != 0) {
          map_[i][j].~T();
          i_count--;
        }
      }
    }
    for (size_t i = 0; i < e_count; ++i) {
      alloc_traits::deallocate(alloc_, map_[i], kBlockSize);
    }
    t_alloc_traits::deallocate(alloc_t_, map_, capacity_);
    map_ = nullptr;
  }

  void ex_alloc(size_t& e_count) {
    map_ = t_alloc_traits::allocate(alloc_t_, capacity_);
    for (size_t i = 0; i < capacity_; ++i) {
      map_[i] = alloc_traits::allocate(alloc_, kBlockSize);
      ++e_count;
    }
  }

  void int_alloc(size_t& i_count) {
    ++i_count;
    ++size_;
    ++column_l_;
    row_l_ += (column_l_ / kBlockSize);
    column_l_ %= kBlockSize;
  }
  void swap(Deque& first, Deque& second) {
    std::swap(first.row_l_, second.row_l_);
    std::swap(first.column_l_, second.column_l_);
    std::swap(first.row_f_, second.row_f_);
    std::swap(first.column_f_, second.column_f_);
    std::swap(first.capacity_, second.capacity_);
    std::swap(first.size_, second.size_);
    std::swap(first.map_, second.map_);
  }

  void set_to_default(Deque&& other) {
    other.size_ = 0;
    other.capacity_ = 0;
    other.map_ = nullptr;
    other.row_l_ = 0;
    other.row_f_ = 0;
    other.column_l_ = 0;
    other.column_f_ = 0;
  }

  size_t size_ = {};
  size_t capacity_{};
  T** map_ = nullptr;
  size_t row_f_ = {};
  size_t row_l_ = {};
  size_t column_f_ = {};
  size_t column_l_ = {};
  Allocator alloc_ = {};
  t_alloc alloc_t_ = {};
  static constexpr size_t kBlockSize = 32;
};

template <typename T, typename Allocator>
template <bool IsConst>
class Deque<T, Allocator>::common_iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using pointer = value_type*;
  using reference = value_type&;
  using difference_type = std::ptrdiff_t;

  common_iterator() noexcept : index_(0), map_(nullptr) {}

  explicit common_iterator(size_t size, pointer* p_deque)
      : index_(size), map_(p_deque) {}

  common_iterator(const common_iterator& other) noexcept
      : index_(other.index_), map_(other.map_) {}

  common_iterator& operator=(const common_iterator& other) noexcept {
    if (this != &other) {
      common_iterator tmp(other);
      std::swap(index_, tmp.index_);
      std::swap(map_, tmp.map_);
    }
    return *this;
  }
  ~common_iterator() = default;

  reference operator*() const noexcept {
    return *(*map_ + index_ % kBlockSize);
  }
  pointer operator->() const noexcept { return (*map_ + index_ % kBlockSize); }
  common_iterator& operator++() noexcept {
    ++index_;
    if ((index_) % kBlockSize == 0) {
      ++map_;
      index_ = 0;
    }
    return *this;
  }
  common_iterator& operator--() noexcept {
    if (index_ == 0) {
      map_--;
      index_ = kBlockSize - 1;
    } else {
      index_--;
    }
    return *this;
  }
  explicit operator common_iterator<true>() const {
    return common_iterator<true>(index_, map_);
  }

  common_iterator operator--(int) noexcept {
    common_iterator tmp(*this);
    --(*this);
    return tmp;
  }

  common_iterator operator++(int) noexcept {
    common_iterator tmp(*this);
    ++(*this);
    return tmp;
  }

  common_iterator& operator+=(difference_type num) {
    if (num > 0) {
      size_t offset = (index_ + num) % kBlockSize;
      size_t steps = (index_ + num) / kBlockSize;
      if (offset == index_) {
        steps++;
      }
      index_ = offset;
      map_ += steps;
    } else if (num < 0) {
      index_ -= -num;
      size_t steps = index_ / kBlockSize;
      if (steps > 0) {
        index_ %= kBlockSize;
        map_ -= steps;
      }
    }
    return *this;
  }

  common_iterator operator+(difference_type num) const {
    common_iterator tmp(*this);
    return (tmp += num);
  }

  common_iterator& operator-=(difference_type num) {
    if (num > 0) {
      size_t offset = (index_ + kBlockSize - num % kBlockSize) % kBlockSize;
      size_t steps = num / kBlockSize;
      if (offset > index_) {
        steps++;
      }
      index_ = offset;
      map_ -= steps;
    } else if (num < 0) {
      index_ += -num;
      size_t steps = index_ / kBlockSize;
      if (steps > 0) {
        index_ %= kBlockSize;
        map_ += steps;
      }
    }
    return *this;
  }

  common_iterator operator-(difference_type num) const {
    common_iterator tmp(*this);
    return (tmp -= num);
  }
  difference_type operator-(const common_iterator& other) {
    return static_cast<difference_type>(map_ - other.map_) * kBlockSize +
           static_cast<difference_type>(index_) -
           static_cast<difference_type>(other.index_);
  }

  bool operator<(const common_iterator& other) const {
    if (this->map_ < other.map_) {
      return true;
    }
    if (this->map_ == other.map_) {
      return (this->index_ < other.index_);
    }
    return false;
  }
  bool operator<=(const common_iterator& other) const {
    return ((*this < other) || (*this == other));
  }
  bool operator>(const common_iterator& other) const {
    return (*this != other) && (*this >= other);
  }
  bool operator>=(const common_iterator& other) const {
    return !(*this < other);
  }
  bool operator==(const common_iterator& other) const {
    return ((*this >= other) && (other >= *this));
  }
  bool operator!=(const common_iterator& other) const {
    return !(*this == other);
  }

  reference operator[](difference_type index) const { return *(*this + index); }

 private:
  pointer* map_ = nullptr;
  size_t index_ = 0;
  static constexpr size_t kBlockSize = 32;
};
