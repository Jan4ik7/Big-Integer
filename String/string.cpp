#include "string.hpp"

#include <cstring>
#include <iostream>
#include <vector>

String::String(size_t size, char character)
    : size_(size), capacity_(size), str_(new char[size + 1]) {
  memset(str_, character, size_);
  str_[size_] = '\0';
}

String::String(const char* string)
    : size_(strlen(string)),
      capacity_(size_),
      str_(new char[strlen(string) + 1]) {
  memcpy(str_, string, size_);
  str_[size_] = '\0';
}

String::String(const String& other) : size_(other.size_), capacity_(size_) {
  str_ = new char[size_ + 1];
  memcpy(str_, other.str_, size_);
  str_[size_] = '\0';
}

String& String::operator=(const String& other) {
  if (this == &other) {
    return *this;
  }
  delete[] str_;
  size_ = other.size_;
  capacity_ = size_;
  str_ = new char[size_ + 1];
  memcpy(str_, other.str_, size_);
  str_[size_] = '\0';
  return *this;
}

String::~String() { delete[] str_; }

void String::Clear() {
  size_ = 0;
  str_[size_] = '\0';
}

void String::PushBack(char new_character) {
  if (capacity_ == 0) {
    Reserve(1);
  }
  if (size_ == capacity_) {
    Reserve(2 * capacity_);
  }
  str_[size_++] = new_character;
}

void String::PopBack() {
  if (size_ != 0) {
    str_[--size_] = '\0';
  }
}

void String::Resize(size_t new_size) {
  Reserve(new_size);
  for (size_t i = size_; i < new_size; ++i) {
    str_[i] = '\0';
  }
  size_ = new_size;
  str_[size_] = '\0';
}

void String::Resize(size_t new_size, char character) {
  size_t tmp = size_;
  Resize(new_size);
  if (new_size > tmp) {
    memset(str_ + tmp, character, new_size - tmp);
  }
  str_[new_size] = '\0';
}

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    capacity_ = new_cap;
    char* copy = str_;
    str_ = new char[new_cap + 1];
    if (copy != nullptr) {
      memcpy(str_, copy, size_);
      delete[] copy;
    }
  }
  if (capacity_ != 0) {
    str_[size_] = '\0';
  }
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    char* copy = str_;
    str_ = new char[size_];
    memcpy(str_, copy, size_);
    delete[] copy;
    capacity_ = size_;
  }
}

void String::Swap(String& other) {
  std::swap(other.str_, str_);
  std::swap(other.size_, size_);
  std::swap(other.capacity_, capacity_);
}

char& String::operator[](int index) { return str_[index]; }

const char& String::operator[](int index) const { return str_[index]; }

char& String::Front() { return str_[0]; }

char& String::Back() { return str_[size_ - 1]; }

const char& String::Front() const { return str_[0]; }

const char& String::Back() const { return str_[size_ - 1]; }

bool String::Empty() const { return (size_ == 0); }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

char* String::Data() { return &str_[0]; }

const char* String::Data() const { return &str_[0]; }

bool operator<(const String& lhs, const String& rhs) {
  if ((lhs.str_ == nullptr) || (rhs.str_ == nullptr)) {
    return false;
  }
  for (size_t i = 0; i < (std::min(lhs.size_, rhs.size_)); i++) {
    if (lhs.str_[i] > rhs.str_[i]) {
      return false;
    }
    if (lhs.str_[i] < rhs.str_[i]) {
      return true;
    }
  }
  return (lhs.size_ < rhs.size_);
}
bool operator>(const String& lhs, const String& rhs) { return !(lhs <= rhs); }

bool operator<=(const String& lhs, const String& rhs) {
  return (lhs < rhs || lhs == rhs);
}

bool operator>=(const String& lhs, const String& rhs) { return !(lhs < rhs); }

bool operator==(const String& lhs, const String& rhs) {
  return ((lhs >= rhs) && (rhs >= lhs));
}
bool operator!=(const String& lhs, const String& rhs) { return !(lhs == rhs); }

String& String::operator+=(const String& other) {
  Reserve(size_ + other.size_);
  memcpy(str_ + size_, other.str_, other.size_);
  size_ = capacity_;
  str_[size_] = '\0';
  return *this;
}

String String::operator+(const String& other) const {
  String copy = *this;
  return copy += other;
}
String String::operator*(size_t n) const {
  String copy = *this;
  copy *= n;
  return copy;
}

String& String::operator*=(size_t n) {
  if (n == 0) {
    Resize(0);
    return *this;
  }
  Reserve(size_ * n);
  for (size_t i = 1; i < n; ++i) {
    memcpy(str_ + (i * size_), str_, size_);
  }
  size_ = capacity_;
  str_[size_] = '\0';
  return *this;
}

std::istream& operator>>(std::istream& input, String& other) {
  char character;
  while ((input.get(character)) && (std::isspace(character) == 0)) {
    other.PushBack(character);
  }
  return input;
}
std::ostream& operator<<(std::ostream& out, const String& other) {
  for (size_t i = 0; i < other.Size(); i++) {
    out << other[i];
  }
  return out;
}

std::vector<String> String::Split(const String& delim) const {
  std::vector<String> res;
  String copy = *this;
  String tmp = "";
  for (size_t i = 0; i < copy.size_; ++i) {
    bool equal = true;
    for (size_t j = 0; j < delim.size_; j++) {
      if (copy[i + j] != delim[j]) {
        equal = false;
        break;
      }
    }
    if (equal) {
      res.push_back(tmp);
      tmp = "";
      i += delim.size_ - 1;
    } else {
      tmp.PushBack(copy[i]);
    }
  }
  res.push_back(tmp);
  return res;
}

String String::Join(const std::vector<String>& other) const {
  String res;
  size_t index = 1;
  if (other.empty()) {
    return "";
  }
  res = other[0];
  while (index < other.size()) {
    res += *this + other[index];
    index++;
  }
  return res;
}
