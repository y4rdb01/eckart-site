#include <iostream>

using namespace std;

// Copies the standard input to the standard output.
int main(int argc, char* argv[]) {
	char c;

	while (!cin.eof()) cout << (char) cin.get();

	return 0;
}
