/*
Basis for a Simple Lisp interpreter that consists of these key (class) parts:

	Heap
	Run-time Stack
	String Table

NOTE: Remember to compiler with g++.
*/

#include <iostream>
#include <cstring>
#include <cstdlib>

// So things like cin and cout don't have to be qualified.
using namespace std;

// Called when there is a fatal error and Simple Lisp must exit. The error
// message is printed and the program "exited" by throwing an uncaught
// exception.
//
void fatalError(string errorMessage) {
	cerr << "\n\n" << errorMessage;
	cerr << "\n\nSimple Lisp session terminated.\n";
	throw 1;
}

// A "generic" address type for holding an address location in each of the
// primary components of the process memory map (heap, run-time stack, and
// string table). The string table holds constants while both program code
// and data are kept in the heap.
//
class MemoryReference {
    public:
	enum AddrType { HEAP, STRING, NUMBER } addrType;
	int address;

	// This default constructor allows the NIL_PTR in Heap to be
	// declared as a const.
	//
	MemoryReference() {
		addrType = HEAP;
		address = 0;
	}

	MemoryReference(AddrType type, int addr) {
		addrType = type;
		address = addr;
	}

	// Test the equality of 2 memory references.
	//
	friend bool operator==(const MemoryReference&, const MemoryReference&);
};

// A constant nil pointer that will come in handy later on. Declaring it
// within the global scope makes it easier to reference.
//
const MemoryReference NIL_PTR;

// Contains all of the allocated cells for data and programs.
//
class Heap {
    private:
	static int heapSize;


	class ConsCell {
	    public:
		MemoryReference car;
		MemoryReference cdr;

		ConsCell() {}
	};

	// The actual cells of the heap.
	//
	ConsCell *memory;

	// List of unused cells that can be allocated.
	//
	MemoryReference freeList;

	// The number of cells in the freeList. The getCell method
	// must update this as allocations are made.
	//
	int cellsRemaining;

	// Performs a garbage collection. NOT CURRENTLY IMPLEMENTED.
	//
	void doGC() {}

	// To make the Heap a singleton (single instance).
	// 	1) Override the constructor and make it private.
	// 	2) Don't implement initialization (to avoid copies).
	// 	3) Don't implement assignment (to avoid copies).
	//
	Heap() {
		memory = new ConsCell[heapSize];

		freeList = MemoryReference(MemoryReference::HEAP, 1);
		cellsRemaining = heapSize-1;

		// The first cell in the memory is nil.
		//
		memory[0].car = NIL_PTR;
		memory[0].cdr = NIL_PTR;

		// The initial freeList points to the second cell which is
		// a linked list of the rest of the memory.
		//
		freeList = MemoryReference(MemoryReference::HEAP, 1);
		for (int i = 1; i < heapSize-1; i++) {
			memory[i].cdr = MemoryReference(MemoryReference::HEAP, i+1);
		}
		memory[heapSize-1].cdr = NIL_PTR;
	};
	Heap(const Heap&) {}		
	Heap& operator=(const Heap&) {}

    public:
	// Set the size of the heap.
	//
	static void setSize(int size) {
		heapSize = size;
	}

	// Used to get single instance of the heap, so I don't have to keep
	// passing it around for the i/o operators "<<" and ">>".
	//
	static Heap* getHeap() {
		static Heap* heap = new Heap();
		return heap;
	}

	// Returns a memory reference to a new cons cell.
	//
	MemoryReference getCell();

	// Sets the car of the cons cell indicated by the first argument to
	// the value indicated by the second.
	//
	void setCar(MemoryReference cell, MemoryReference value);

	// Sets the cdr of the cons cell indicated by the first argument to
	// the value indicated by the second.
	//
	void setCdr(MemoryReference cell, MemoryReference value);

	// Returns the car of the indicated cons cell.
	//
	MemoryReference getCar(MemoryReference cell);

	// Returns the cdr of the indicated cons cell.
	//
	MemoryReference getCdr(MemoryReference cell);
};

// Each "frame" on the run-time stack represents a function invocation
// (whether builtin or user defined). The run-time stack represents the
// current state of the computation.
//
class RunTimeStack {
    private:
	static const int MAX_STACK_SIZE = 32;

	MemoryReference stack[MAX_STACK_SIZE];
	int stackTop;

    public:
	RunTimeStack() {
		stackTop = -1;
	}

	// Evaluates the top item on the stack, replacing it with the
	// result.
	//
	void eval();

	// Push an item onto the stack.
	//
	inline void push(MemoryReference memRef) {
		if (MAX_STACK_SIZE-1 == stackTop) {
			fatalError("INTERNAL ERROR: Run-time stack overflow.");
		} else {
			stack[++stackTop] = memRef;
		}
	}

	// Take the top item off the stack and return it.
	//
	inline MemoryReference pop() {
		if (stackTop < 0) {
			fatalError("INTERNAL ERROR: Run-time stack underflow.");
		}
		return stack[stackTop--];
	}
};

// Keeps track of all the named symbols in the system. This includes all of
// the builtin Lisp functions, constants (e.g., t, nil), and user supplied
// non-numeric atoms (which appear as part of data as well as user defined
// parameter and function names.
//
class StringTable {
    public:
	const static int MAX_TOKEN_SIZE = 128;

    private:
	const static int MAX_SIZE = 1024;
	char strings[MAX_SIZE];
	int nextFree;

	// To make the StringTable a singleton (single instance).
	// 	1) Override the constructor and make it private.
	// 	2) Don't implement initialization (to avoid copies).
	// 	3) Don't implement assignment (to avoid copies).
	//
	StringTable() {
		nextFree = 0;
		strcpy(&strings[nextFree], "t\0");
		nextFree += 2;
		strcpy(&strings[nextFree], "nil\0");
		nextFree += 4;
		strcpy(&strings[nextFree], "quote\0");
		nextFree += 6;
		strcpy(&strings[nextFree], "eval\0");
		nextFree += 5;
		strcpy(&strings[nextFree], "write\0");
		nextFree += 6;
		strcpy(&strings[nextFree], "+\0");
		nextFree += 2;
	};
	StringTable(const StringTable&) {}
	StringTable& operator=(const StringTable&) {}

    public:
	// Used to get single instance of the string table, so I don't have
	// to keep passing it around for the i/o operators "<<" and ">>".
	//
	static StringTable* getStringTable() {
		static StringTable* stringTable = new StringTable();
		return stringTable;
	}

	// Saves a new string into the character array and returns a
	// MemoryReference for it. If the string is already contained in
	// the array, then the MemoryReference for the existing entry is
	// returned instead (and no new array entry created).
	//
	MemoryReference saveString(string str);

	// Returns the string for the indicated symbol. The empty string
	// is returned if the symbol doesn't exist.
	//
	string getString(MemoryReference symbol);
};

// Convenient constants for known Lisp functions and other important values.
//
static const MemoryReference T(MemoryReference::STRING, 0);
static const MemoryReference NIL(MemoryReference::STRING, 2);
static const MemoryReference QUOTE(MemoryReference::STRING, 6);
static const MemoryReference EVAL(MemoryReference::STRING, 12);
static const MemoryReference WRITE(MemoryReference::STRING, 17);
static const MemoryReference PLUS(MemoryReference::STRING, 23);

// Reads the next atom, number, or list from the cin InputStream.
//
istream& operator>>(istream& in, MemoryReference& obj);

// Prints the given item to cout.
//
ostream& operator<<(ostream& out, const MemoryReference& obj);

// Home of the reader-evaluation-printer loop that constitutes the user
// interface.  The arguments to main are currently ignored. Zero (0) is
// returned to indicate a normal (expected) program exit (for Posix systems).
//
int main(int argc, char *argv[]) {
	// Process any command line arguments.
	//
	int size = 2048; // Default size of the heap.
	for (int i = 1; i < argc; i++) {
		if (argv[i] == "-hs") {
			// Set the heap size.
			size = atoi(argv[++i]);
		} else {
			cerr << "Unrecognized command line argument to '"
				<< argv[0] << "': "
				<< argv[i] << "\n";
		}
	}

	Heap::setSize(size);

	// The main read-eval-print loop that defines the user interface.
	//
	try {
		int commandCount = 1;
		RunTimeStack* rts = new RunTimeStack();
		cout << "Welcome to Simple Lisp\n\n";
		while (!cin.eof()) {
			MemoryReference input;
			cout << commandCount++ << "> ";
			cin >> input;
			if (cin.eof()) break;
			rts->push(input);
			rts->eval();
			cout << rts->pop();
			cout << "\n";
		}
		cout << "\n\nThanks for using the Simple Lisp system.\n";
		return 0;
	} catch(int exceptionCode) {
		return exceptionCode;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Below are the implementations for all of the above class members.

// Test the equality of 2 memory references.
//
bool operator==(const MemoryReference& lhs, const MemoryReference& rhs) {
	return lhs.addrType == rhs.addrType && lhs.address == rhs.address;
}

// Declare the static variable from the class.
//
int Heap::heapSize;

// Returns a memory reference to a new cons cell.
//
MemoryReference Heap::getCell() {
	MemoryReference consCell = freeList;

	if (cellsRemaining == 0) {
		// Perform a garbage collection cycle.
		//
		doGC();

		// If there's still nothing left after a GC, then "crash".
		//
		if (cellsRemaining == 0) {
			fatalError("Heap space exhausted.");
		}
	}

	// Allocate a cons cell from the freeList.
	//
	cellsRemaining--;
	freeList = memory[freeList.address].cdr;
	return consCell;
}

// Sets the car of the cons cell indicated by the first argument to the
// value indicated by the second.
//
void Heap::setCar(MemoryReference cell, MemoryReference value) {
	// Run-time type check.
	//
	if (MemoryReference::HEAP != cell.addrType) {
		fatalError("INTERNAL ERROR: Attempted to set the car of an atom.");
	}

	memory[cell.address].car = value;
}

// Sets the cdr of the cons cell indicated by the first argument to the
// value indicated by the second.
//
void Heap::setCdr(MemoryReference cell, MemoryReference value) {
	// Run-time type check.
	//
	if (MemoryReference::HEAP != cell.addrType) {
		fatalError("INTERNAL ERROR: Attempted to set the cdr of an atom.");
	}

	memory[cell.address].cdr = value;
}

// Returns the car of the indicated cons cell.
//
MemoryReference Heap::getCar(MemoryReference cell) {
	// Run-time type check.
	//
	if (MemoryReference::HEAP != cell.addrType) {
		// This should really just terminate the current computation
		// by popping everything off the stack - rather than quitting.
		//
		fatalError("ERROR: Attempted to access the car of an atom.");
	}

	return memory[cell.address].car;
}

// Returns the cdr of the indicated cons cell.
//
MemoryReference Heap::getCdr(MemoryReference cell) {
	// Run-time type check.
	//
	if (MemoryReference::HEAP != cell.addrType) {
		// This should really just terminate the current computation
		// by popping everything off the stack - rather than quitting.
		//
		fatalError("ERROR: Attempted to access the cdr of an atom.");
	}

	return memory[cell.address].cdr;
}

// Saves a new string into the character array and returns a MemoryReference
// for it. If the string is already contained in the array, then the
// MemoryReference for the existing entry is returned instead (and no new
// array entry created).
//
MemoryReference StringTable::saveString(string str) {
	// Determine whether or not the string is already in the table.
	//
	for (int i=0; i < nextFree; ) {
		if (strcmp(&strings[i], str.c_str()) == 0) {
			// String is already in the table.
			//
			return MemoryReference(MemoryReference::STRING, i);
		} else while (strings[i++] != '\0');
	}

	// Should only get here if the string is NOT already in the table.
	// So, save the string and return a reference to it.
	//
	int oldNextFree = nextFree;
	nextFree += str.length();
	if (nextFree >= MAX_SIZE) {
		fatalError("INTERNAL ERROR: String Table size exceeded.");
	}
	strcpy(&strings[oldNextFree], str.c_str());
	strings[nextFree++] = '\0';
	return MemoryReference(MemoryReference::STRING, oldNextFree);
}

// Returns the string for the indicated symbol. The empty string is returned
// if the symbol doesn't exist.
//
string StringTable::getString(MemoryReference symbol) {
	if (MemoryReference::STRING == symbol.addrType
			&& symbol.address >= 0
			&& symbol.address < nextFree
			&& '\0' == strings[symbol.address-1]) {
		return (string) &strings[symbol.address];
	} else {
		return "";
	}
}

// Consume any comments and whitespace before returning.
//
void skipCommentsAndWhiteSpace(istream& in) {
	while (1) {
		char c = in.peek();
		if (' ' == c || '\t' == c || '\n' == c) {
			in.get();
		} else if (';' == c) {
			while ('\n' != in.get());
		} else {
			break;
		}
	}
}

// Reads the next atom, number, or list from the cin InputStream.
//
istream& operator>>(istream& in, MemoryReference& obj) {
	skipCommentsAndWhiteSpace(in);
	char c = in.peek();
	if (in.eof()) {
		// End of file, so do nothing.
	} else if ('0' <= c && c <= '9') {
		// Next input is a number.
		int number;
		in >> number;
		obj = MemoryReference(MemoryReference::NUMBER, number);
	} else if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('"' == c)) {
		// Next input is an atom (or a double quoted string).
		char text[StringTable::MAX_TOKEN_SIZE];
		int i = 0;
		bool quotedString = ('"' == c);
		if (quotedString) in >> c;	// Quotes are part of value. 
		while (true) {
			c = in.peek();
			if ('"' == c || '(' == c || ')' == c || '.' == c) {
				break;
			} else if (!quotedString && ' ' == c) {
				break;
			} else {
				in >> c;
				text[i++] = c;
			}
			if (StringTable::MAX_TOKEN_SIZE-1 == i) {
				fatalError("INPUT ERROR: Size of atom/string exceeds MAX_TOKEN_SIZE.");
			}
		}
		text[i] = '\0';
		if (quotedString) in >> c;	// Quotes are part of value. 

		obj = StringTable::getStringTable()->saveString((string) text);
	} else if ('+' == c || '-' == c || '*' == c || '/' == c) {
		// Special operation symbols.
		in >> c;
		obj = StringTable::getStringTable()->saveString(string(1, c));
	} else if ('(' == c) {
		// Next input is a left parenthesis. Consume the parenthesis
		// and read items recursively until the corresponding right
		// parenthesis is seen, then stop reading.
		in >> c;
		skipCommentsAndWhiteSpace(in);
		if (')' == in.peek()) {
			// This is a special case (empty list) so return NIL.
			in >> c;
			obj = NIL_PTR;
		} else if ('.' == in.peek()) {
			fatalError("INPUT ERROR: Unexpected '( .', need intervening item.");
		} else {
			MemoryReference item = obj = Heap::getHeap()->getCell(), nextItem;
			while (true) {
				in >> nextItem;
				Heap::getHeap()->setCar(item, nextItem);

				// What's the remainder the list?
				skipCommentsAndWhiteSpace(in);
				c = in.peek();
				if (')' == c) {
					// Last item in list.
					in >> c;
					Heap::getHeap()->setCdr(item, NIL_PTR);
					break;
				} else if ('.' == in.peek()) {
					// Dotted pair.
					in >> c;
					in >> nextItem;
					Heap::getHeap()->setCdr(item, nextItem);

					skipCommentsAndWhiteSpace(in);
					if (')' != in.peek()) {
						fatalError("INPUT ERROR: Dotted pair has more than one item for the cdr.");
					} else {
						in >> c;
					}
					break;
				} else {
					// Multi-item list.
					nextItem = Heap::getHeap()->getCell();
					Heap::getHeap()->setCdr(item, nextItem);
					item = nextItem;
					skipCommentsAndWhiteSpace(in);
				}
			}
		}
	} else {
		fatalError("INPUT ERROR: Unexpected input character '"
				+ string(1, c) + "'.");
	}
	return in;
}

// Prints the given item to cout.
//
ostream& operator<<(ostream& out, const MemoryReference& obj) {
	if (MemoryReference::STRING == obj.addrType) {
		out << StringTable::getStringTable()->getString(obj);
	} else if (MemoryReference::NUMBER == obj.addrType) {
		out << obj.address;
	} else if (MemoryReference::HEAP == obj.addrType) {
		// The obj must be a list, which will no doubt need to be
		// printed recursively.
		//

		// STUDENT CODE NEEDED HERE!!!!!

	} else {
		fatalError("INTERNAL ERROR: Attempted to print an illegal value.");
	}
	return out;
}

// Evaluates the given Lisp structure on top of the stack and replaces it
// with the result.
//
void RunTimeStack::eval() {
	Heap* heap = Heap::getHeap();
	StringTable* strTbl = StringTable::getStringTable();
	MemoryReference top = this->pop();

	// Determine the type of thing to be evaluated.
	//
	if (MemoryReference::NUMBER == top.addrType) {
		this->push(top);
	} else if (MemoryReference::STRING == top.addrType) {
		if (T == top) {
			this->push(top);
		} else if (NIL == top) {
			this->push(NIL_PTR);
		} else {
			string str = strTbl->getString(top);

			// The string must name a variable whose value I
			// need to find on the run-time stack. If the value
			// can't be found, then call "fatalError".
			//

			// STUDENT CODE NEEDED HERE!!!!!
		}
	} else if (NIL_PTR == top) {
		// This is nil.
		//
		this->push(NIL_PTR);
	} else if (MemoryReference::HEAP == top.addrType) {
		// This should be a function call, either a builtin function
		// or one that was previously defined using "defun".
		//
		MemoryReference fun = heap->getCar(top);

		if (QUOTE == fun) {
			this->push(heap->getCar(heap->getCdr(top)));
		} else if (EVAL == fun) {
			this->push(heap->getCar(heap->getCdr(top)));
			this->eval();
		} else if (WRITE == fun) {
			this->push(heap->getCar(heap->getCdr(top)));
			this->eval();
			MemoryReference value = this->pop();
			cout << value;
			this->push(value);
		} else if (PLUS == fun) {
			this->push(heap->getCar(heap->getCdr(top)));
			this->eval();
			this->push(heap->getCar(heap->getCdr(heap->getCdr(top))));
			this->eval();

			MemoryReference arg2 = this->pop();
			MemoryReference arg1 = this->pop();

			if (MemoryReference::NUMBER != arg1.addrType
					|| MemoryReference::NUMBER != arg2.addrType) {
				fatalError("ILLEGAL ARITHMETIC: Attempted to add one or more non-numbers.");
			}
			this->push(MemoryReference(MemoryReference::NUMBER,
						   arg1.address + arg2.address));
		} else if (MemoryReference::STRING == fun.addrType) {
			// Must be a user defined function.
			//

			// STUDENT CODE NEEDED HERE!!!!!

		} else if (MemoryReference::HEAP == fun.addrType) {
			// Must be a lambda function.
			//

			// STUDENT CODE NEEDED HERE!!!!!

		} else {
			fatalError("UNKNOWN FUNCION: Function '"
				+ StringTable::getStringTable()->getString(fun)
				 + "' not defined.");
		} 
	}

	return;
}
