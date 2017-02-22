#include <iostream>
#include <cmath>

using namespace std;

class Complex {
	friend istream& operator>>(istream& in, Complex& c);
	friend ostream& operator<<(ostream& out, const Complex& c);
	friend bool operator==(const Complex& lhs, const Complex& rhs);
	friend bool operator<(Complex& lhs, Complex& rhs);
	friend Complex operator+(const Complex& lhs, const Complex& rhs);

    private:
	float real, imaginary;

    protected:

    public:
	Complex() { real = imaginary = 0; }
	Complex(float r, float i = 0) { real = r; imaginary = i; }
	~Complex() {}

	// Copy constructor (not an assignment)
	Complex(const Complex& source) {
		real = source.real;
		imaginary = source.imaginary;
	}

	Complex& operator=(const Complex& rhs);
	Complex operator++();
	Complex operator++(int unused);
	Complex& operator+=(const Complex& rhs);

	// Implicitly inline since implementation is in class declaration.
	operator float() const { return real; } // Conversion operator

	double measure() {
		return sqrt(real*real + imaginary*imaginary);
	}
};

Complex& Complex::operator=(const Complex& rhs) {
	// Avoid self-assignment.
	if (this != &rhs) {
	 	// If part of Complex was dynamically allocated then these
		// two statements would deallocate the old value.
		//
            	// delete point_to_object; // free resource;
            	// point_to_object = 0;

            	real = rhs.real;
            	imaginary = rhs.imaginary;
        }
        return *this;
}

// Prefix increment operator
inline Complex Complex::operator++() {
	++real;
	return *this;
}

// Postfix increment operator
inline Complex Complex::operator++(int unused) {
	Complex result = *this;
	++(*this);	// Call the prefix operator for Complex
	return result;
}

// Addition assignment operator
inline Complex& Complex::operator+=(const Complex& rhs) {
	this->real += rhs.real;
	this->imaginary += rhs.imaginary;
	return *this;
}

istream& operator>>(istream& in, Complex& c) {
	char ch;
	in >> c.real;
	ch = in.peek();
	if ('+' == ch || '-' == ch) {
		// Read the imaginary part.
		in >> c.imaginary;
		in >> ch;	// Consuming the expected trailing "i".
	}
	return in;
}

ostream& operator<<(ostream& out, const Complex& c) {
	out << c.real;
	if (c.imaginary != 0) {
		out << (c.imaginary > 0 ? "+" : "") << c.imaginary << "i";
	}
	return out;
}

bool operator==(const Complex& lhs, const Complex& rhs) {
	return (lhs.real == rhs.real) && (lhs.imaginary == rhs.imaginary);
}

bool operator<(Complex& lhs, Complex& rhs) {
	return lhs.measure() < rhs.measure();
}

Complex operator+(const Complex& lhs, const Complex& rhs) {
	return Complex(lhs.real + rhs.real, lhs.imaginary + rhs.imaginary);
}

// Declare a new namespace.
namespace complex_comparison {
	void compare(Complex first, Complex second) {
		// Comparison operations.
		if (first < second) {
			cout << "first is less than second\n";
		} else if (first == second) {
			cout << "first is equal to second\n";
		} else {
			cout << "first must be greater than second\n";
		}
	}
}

int main(int argc, char* argv[]) {
	Complex first, second;

	// Input and output examples.
	std::cout << "Enter imaginary number (e.g., 3+4i): ";
	cin >> first;
	cout << "Enter another imaginary number (e.g., 2+3i): ";
	cin >> second;
	cout << "The sum of " << first << " and " << second << " is "
		<< first + second << "\n";

	cout << "Add 3.0 to " << first << " to get " << Complex(3) + first << "\n";
	cout << "The pre increment of " << second << " is ";
	cout << ++second << "\n";

	cout << "The measure of first is " << first.measure() << "\n";
	cout << "The measure of second is " << second.measure() << "\n";

	complex_comparison::compare(first, second);

	return 0;
}
