#pragma once

#include <algorithm>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix() : data_(std::vector<std::vector<T>>(N, std::vector<T>(M, 0))) {}

  explicit Matrix(std::vector<std::vector<T>> my_vec) : data_(my_vec) {}

  Matrix(T elem) : data_(N, std::vector<T>(M, elem)) {}

  Matrix(const Matrix& other) : data_(other.data_) {}

  Matrix& operator=(const Matrix& other) = default;

  ~Matrix() = default;

  Matrix& operator+=(const Matrix& other);

  Matrix operator+(const Matrix& other) const;

  Matrix& operator-=(const Matrix& other);

  Matrix operator-(const Matrix& other) const;

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& other) const;

  Matrix& operator*=(T element);

  Matrix operator*(T element) const;

  bool operator==(const Matrix& other) const { return (data_ == other.data_); }

  T& operator()(size_t row, size_t col) { return data_[row][col]; }

  T operator()(size_t row, size_t col) const { return data_[row][col]; }

  Matrix<M, N, T> Transposed() const;

 private:
  std::vector<std::vector<T>> data_;
};

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() const {
  Matrix<M, N, T> copy;
  for (size_t row = 0; row < N; ++row) {
    for (size_t col = 0; col < M; ++col) {
      copy(col, row) = data_[row][col];
    }
  }
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix<N, M, T>& other) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < M; col++) {
      data_[row][col] += other.data_[row][col];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(const Matrix<N, M, T>& other) const {
  Matrix copy = *this;
  copy += other;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix<N, M, T>& other) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < M; col++) {
      data_[row][col] -= other.data_[row][col];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(const Matrix<N, M, T>& other) const {
  Matrix copy = *this;
  copy -= other;
  return copy;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator*=(T element) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < M; col++) {
      data_[row][col] *= element;
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(T element) const {
  Matrix copy = *this;
  copy *= element;
  return copy;
}

template <size_t N, size_t M, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, M, T>::operator*(const Matrix<M, K, T>& other) const {
  Matrix<N, K, T> copy;
  for (size_t row_n = 0; row_n < N; ++row_n) {
    for (size_t row_k = 0; row_k < K; ++row_k) {
      for (size_t row_m = 0; row_m < M; ++row_m) {
        copy(row_n, row_k) += (*this)(row_n, row_m) * other(row_m, row_k);
      }
    }
  }
  return copy;
}

template <size_t N, typename T>
class Matrix<N, N, T> {
 public:
  Matrix() : data_(std::vector<std::vector<T>>(N, std::vector<T>(N, 0))) {}

  explicit Matrix(std::vector<std::vector<T>> my_vec) : data_(my_vec) {}

  Matrix(T elem) : data_(N, std::vector<T>(N, elem)) {}

  Matrix(const Matrix& other) : data_(other.data_) {}

  Matrix& operator=(const Matrix& other) = default;

  ~Matrix() = default;

  Matrix& operator+=(const Matrix& other);

  Matrix operator+(const Matrix& other) const;

  Matrix& operator-=(const Matrix& other);

  Matrix operator-(const Matrix& other) const;

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<N, K, T>& other) const;

  Matrix& operator*=(T element);

  Matrix operator*(T element) const;

  bool operator==(const Matrix& other) const { return (data_ == other.data_); }

  T& operator()(size_t row, size_t col) { return data_[row][col]; }

  T operator()(size_t row, size_t col) const { return data_[row][col]; }

  Matrix<N, N, T> Transposed() const {
    Matrix<N, N, T> copy;
    for (size_t row = 0; row < N; row++) {
      for (size_t col = 0; col < N; col++) {
        copy(col, row) = data_[row][col];
      }
    }
    return copy;
  }
  T Trace() const;

 private:
  std::vector<std::vector<T>> data_;
};

template <size_t N, typename T>
Matrix<N, N, T>& Matrix<N, N, T>::operator+=(const Matrix<N, N, T>& other) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < N; col++) {
      data_[row][col] += other.data_[row][col];
    }
  }
  return *this;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator+(const Matrix<N, N, T>& other) const {
  Matrix copy = *this;
  copy += other;
  return copy;
}

template <size_t N, typename T>
Matrix<N, N, T>& Matrix<N, N, T>::operator-=(const Matrix<N, N, T>& other) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < N; col++) {
      data_[row][col] -= other.data_[row][col];
    }
  }
  return *this;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator-(const Matrix<N, N, T>& other) const {
  Matrix copy = *this;
  copy -= other;
  return copy;
}

template <size_t N, typename T>
Matrix<N, N, T>& Matrix<N, N, T>::operator*=(T element) {
  for (size_t row = 0; row < N; row++) {
    for (size_t col = 0; col < N; col++) {
      data_[row][col] *= element;
    }
  }
  return *this;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator*(T element) const {
  Matrix copy = *this;
  copy *= element;
  return copy;
}
template <size_t N, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, N, T>::operator*(const Matrix<N, K, T>& other) const {
  Matrix<N, K, T> copy;
  for (size_t row_n = 0; row_n < N; ++row_n) {
    for (size_t row_k = 0; row_k < K; ++row_k) {
      for (size_t row_m = 0; row_m < N; ++row_m) {
        copy(row_n, row_k) += (*this)(row_n, row_m) * other(row_m, row_k);
      }
    }
  }
  return copy;
}
template <size_t N, typename T>
T Matrix<N, N, T>::Trace() const {
  T sum;
  sum = 0;
  sum += data_[0][0];
  for (size_t row = 1; row < N; row++) {
    sum += data_[row][row];
  }
  return sum;
}
