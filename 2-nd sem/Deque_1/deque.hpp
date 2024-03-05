#include <algorithm>
#include <iostream>
#include <vector>

template <typename T>
class Deque {
 public:
  template <bool Isconst>
  class common_iterator;
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque()
      : size_(0),
        capacity_(2),
        map_(nullptr),
        row_l_(0),
        row_f_(0),
        column_l_(0),
        column_f_(0) {
    size_t count_e = 0;
    try {
      map_ = new T*[capacity_];
      for (size_t i = 0; i < capacity_; ++i) {
        map_[i] = reinterpret_cast<T*>(new uint8_t[kBlockSize * sizeof(T)]);
        ++count_e;
      }
    } catch (...) {
      cleanup(count_e, 0);
      throw;
    }
  };

  Deque(const Deque& other)
      : size_(0),
        capacity_(other.capacity_),
        map_(nullptr),
        row_l_(other.row_f_),
        row_f_(other.row_f_),
        column_l_(other.column_f_),
        column_f_(other.column_f_) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      map_ = new T*[capacity_];
      for (size_t i = 0; i < capacity_; ++i) {
        map_[i] = reinterpret_cast<T*>(new uint8_t[kBlockSize * sizeof(T)]);
        ++count_e;
      }
      while (row_l_ != other.row_l_ || column_l_ != other.column_l_) {
        new (&map_[row_l_][column_l_]) T(other.map_[row_l_][column_l_]);
        ++count_i;
        ++size_;
        ++column_l_;
        row_l_ += (column_l_ / kBlockSize);
        column_l_ %= kBlockSize;
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }

  Deque(size_t count, const T& value = T())
      : size_(0),
        capacity_(count / kBlockSize + 1),
        map_(nullptr),
        row_f_(0),
        row_l_(0),
        column_l_(0),
        column_f_(0) {
    size_t count_i = 0;
    size_t count_e = 0;
    try {
      map_ = new T*[capacity_];
      for (size_t i = 0; i < capacity_; ++i) {
        map_[i] = reinterpret_cast<T*>(new uint8_t[kBlockSize * sizeof(T)]);
        ++count_e;
      }
      while (size_ != count) {
        new (&map_[row_l_][column_l_]) T(value);
        ++count_i;
        ++size_;
        ++column_l_;
        row_l_ += (column_l_ / kBlockSize);
        column_l_ %= kBlockSize;
      }
    } catch (...) {
      cleanup(count_e, count_i);
      throw;
    }
  }

  ~Deque() {
    while (row_f_ != row_l_ || column_f_ != column_l_) {
      map_[row_f_][column_f_].~T();
      ++column_f_;
      row_f_ += (column_f_ / kBlockSize);
      column_f_ %= kBlockSize;
    }
    for (size_t i = 0; i < capacity_; ++i) {
      delete[] reinterpret_cast<uint8_t*>(map_[i]);
    }
    delete[] map_;
  };

  Deque& operator=(const Deque& other) {
    auto tmp(other);
    std::swap(row_l_, tmp.row_l_);
    std::swap(column_l_, tmp.column_l_);
    std::swap(row_f_, tmp.row_f_);
    std::swap(column_f_, tmp.column_f_);
    std::swap(capacity_, tmp.capacity_);
    std::swap(size_, tmp.size_);
    std::swap(map_, tmp.map_);
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

  void push_back(const T& value) {
    size_t prev_column_l = column_l_;
    try {
      ++size_;
      if (row_l_ >= capacity_) {
        make_more_space();
      }
      new (&map_[row_l_][column_l_]) T(value);
      ++column_l_;
      row_l_ += (column_l_ / kBlockSize);
      column_l_ %= kBlockSize;
    } catch (...) {
      column_l_ = prev_column_l;
      row_l_ -= (column_l_ / kBlockSize);
      map_[row_l_][column_l_].~T();
      --size_;
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

  void push_front(const T& value) {
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
      new (&map_[future_row_f][future_column_f]) T(value);
      row_f_ = future_row_f;
      column_f_ = future_column_f;
    } catch (...) {
      row_f_ = prev_row;
      column_f_ = prev_column;
      map_[row_f_][column_l_].~T();
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

  void insert(iterator pos, const T& value) {
    size_t prev_size = size_;
    T tmp = value;
    try {
      while (pos != end()) {
        std::swap(tmp, *pos);
        ++pos;
      }
      push_back(tmp);
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
    T** new_map = new T*[capacity_ * 2 + 1]();
    for (size_t i = 0; i < capacity_; ++i) {
      new_map[i + (capacity_ / 4 + 1)] = map_[i];
    }
    for (size_t i = 0; i < capacity_ * 2 + 1; ++i) {
      if (new_map[i] == nullptr) {
        new_map[i] = reinterpret_cast<T*>(new uint8_t[kBlockSize * sizeof(T)]);
      }
    }
    delete[] map_;
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
          --i_count;
        }
      }
    }
    for (size_t i = 0; i < e_count; ++i) {
      delete[] reinterpret_cast<uint8_t*>(map_[i]);
    }
    delete[] map_;
    map_ = nullptr;
  }

  size_t size_ = {};
  size_t capacity_{};
  T** map_ = nullptr;
  size_t row_f_ = {};
  size_t row_l_ = {};
  size_t column_f_ = {};
  size_t column_l_ = {};
  static constexpr size_t kBlockSize = 32;
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const Deque<T>& deque) {
  out << "[ ";
  for (size_t i = 0; i < deque.size(); ++i) {
    out << deque[i] << " ";
  }
  out << "]";
  return out;
}

template <typename T>
template <bool IsConst>
class Deque<T>::common_iterator {
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
