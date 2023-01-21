#include "geometry.hpp"

#include <iostream>
#include <vector>

Vector& Vector::operator+=(const Vector& r_vec) {
  x_ += r_vec.x_;
  y_ += r_vec.y_;
  return *this;
}

Vector& Vector::operator-=(const Vector& r_vec) {
  x_ -= r_vec.x_;
  y_ -= r_vec.y_;
  return *this;
}

Vector Vector::operator+(const Vector& r_vec) const {
  return Vector(*this) += r_vec;
}

Vector Vector::operator-(const Vector& r_vec) const {
  return Vector(*this) -= r_vec;
}

Vector& Vector::operator*=(int64_t element) {
  x_ *= element;
  y_ *= element;
  return *this;
}

Vector Vector::operator*(int64_t element) const {
  return Vector(*this) *= element;
}

int64_t Vector::operator*(const Vector& r_vec) const {
  return (this->x_ * r_vec.x_ + this->y_ * r_vec.y_);
}

int64_t Vector::operator^(const Vector& r_vec) const {
  return ((r_vec.y_ * this->x_) - (r_vec.x_ * this->y_));
}
Vector Vector::operator-() const { return Vector(-x_, -y_); }
int64_t Vector::GetX() const { return x_; }
int64_t Vector::GetY() const { return y_; }
int64_t Point::GetX() const { return x_; }
int64_t Point::GetY() const { return y_; }
void Point::Move(const Vector& vec) {
  this->x_ += vec.GetX();
  this->y_ += vec.GetY();
}

bool Point::ContainsPoint(const Point& pnt) const {
  return (x_ == pnt.x_ && y_ == pnt.y_);
}
bool Point::CrossSegment(const Segment& seg) const {
  return seg.ContainsPoint(*this);
}
IShape* Point::Clone() {
  IShape* other = new Point(*this);
  return other;
}
Vector Point::operator-(const Point& pnt) const {
  Vector res(this->GetX() - pnt.GetX(), this->GetY() - pnt.GetY());
  return res;
}

Point Segment::GetA() const { return a_pt_; }

Point Segment::GetB() const { return b_pt_; }
void Segment::Move(const Vector& vec) {
  this->a_pt_.Move(vec);
  this->b_pt_.Move(vec);
}
bool Segment::ContainsPoint(const Point& c_pt) const {
  Vector ab_vec(b_pt_ - a_pt_);
  Vector ac_vec(c_pt - a_pt_);
  Vector ca_vec(a_pt_ - c_pt);
  Vector bc_vec(b_pt_ - c_pt);
  return (((ab_vec ^ ac_vec) == 0) && ((ca_vec * bc_vec) <= 0));
}
bool Segment::CrossSegment(const Segment& cd_seg) const {
  Vector ab_vec(b_pt_ - a_pt_);
  Vector cd_vec(cd_seg.b_pt_ - cd_seg.a_pt_);
  Vector ac_vec(cd_seg.a_pt_ - a_pt_);
  Vector bd_vec(cd_seg.b_pt_ - a_pt_);
  Vector cb_vec(b_pt_ - cd_seg.a_pt_);
  Vector ca_vec(a_pt_ - cd_seg.a_pt_);
  if (ab_vec.GetX() * cd_vec.GetY() == cd_vec.GetX() * ab_vec.GetY()) {
    if (ab_vec.GetX() * ac_vec.GetY() != ac_vec.GetX() * ab_vec.GetY()) {
      return false;
    }
    return (cd_seg.ContainsPoint(a_pt_) || cd_seg.ContainsPoint(b_pt_) ||
            this->ContainsPoint(cd_seg.a_pt_) ||
            this->ContainsPoint(cd_seg.b_pt_));
  }
  return ((((ab_vec ^ bd_vec) * (ab_vec ^ ac_vec)) <= 0) &&
          (((cd_vec ^ ca_vec) * (cd_vec ^ cb_vec)) <= 0));
}
IShape* Segment::Clone() {
  IShape* other = new Segment(*this);
  return other;
}

int64_t Line::GetA() const { return (b_pt_.GetY() - a_pt_.GetY()); }

int64_t Line::GetB() const { return (a_pt_.GetX() - b_pt_.GetX()); }

int64_t Line::GetC() const {
  return ((a_pt_.GetX() * (a_pt_.GetY() - b_pt_.GetY())) +
          ((a_pt_.GetY() * (b_pt_.GetX() - a_pt_.GetX()))));
}
void Line::Move(const Vector& vec) {
  this->a_pt_.Move(vec);
  this->b_pt_.Move(vec);
}
bool Line::ContainsPoint(const Point& pnt) const {
  return (((b_pt_.GetY() - a_pt_.GetY()) * (pnt.GetX() - a_pt_.GetX())) ==
          ((pnt.GetY() - a_pt_.GetY()) * (b_pt_.GetX() - a_pt_.GetX())));
}
bool Line::CrossSegment(const Segment& cd_seg) const {
  Vector ab_vec(b_pt_ - a_pt_);
  Vector ac_vec(cd_seg.GetA() - a_pt_);
  Vector ad_vec(cd_seg.GetB() - a_pt_);
  return (((ab_vec ^ ac_vec) * (ab_vec ^ ad_vec)) <= 0);
}
IShape* Line::Clone() {
  IShape* other = new Line(*this);
  return other;
}

Point Ray::GetA() const { return a_pt_; }

Vector Ray::GetVector() const {
  Vector ray_vec(this->b_pt_.GetX() - this->a_pt_.GetX(),
                 this->b_pt_.GetY() - this->a_pt_.GetY());
  return ray_vec;
}
void Ray::Move(const Vector& vec) {
  this->a_pt_.Move(vec);
  this->b_pt_.Move(vec);
}
bool Ray::ContainsPoint(const Point& c_pt) const {
  Vector ab_vec(b_pt_ - a_pt_);
  Vector ac_vec(c_pt - a_pt_);
  return ((((ab_vec) ^ (ac_vec)) == 0) && (((ab_vec) * (ac_vec)) >= 0));
}
bool Ray::CrossSegment(const Segment& cd_seg) const {
  Line line(this->a_pt_, this->b_pt_);
  if (!(line.CrossSegment(cd_seg))) {
    return false;
  }
  Vector ad_vec(cd_seg.GetB() - a_pt_);
  Vector bc_vec(cd_seg.GetA() - b_pt_);
  Vector bd_vec(cd_seg.GetB() - b_pt_);
  Vector ac_vec(cd_seg.GetA() - a_pt_);
  Vector ab_vec(b_pt_ - a_pt_);
  // r_seg - segment formed by two ray points
  Segment r_seg(a_pt_, b_pt_);
  if (r_seg.CrossSegment(cd_seg)) {
    return true;
  }
  if (((ac_vec ^ ad_vec) == 0) && ((bc_vec ^ bd_vec) == 0)) {
    return (((ac_vec) * (ab_vec) >= 0) || ((ad_vec) * (ab_vec) >= 0));
  }
  return (((ac_vec ^ ad_vec) * (ac_vec ^ ad_vec)) >
          ((bc_vec ^ bd_vec) * (bc_vec ^ bd_vec)));
}

IShape* Ray::Clone() {
  IShape* other = new Ray(*this);
  return other;
}

Point Circle::GetCentre() const { return o_pt_; }
size_t Circle::GetRadius() const { return rad_; }
void Circle::Move(const Vector& vec) { this->o_pt_.Move(vec); }
bool Circle::ContainsPoint(const Point& pnt) const {
  return (((o_pt_.GetX() - pnt.GetX()) * (o_pt_.GetX() - pnt.GetX()) +
           (o_pt_.GetY() - pnt.GetY()) * (o_pt_.GetY() - pnt.GetY())) <=
          rad_ * rad_);
}
bool Circle::CrossSegment(const Segment& ab_seg) const {
  Vector ao_vec(o_pt_ - ab_seg.GetA());
  Vector ob_vec(ab_seg.GetB() - o_pt_);
  Vector ab_vec(ab_seg.GetB() - ab_seg.GetA());
  // Line (that is perpendicular to segment ab_seg) is formed by
  //  two point of our segment ab_seg
  Point seg_o1(ab_seg.GetA().GetY(), ab_seg.GetB().GetX());
  Point seg_o2(ab_seg.GetB().GetY(), ab_seg.GetA().GetX());
  Line per_line(seg_o1, seg_o2);
  // the line is moved to the center of our circle using move method
  // the line is shifted along the radius vector to the circle center
  Vector rad_vec(o_pt_ - seg_o1);
  per_line.Move(rad_vec);
  int64_t dis_ao_vec = SquareDist(this->o_pt_, ab_seg.GetA());
  int64_t dis_bo_vec = SquareDist(this->o_pt_, ab_seg.GetB());
  if ((dis_ao_vec < (this->rad_) * (this->rad_)) &&
      (dis_bo_vec < (this->rad_) * (this->rad_))) {
    return false;
  }
  if ((dis_ao_vec == (this->rad_) * (this->rad_)) ||
      (dis_bo_vec == (this->rad_) * (this->rad_))) {
    return true;
  }
  if ((dis_ao_vec < (this->rad_) * (this->rad_)) &&
      (dis_bo_vec > (this->rad_) * (this->rad_))) {
    return true;
  }
  if ((dis_ao_vec > (this->rad_) * (this->rad_)) &&
      (dis_bo_vec < (this->rad_) * (this->rad_))) {
    return true;
  }
  if (((ao_vec ^ ob_vec) * (ao_vec ^ ob_vec) / ((ab_vec * ab_vec))) >
      rad_ * rad_) {
    return false;
  }
  return per_line.CrossSegment(ab_seg);
}

IShape* Circle::Clone() {
  IShape* other = new Circle(*this);
  return other;
}
