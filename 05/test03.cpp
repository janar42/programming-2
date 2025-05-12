#include <iostream>

class A
{
public:
    A ( void ) { std::cout << *this; }
    virtual ~A ( void ) { }
    virtual void print ( std::ostream & os ) const = 0;
    friend std::ostream & operator << ( std::ostream & os, const A & x )
    { x . print ( os ); return os; }
};

class B : public A
{
public:
    B ( int x ) { m_X = x; }
    virtual void print ( std::ostream & os ) const { os << "B:" << m_X; }
private:
    int m_X;
};

int main ( void )
{
    B test ( 53 );
    return 0;
}