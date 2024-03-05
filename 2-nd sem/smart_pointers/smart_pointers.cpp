#pragma once
#include <memory>

template <typename T>
class SharedPtr {
 public:
  using element_type = T;
  template <typename U>
  friend class WeakPtr;

  template <typename... Args>
  friend SharedPtr<T> MakeShared(Args&&... args);
  template <typename Alloc, typename... Args>
  friend SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args);

  SharedPtr() : control_block_(nullptr) , ptr_(nullptr) {}

  explicit SharedPtr(T* ptr) {
    control_block_ =
        new ControlBlockRegular<T, std::default_delete<T>, std::allocator<T>>(
            ptr, std::default_delete<T>(), std::allocator<T>());
    shared_counter();
  }

  SharedPtr(const SharedPtr &other): control_block_(other.cb_), ptr_(other.ptr_) {
    shared_counter();
  }

  SharedPtr &operator=(SharedPtr &other) {
    auto tmp_ptr = SharedPtr<T>(other);
    swap(tmp_ptr);
    return *this;
  }

  SharedPtr(SharedPtr &&other): control_block_(other.control_block_), ptr_(other.ptr_) {
    other.control_block_ = nullptr;
    other.ptr_ = nullptr;
  }

  SharedPtr &operator=(SharedPtr &&other) {
    SharedPtr<T>(std::move(other)).swap(*this);
    return *this;
  }
  template <typename Deleter>
  SharedPtr(T *ptr, const Deleter &del):
      SharedPtr(ptr, del, std::allocator<T>()) {}

  SharedPtr(T *ptr) :
      SharedPtr(ptr, std::default_delete<T>(), std::allocator<T>()) {}

  template <typename Deleter, typename Alloc>
  SharedPtr(T *ptr, const Deleter &del, const Allocator &alloc) :
      control_block_(allocate_make(ptr, alloc, del)),
      ptr_(ptr) {
  }

  template <typename Y>
  SharedPtr(Y *ptr) :
      SharedPtr(ptr, std::default_delete<T>(), std::allocator<T>()) {}

  template <typename Y, typename Deleter>
  SharedPtr(Y *ptr, const Deleter &del):
      SharedPtr(ptr, del, std::allocator<T>()) {}

  template <typename Y, typename Deleter, typename Alloc>
  SharedPtr(Y *ptr, const Deleter &del, const Allocator &alloc) :
      control_block_(allocate_make(ptr, alloc, del)),
      ptr_(ptr) {
  }

  SharedPtr(const SharedPtr &other) : control_block_(other.cb_), ptr_(other.ptr_) {
    if (control_block_) {
      shared_counter();
    }
  }

  SharedPtr& operator=(std::nullptr_t) {
    control_block_ = nullptr;
    return *this;
  }

  SharedPtr& operator=(const SharedPtr& shar) {
    SharedPtr copy = shar;
    copy.swap(*this);
    return *this;
  }

  SharedPtr(SharedPtr&& shar) {
    control_block_ =
        new ControlBlockRegular<T, std::default_delete<T>, std::allocator<T>>(
            shar.control_block_, std::default_delete<T>(), std::allocator<T>());
    shar.control_block_ = nullptr;
  }

  template<typename Y>
  SharedPtr(const SharedPtr<Y> &other): control_block_(other.cb_), ptr_(other.ptr_) {
    shared_counter();
  }

  SharedPtr& operator=(SharedPtr&& shar) {
    SharedPtr copy = std::move(shar);
    copy.swap(*this);
    return *this;
  }

  template<typename Y>
  SharedPtr(const SharedPtr<Y> &other): control_block_(other.cb_), ptr_(other.ptr_) {
    shared_counter();
  }

  template<typename Y>
  SharedPtr &operator=(SharedPtr<Y> &other) {
    auto tmp_ptr = SharedPtr<T>(other);
    swap(tmp_ptr);
    return *this;
  }

  template<typename Y>
  SharedPtr(SharedPtr<Y> &&other): cb_(other.cb_), ptr_(other.ptr_) {
    other.cb_ = nullptr;
    other.ptr_ = nullptr;
  }

  template<typename Y>
  SharedPtr &operator=(SharedPtr<Y> &&other) {
    SharedPtr<T>(std::move(other)).swap(*this);
    return *this;
  }



  ~SharedPtr() {
    control_block_->shared_count--;
    if (!control_block_->shared_count) {
      control_block_->use_deleter();
    }
    if (!control_block_->shared_count && !control_block_->weak_count) {
      control_block_->destroy();
    }
  }

  void reset() { *this = SharedPtr(); }

  template <typename U>
  void reset(U* pointer) {
    *this = SharedPtr(pointer);
  }

  size_t use_count() const {
    return (control_block_ ? control_block_->shared_count : 0);
  }

  T* get() const {
    return control_block_ ? control_block_->get_ptr() : nullptr;
  }

  T* operator->() const { return get(); }

  T& operator*() const { return *get(); }

 private:
  struct BaseControlBlock {
    BaseControlBlock() = default;
    virtual T* get_ptr() noexcept = 0;
    virtual void dispose() noexcept = 0;
    virtual void destroy() noexcept = { delete this }
    virtual ~BaseControlBlock() = default;

    size_t shared_count;
    size_t weak_count;
  };
  template <typename Y, typename Deleter, typename Alloc>
  struct ControlBlockRegular : BaseControlBlock {
    T* get_ptr() override { return &ptr; }
    void dispose() override {
      using AllocTraits = std::allocator_traits<Allocator>;
      AllocTraits::destroy(alloc, &obj);
    }

    void destroy() noexcept override {
        auto tmp_alloc =
        typename std::allocator_traits<Allocator>::template rebind_alloc<
        ControlBlockMakeShared>(alloc);
        tmp_alloc.deallocate(this, 1);
    }

    T* ptr;
    Deleter deleter;
    Alloc alloc;
  };
  template <typename T, typename Alloc>
  struct ControlBlockMakeShared : BaseControlBlock {
    template <typename... Args>
    ControlBlockMakeShared(Alloc allocator, Args&&... args)
        : object(std::forward<Args>(args)...), alloc(allocator) {}
    T* get_ptr() override { return reinterpret_cast<T&>(object); }

    void use_deleter() override { deleter_(object); }

    void destroy() override {
      /*block_alloc_type temp = block_alloc;
      alloc.~Alloc();
      block_alloc.~block_alloc_type();
      block_alloc_traits::deallocate(temp, this, 1);*/
    }

    alignas(T) char object[sizeof(T)];
    Alloc alloc;
  };

  template <typename U, typename... Args>
  SharedPtr<U> make_shared(Args&&... args);

  template <typename U, typename Alloc, typename... Args>
  SharedPtr<U> allocate_shared(const Alloc& alloc, Args&&... args);

  void shared_counter() {
    if (control_block_) {
      control_block_->shared_count++;
    }
  }
  void weak_counter() {
    if (control_block_) {
      control_block_->weak_count++;
    }
  }
  void swap(SharedPtr& shar) { std::swap(control_block_, shar.cb); }

  template<typename Allocator, typename Deleter>
  auto allocate_make(T *arg_ptr, const Allocator &alloc, const Deleter &del) {
    auto cont_alloc =
        typename std::allocator_traits<Allocator>::template rebind_alloc<
    ControlBlockRegular<T, Allocator, Deleter>>(alloc);
    auto ptr = cont_alloc.allocate(1);
    new(ptr) ControlBlockDirect<T, Allocator, Deleter>(1,
                                                       0,
                                                       arg_ptr,
                                                       alloc,
                                                       del);
    return ptr;
  }

  BaseControlBlock* control_block_;
  T* ptr_;
};

template <typename U, typename... Args>
SharedPtr<U> MakeShared(Args&&... args) {
  return AllocShared<U>(std::allocator<U>(), std::forward<Args>(args)...);
}

template <typename U, typename Alloc, typename... Args>
SharedPtr<U> AllocateShared(const Alloc& alloc, Args&&... args) {
  using alloc_traits = std::allocator_traits<Alloc>;
  using control_block_type = typename alloc_traits::template rebind_alloc<
      SharedPtr<U>::template ControlBlockMakeShared<Alloc>>;
  using alloc_control_bloc_traits =
      typename alloc_traits::template rebind_traits<
          SharedPtr<U>::template ControlBlockMakeShared<Alloc>>::other;
  ;
  control_block_type new_alloc(alloc);
  control_block_type control_block =
      alloc_control_bloc_traits::allocate(new_alloc, 1);
  try {
    alloc_control_bloc_traits::construct(new_alloc, control_block,
                                         std::forward<Args>(args)...);
    control_block->shared_count = 1;
    control_block->weak_count = 0;

    return SharedPtr<U>(new_alloc, control_block);
  } catch (...) {
    alloc_control_bloc_traits::deallocate(new_alloc, control_block, 1);
    throw;
  }
}

template <typename T>
class WeakPtr {
 public:
  WeakPtr() : wptr_{} {};

  template <typename U>
  WeakPtr(const WeakPtr<U>& other) : wptr_{other.wptr_} {
    SharedPtr<T>::weak_counter();
  }

  template <typename U>
  WeakPtr& operator=(const WeakPtr<U>& weak) {
    WeakPtr copy = weak;
    std::swap(copy, weak);
    return *this;
  }

  template <typename U>
  WeakPtr(const SharedPtr<U>& ptr) : wptr_(ptr.control_block_) {
    SharedPtr<T>::weak_counter();
  }
  template <typename U>
  WeakPtr(WeakPtr<U>&& other) : wptr_(other.wptr_) {
    other.wptr_ = nullptr;
  }

  template <typename U>
  WeakPtr& operator=(WeakPtr<U>&& weak) {
    WeakPtr copy = std::move(weak);
    std::swap(copy, weak);
    return *this;
  }

  WeakPtr(const WeakPtr& other) : wptr_{other.wptr_} {
    SharedPtr<T>::weak_counter();
  }

  WeakPtr& operator=(const WeakPtr& weak) {
    WeakPtr copy = weak;
    std::swap(copy, weak);
    return *this;
  }

  WeakPtr(const SharedPtr<T>& ptr) : wptr_(ptr.control_block_) {
    SharedPtr<T>::weak_counter();
  }
  WeakPtr(WeakPtr&& other) : wptr_(other.wptr_) { other.wptr_ = nullptr; }

  WeakPtr& operator=(WeakPtr&& weak) {
    WeakPtr copy = std::move(weak);
    std::swap(copy, weak);
    return *this;
  }

  void reset() { *this = WeakPtr<T>(); }

  ~WeakPtr() {
    if (wptr_) {
      wptr_->weak_count--;
      if (wptr_->shared_count == 0 && wptr_->weak_count == 0) {
        wptr_->destroy();
      }
    }
  }

  bool expired() const noexcept {
    return (wptr_ == nullptr || wptr_->shared_count == 0);
  }

  SharedPtr<T> lock() const noexcept {
    if (expired()) {
      return SharedPtr<T>{};
    }
    return SharedPtr<T>{wptr_, (wptr_->get_ptr())};
  }

 private:
  template <typename U>
  friend class SharedPtr;

  template <typename U>
  friend class WeakPtr;

  typename SharedPtr<T>::BaseControlBlock* wptr_ = nullptr;
};
