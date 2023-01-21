#include "big_integer.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int kKbase = 10;
BigInt::BigInt(int64_t number) {
  if (number < 0) {
    this->positive_ = false;
  }
  if (number == 0) {
    this->number_.push_back(0);
    return;
  }
  while (number != 0) {
    this->number_.push_back(std::abs(number % kKbase));
    number /= kKbase;
  }
}

BigInt& BigInt::operator=(const BigInt& r_num) {
  BigInt copy = r_num;
  Swap(copy);
  return *this;
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}
BigInt BigInt::operator++(int) {
  BigInt number = *this;
  *this += 1;
  return number;
}

BigInt BigInt::operator--() { return *this -= 1; }

BigInt BigInt::operator--(int) {
  BigInt copy = *this;
  *this -= 1;
  return copy;
}

BigInt::BigInt(const std::string& str) {
  positive_ = true;
  int start = 0;
  if (str[start] == '-') {
    positive_ = false;
    start++;
  } else if (str[start] == '+') {
    start++;
  }
  for (size_t i = start; i < str.size(); i++) {
    number_.push_back(str[i] - '0');
  }
  std::reverse(number_.begin(), number_.end());
}
BigInt::BigInt(std::vector<int16_t> str) { this->number_ = str; }

std::istream& operator>>(std::istream& inp, BigInt& r_num) {
  std::string input;
  inp >> input;
  r_num = BigInt(input);
  return inp;
}
std::ostream& operator<<(std::ostream& out, const BigInt& r_num) {
  if ((!(r_num.positive_) && r_num != 0)) {
    out << "-";
  }
  for (int i = (r_num.number_.size() - 1); i >= 0; i--) {
    out << r_num.number_[i];
  }
  return out;
}
BigInt BigInt::operator+=(const BigInt& r_num) {
  if (*this >= 0 && r_num >= 0) {
    Sum(r_num, *this);
  } else if (*this >= 0 && r_num < 0) {
    if (*this >= -r_num) {
      Dif(*this, r_num, *this);
    }
    if (*this < -r_num) {
      Dif(r_num, *this, *this);
      this->positive_ = false;
    }
  } else if (*this < 0 && r_num >= 0) {
    if (-*this < r_num) {
      Dif(r_num, *this, *this);
      this->positive_ = true;
    }
    if (-*this > r_num) {
      Dif(*this, r_num, *this);
      this->positive_ = false;
    }
    if (-*this == r_num) {
      Dif(r_num, *this, *this);
      this->positive_ = true;
    }
  } else if (*this < 0 && r_num < 0) {
    Sum(r_num, *this);
    this->positive_ = false;
  }
  return *this;
}
BigInt BigInt::operator+(const BigInt& r_num) const {
  BigInt sum = *this;
  return sum += r_num;
}
BigInt BigInt::operator%(const BigInt& r_num) {
  BigInt mod = *this;
  return (mod - (mod / r_num) * r_num);
}
BigInt BigInt::operator%=(const BigInt& r_num) {
  *this = *this % r_num;
  return *this;
}

BigInt BigInt::operator*=(const BigInt& r_num) {
  if ((this->positive_ && r_num.positive_) ||
      (!this->positive_ && !r_num.positive_)) {
    *this = Mul(r_num, *this);
    return *this;
  }
  *this = -Mul(r_num, *this);
  return *this;
}
BigInt BigInt::operator*(const BigInt& r_num) const {
  BigInt num = *this;
  return num *= r_num;
}
BigInt BigInt::operator/=(const BigInt& r_num) {
  if ((*this >= 0 && r_num >= 0) || (*this < 0 && r_num < 0)) {
    *this = Div(*this, r_num);
    return *this;
  }
  if (*this < 0) {
    *this = -Div((-*this), r_num);
    return *this;
  }
  if (r_num < 0) {
    *this = -Div(*this, -r_num);
    return *this;
  }
  return *this;
}
BigInt BigInt::operator/(const BigInt& r_num) {
  BigInt div = *this;
  return div /= r_num;
}
BigInt BigInt::operator-=(const BigInt& r_num) {
  if (*this >= 0 && r_num >= 0) {
    if (*this >= r_num) {
      Dif(*this, r_num, *this);
    } else if (*this < r_num) {
      Dif(r_num, *this, *this);
      this->positive_ = false;
    }
  } else if (*this >= 0 && r_num < 0) {
    Sum(r_num, *this);
  } else if (*this < 0 && r_num >= 0) {
    Sum(r_num, *this);
    this->positive_ = false;
  } else if (*this < 0 && r_num < 0) {
    if (*this >= r_num) {
      Dif(r_num, *this, *this);
      this->positive_ = true;
    } else if (*this < r_num) {
      Dif(*this, r_num, *this);
      this->positive_ = false;
    }
  }
  return *this;
}
BigInt BigInt::operator-(const BigInt& r_num) {
  BigInt num = *this;
  return num -= r_num;
}

bool BigInt::operator<(const BigInt& r_num) const {
  if (this->positive_ == r_num.positive_) {
    if (number_.size() < r_num.number_.size()) {
      return (positive_);
    }
    if (number_.size() > r_num.number_.size()) {
      return !(positive_);
    }
    for (int i = number_.size() - 1; i >= 0; --i) {
      if (number_[i] < r_num.number_[i]) {
        return (positive_);
      }
      if (number_[i] > r_num.number_[i]) {
        return !(positive_);
      }
    }
    return false;
  }
  if (this->number_.back() == 0 && r_num.number_.back() == 0) {
    return false;
  }
  return (!this->positive_ && r_num.positive_);
}
bool BigInt::operator>=(const BigInt& r_num) const { return !(*this < r_num); }

bool BigInt::operator==(const BigInt& r_num) const {
  return ((*this >= r_num) && (r_num >= *this));
}
bool BigInt::operator!=(const BigInt& r_num) const { return !(*this == r_num); }
bool BigInt::operator>(const BigInt& r_num) const {
  return (*this != r_num) && (*this >= r_num);
}

bool BigInt::operator<=(const BigInt& r_num) const { return !(*this > r_num); }

BigInt BigInt::operator-() const {
  BigInt tmp;
  tmp = *this;
  if (tmp != 0) {
    tmp.positive_ = !positive_;
  }
  return tmp;
}
void BigInt::Dif(const BigInt& l_num, const BigInt& r_num, BigInt& res) {
  int trn = 0;
  size_t min = std::min(l_num.number_.size(), r_num.number_.size());
  size_t max = std::max(l_num.number_.size(), r_num.number_.size());
  for (size_t i = 0; i < min; ++i) {
    int cur_dif = l_num.number_[i] - r_num.number_[i] - trn;
    CurrentDif(cur_dif, trn);
    res.number_[i] = (cur_dif);
  }
  for (size_t i = min; i < max; ++i) {
    if (l_num.number_[i] - trn < 0) {
      if (i >= res.number_.size()) {
        res.number_.push_back(l_num.number_[i] + kKbase - trn);
      } else {
        res.number_[i] = (l_num.number_[i] + kKbase - trn);
      }
      trn = 1;
    } else {
      if (i >= res.number_.size()) {
        res.number_.push_back(l_num.number_[i] - trn);
      } else {
        res.number_[i] = (l_num.number_[i] - trn);
      }
      trn = 0;
    }
  }
  while (res.number_.size() > 1 && res.number_.back() == 0) {
    res.number_.pop_back();
  }
}
void BigInt::Sum(const BigInt& r_num, BigInt& res) {
  int64_t overflow = 0;
  for (size_t i = 0; i < r_num.number_.size(); ++i) {
    if (i == number_.size()) {
      res.number_.push_back((r_num.number_[i] + overflow) % kKbase);
      overflow = (r_num.number_[i] + overflow) / kKbase;
    } else {
      int64_t temp = number_[i] + r_num.number_[i] + overflow;
      res.number_[i] = temp % kKbase;
      overflow = temp / kKbase;
    }
    for (size_t i = r_num.number_.size(); overflow != 0 && i < number_.size();
         ++i) {
      int64_t temp = number_[i] +overflow;
      int64_t temp = number_[i] + overflow;
      number_[i] = temp % kKbase;
      overflow = temp / kKbase;
    }
  }
  if (overflow != 0) {
    res.number_.push_back(overflow);
  }
}
BigInt BigInt::Mul(const BigInt& l_num, const BigInt& r_num) {
  BigInt res;
  int trn;
  res.number_.resize(l_num.number_.size() + r_num.number_.size(), 0);
  for (size_t i = 0; i < l_num.number_.size(); ++i) {
    for (size_t j = 0; j < r_num.number_.size(); ++j) {
      res.number_[i + j] += l_num.number_[i] * r_num.number_[j];
    }
  }
  for (size_t i = 0; i < res.number_.size(); ++i) {
    trn = res.number_[i] / kKbase;
    res.number_[i] %= kKbase;
    if ((i == res.number_.size() - 1) && trn != 0) {
      res.number_.push_back(trn);
    } else {
      if (i + 1 < res.number_.size()) {
        res.number_[i + 1] += trn;
      }
    }
  }
  while (res.number_.size() > 1 && res.number_[res.number_.size() - 1] == 0) {
    res.number_.pop_back();
  }
  return res;
}
BigInt BigInt::Div(BigInt l_num, const BigInt& r_num) {
  BigInt res;
  if (l_num >= r_num) {
    res.number_.resize(l_num.number_.size() - r_num.number_.size() + 1);
    for (int i = res.number_.size() - 1; i >= 0; --i) {
      while (r_num * res <= l_num) {
        res.number_[i]++;
      }
      res.number_[i]--;
    }
  } else {
    res.number_.push_back(0);
  }
  while (res.number_.size() > 1 && res.number_.back() == 0) {
    res.number_.pop_back();
  }
  return res;
}
int BigInt::CurrentDif(int& cur_dif, int& trn) {
  if (cur_dif < 0) {
    cur_dif += kKbase;
    trn = 1;
  } else {
    trn = 0;
  }
  return cur_dif;
}

void BigInt::Swap(BigInt& other) {
  number_.swap(other.number_);
  std::swap(positive_, other.positive_);
}
