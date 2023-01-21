#include <iostream>
#include <vector>

class Vector {
 public:
  Vector() : x_(0), y_(0){};

  Vector(int64_t xpoint, int64_t ypoint) : x_(xpoint), y_(ypoint){};

  Vector& operator+=(const Vector& r_vec);

  Vector operator+(const Vector& r_vec) const;

  Vector& operator-=(const Vector& r_vec);

  Vector operator-(const Vector& r_vec) const;

  int64_t operator*(const Vector& r_vec) const;

  int64_t operator^(const Vector& r_vec) const;

  Vector& operator*=(int64_t element);

  Vector operator*(int64_t element) const;

  Vector operator-() const;

  int64_t GetX() const;

  int64_t GetY() const;

 private:
  int64_t x_;
  int64_t y_;
};

class IShape;
class Point;
class Segment;
class Line;
class Ray;
class Circle;
class IShape {
 public:
  virtual ~IShape() = default;

  virtual void Move(const Vector&) = 0;

  virtual bool ContainsPoint(const Point&) const = 0;

  virtual bool CrossSegment(const Segment&) const = 0;

  virtual IShape* Clone() = 0;
};

class Point : public IShape {
 public:
  Point() : x_(0), y_(0){};

  ~Point() = default;

  Point(int64_t xpoint, int64_t ypoint) : x_(xpoint), y_(ypoint){};
  int64_t GetX() const;

  int64_t GetY() const;

  void Move(const Vector& vec);

  bool ContainsPoint(const Point& pnt) const;

  bool CrossSegment(const Segment& seg) const;

  Vector operator-(const Point& pnt) const;

  IShape* Clone();

 private:
  int64_t x_;
  int64_t y_;
};
class Segment : public IShape {
 public:
  Segment() = default;

  Segment(Point xpoint, Point ypoint) : a_pt_(xpoint), b_pt_(ypoint){};

  ~Segment() = default;

  Point GetA() const;

  Point GetB() const;

  void Move(const Vector& vec);

  bool ContainsPoint(const Point& c_pt) const;

  bool CrossSegment(const Segment& cd_seg) const;

  IShape* Clone();

 private:
  Point a_pt_;
  Point b_pt_;
};
class Line : public IShape {
 public:
  Line() = default;

  Line(Point xpoint, Point ypoint) : a_pt_(xpoint), b_pt_(ypoint){};

  ~Line() = default;

  int64_t GetA() const;

  int64_t GetB() const;

  int64_t GetC() const;

  void Move(const Vector& vec);

  bool ContainsPoint(const Point& pnt) const;

  bool CrossSegment(const Segment& cd_seg) const;

  IShape* Clone();

 private:
  Point a_pt_;
  Point b_pt_;
};
class Ray : public IShape {
 public:
  Ray() = default;

  Ray(Point xpoint, Point ypoint) : a_pt_(xpoint), b_pt_(ypoint){};

  ~Ray() = default;

  Point GetA() const;

  Vector GetVector() const;

  void Move(const Vector& vec);

  bool ContainsPoint(const Point& c_pt) const;

  bool CrossSegment(const Segment& cd_seg) const;

  IShape* Clone();

 private:
  Point a_pt_;
  Point b_pt_;
};
class Circle : public IShape {
 public:
  Circle() = default;

  Circle(Point xpoint, size_t rad) : o_pt_(xpoint), rad_(rad){};

  ~Circle() = default;

  Point GetCentre() const;

  size_t GetRadius() const;

  void Move(const Vector& vec);

  bool ContainsPoint(const Point& pnt) const;

  bool CrossSegment(const Segment& ab_seg) const;

  IShape* Clone();

 private:
  Point o_pt_;
  int64_t rad_;
  static int64_t SquareDist(const Point& pnt1, const Point& pnt2) {
    return (pnt1.GetX() - pnt2.GetX()) * (pnt1.GetX() - pnt2.GetX()) +
           (pnt1.GetY() - pnt2.GetY()) * (pnt1.GetY() - pnt2.GetY());
  }
};
