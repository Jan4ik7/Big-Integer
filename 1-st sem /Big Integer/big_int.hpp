#pragma once
#include <string>
#include <vector>

class BigInt {
 public:
  BigInt() = default;

  BigInt(int64_t number);

  BigInt(const BigInt& sec) : positive_(sec.positive_), number_(sec.number_) {}

  BigInt(const std::string& str);

  BigInt(std::vector<int16_t> str);

  friend std::istream& operator>>(std::istream& inp, BigInt& r_num);

  friend std::ostream& operator<<(std::ostream& out, const BigInt& r_num);

  BigInt& operator=(const BigInt& r_num);

  BigInt operator+=(const BigInt& r_num);

  BigInt operator+(const BigInt& r_num) const;

  BigInt operator-=(const BigInt& r_num);

  BigInt operator-(const BigInt& r_num);

  BigInt operator/=(const BigInt& r_num);

  BigInt operator%=(const BigInt& r_num);

  BigInt operator%(const BigInt& r_num);

  BigInt operator/(const BigInt& r_num);

  BigInt operator*=(const BigInt& r_num);

  BigInt operator*(const BigInt& r_num) const;

  bool operator<(const BigInt& r_num) const;

  bool operator>=(const BigInt& r_num) const;

  bool operator==(const BigInt& r_num) const;

  bool operator!=(const BigInt& r_num) const;

  bool operator>(const BigInt& r_num) const;

  bool operator<=(const BigInt& r_num) const;

  BigInt& operator++();

  BigInt operator++(int);

  BigInt operator--();
  BigInt operator--(int);

  BigInt operator-() const;

 private:
  size_t size_ = 0;
  bool positive_ = true;
  std::vector<int16_t> number_;
  static void Dif(const BigInt& l_num, const BigInt& r_num, BigInt& res);

  static BigInt Mul(const BigInt& l_num, const BigInt& r_num);

  static BigInt Div(BigInt l_num, const BigInt& r_num);

  void Sum(const BigInt& r_num, BigInt& res);

  void Swap(BigInt& other);

  static int CurrentDif(int& cur_dif, int& trn);
};
