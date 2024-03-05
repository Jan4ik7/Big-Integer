#pragma once
#include <cstddef>
#include <iostream>
#include <vector>

class String {
 public:
  String() = default;

  String(size_t size, char character);

  String(const char* string);

  String(const String& other);

  String& operator=(const String& other);

  ~String();

  void Clear();

  void PushBack(char new_character);

  void PopBack();

  void Resize(size_t new_size);

  void Resize(size_t new_size, char character);

  void Reserve(size_t new_cap);

  void ShrinkToFit();

  void Swap(String& other);

  char& operator[](int index);

  const char& operator[](int index) const;

  char& Front();

  char& Back();

  const char& Front() const;

  const char& Back() const;

  bool Empty() const;

  size_t Size() const;

  size_t Capacity() const;

  char* Data();

  const char* Data() const;

  friend bool operator<(const String& lhs, const String& rhs);

  friend bool operator>(const String& lhs, const String& rhs);

  friend bool operator<=(const String& lhs, const String& rhs);

  friend bool operator>=(const String& lhs, const String& rhs);

  friend bool operator==(const String& lhs, const String& rhs);

  friend bool operator!=(const String& lhs, const String& rhs);

  String& operator+=(const String& other);

  String operator+(const String& other) const;

  String operator*(size_t n) const;

  String& operator*=(size_t n);

  std::vector<String> Split(const String& delim = " ") const;

  String Join(const std::vector<String>& other) const;

 private:
  char* str_ = nullptr;
  size_t size_ = 0;
  size_t capacity_ = 0;
};
std::istream& operator>>(std::istream& input, String& other);
std::ostream& operator<<(std::ostream& out, const String& other);
