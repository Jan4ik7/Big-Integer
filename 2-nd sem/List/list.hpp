#pragma once
#include <algorithm>
#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  using allocator_type = Allocator;
  using value_type = T;
  template <bool IsConst>
  class common_iterator;
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List() : size_(0), fake_node_{} {
    fake_node_.prev = &fake_node_;
    fake_node_.next = &fake_node_;
  }

  List(size_t count, const T& value = T(), const Allocator& alloc = Allocator())
      : size_(0), fake_node_{}, alloc_{alloc} {
    fake_node_.prev = &fake_node_;
    fake_node_.next = &fake_node_;
    try {
      for (size_t i = 0; i < count; ++i) {
        push_back(value);
      }
    } catch (...) {
      this->clear();
      throw;
    }
  }

  explicit List(int count, const Allocator& alloc = Allocator())
      : size_(0), fake_node_{}, alloc_{alloc} {
    Node* new_node = nullptr;
    fake_node_.prev = &fake_node_;
    fake_node_.next = &fake_node_;
    try {
      for (int i = 0; i < count; ++i) {
        construct_node(fake_node_.prev, &fake_node_);
      }
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      this->clear();
      throw;
    }
  }

  List(const List& other)
      : size_(0),
        fake_node_{},
        alloc_{node_alloc_traits::select_on_container_copy_construction(
            other.alloc_)} {
    fake_node_.prev = &fake_node_;
    fake_node_.next = &fake_node_;
    try {
      for (const auto& value : other) {
        push_back(value);
      }
    } catch (...) {
      this->clear();
      throw;
    }
  }

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : size_(0), fake_node_{}, alloc_{alloc} {
    fake_node_.prev = &fake_node_;
    fake_node_.next = &fake_node_;
    try {
      for (auto& value : init) {
        push_back(value);
      }
    } catch (...) {
      this->clear();
      throw;
    }
  }

  List& operator=(const List& other) {
    auto tmp(other);
    std::swap(tmp.fake_node_, fake_node_);
    std::swap(tmp.size_, size_);
    if (node_alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
    return *this;
  }

  ~List() {
    while (!empty()) {
      pop_back();
    }
  }

  void push_back(const T& value) {
    construct_node(fake_node_.prev, &fake_node_, value);
  }

  void push_front(const T& value) {
    construct_node(&fake_node_, fake_node_.next, value);
  }

  Allocator get_allocator() const noexcept { return alloc_; }

  void pop_back() {
    if (empty()) {
      return;
    }
    Node* node_to_delete = static_cast<Node*>(fake_node_.prev);
    node_to_delete->prev->next = (&fake_node_);
    fake_node_.prev = (node_to_delete->prev);
    node_alloc_traits::destroy(alloc_, node_to_delete);
    node_alloc_traits::deallocate(alloc_, (node_to_delete), 1);
    --size_;
  }

  void pop_front() {
    if (empty()) {
      return;
    }
    Node* node_to_delete = static_cast<Node*>(fake_node_.next);
    node_to_delete->next->prev = (&fake_node_);
    fake_node_.next = node_to_delete->next;
    node_alloc_traits::destroy(alloc_, node_to_delete);
    node_alloc_traits::deallocate(alloc_, node_to_delete, 1);
    --size_;
  }

  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

  T& front() { return fake_node_.next->value; }

  const T& front() const { return fake_node_.next->value; }

  T& back() { return fake_node_.prev->value; }

  const T& back() const { return fake_node_.prev->value; }

  iterator begin() { return iterator(static_cast<BaseNode*>(fake_node_.next)); }

  const_iterator begin() const {
    return const_iterator(static_cast<BaseNode*>(fake_node_.next));
  }

  const_iterator cbegin() const {
    return const_iterator(static_cast<BaseNode*>(fake_node_.next));
  }

  iterator end() { return iterator((&fake_node_)); }

  const_iterator end() const { return const_iterator((&fake_node_)); }

  const_iterator cend() const { return const_iterator((&fake_node_)); }

  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

  const_reverse_iterator crbegin() const noexcept {
    return const_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_iterator(cbegin());
  }

 private:
  struct BaseNode {
    BaseNode() : prev(nullptr), next(nullptr) {}
    BaseNode(BaseNode* prev, BaseNode* next) : prev(prev), next(next) {}

    ~BaseNode() = default;

    BaseNode* prev;
    BaseNode* next;
  };

  struct Node : BaseNode {
    T value;
    Node() : BaseNode(nullptr, nullptr), value() {}
    Node(const T& value) : BaseNode(nullptr, nullptr), value(value) {}
    Node(const T& value, Node* prev, Node* next)
        : BaseNode(prev, next), value(value) {}

    ~Node() = default;
  };
  using alloc_traits = std::allocator_traits<Allocator>;
  using node_alloc = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = typename alloc_traits::template rebind_traits<Node>;
  size_t size_{0};
  node_alloc alloc_;
  BaseNode fake_node_{};

  void clear() {
    while (!empty()) {
      pop_back();
    }
  }
  template <typename... Args>
  Node* construct_node(BaseNode* prev, BaseNode* next, Args&&... args) {
    Node* new_node = node_alloc_traits::allocate(alloc_, 1);
    try {
      node_alloc_traits::construct(alloc_, new_node,
                                   std::forward<Args>(args)...);
      new_node->prev = static_cast<Node*>(prev);
      new_node->next = static_cast<Node*>(next);
      prev->next = new_node;
      next->prev = new_node;
      ++size_;
    } catch (...) {
      node_alloc_traits::deallocate(alloc_, new_node, 1);
      throw;
    }
    return new_node;
  }
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::common_iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::conditional_t<IsConst, const T, T>;
  using bnode_type = std::conditional_t<IsConst, const BaseNode, BaseNode>;
  using node_type = std::conditional_t<IsConst, const Node, Node>;
  using pointer = value_type*;
  using reference = value_type&;
  using difference_type = std::ptrdiff_t;

  common_iterator() : ptr_(nullptr) {}

  common_iterator(bnode_type* node) : ptr_(node) {}

  common_iterator(const common_iterator& other) noexcept : ptr_(other.ptr_) {}

  common_iterator(common_iterator&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
  }

  common_iterator& operator=(common_iterator&& other) noexcept {
    if (this != &other) {
      common_iterator tmp = std::move(other);
      std::swap(ptr_, tmp.ptr_);
      other.ptr_ = nullptr;
    }
    return *this;
  }

  common_iterator& operator=(const common_iterator& other) {
    if (this != &other) {
      ptr_ = other.ptr_;
    }
    return *this;
  }

  reference operator*() const noexcept {
    node_type* n_ptr = static_cast<node_type*>(ptr_);
    return n_ptr->value;
  }

  pointer operator->() const noexcept {
    node_type* n_ptr = static_cast<node_type*>(ptr_);
    return &(n_ptr->value);
  }

  common_iterator& operator++() noexcept {
    ptr_ = ptr_->next;
    return *this;
  }

  common_iterator operator++(int) {
    common_iterator temp(*this);
    ptr_ = ptr_->next;
    return temp;
  }

  common_iterator& operator--() {
    ptr_ = ptr_->prev;
    return *this;
  }

  common_iterator operator--(int) {
    common_iterator temp(*this);
    ptr_ = ptr_->prev;
    return temp;
  }

  friend bool operator==(const common_iterator& lhs,
                         const common_iterator& rhs) {
    return lhs.ptr_ == rhs.ptr_;
  }

  friend bool operator!=(const common_iterator& lhs,
                         const common_iterator& rhs) {
    return lhs.ptr_ != rhs.ptr_;
  }

 private:
  bnode_type* ptr_;
};
