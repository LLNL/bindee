// -i operators.in operators.hh --
class Op {
public:
    Op operator+() const;
    Op operator-() const;
    Op& operator*();

    Op operator+(const Op &o) const;
    Op operator-(const Op &o) const;
    Op operator*(double d) const;
    Op operator/(const Op &o) const;

    Op& operator +=(const Op &o);
    Op& operator -=(double d);
    Op& operator *=(double d);
    Op& operator /=(double d);

    bool operator<(double d);
    bool operator<=(double d);
    bool operator>(const Op &o);
    bool operator>=(const Op &o);
    bool operator==(double d);
    bool operator!=(const Op &o);

    friend Op operator*(double d, const Op &);
};
