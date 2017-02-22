#include <iostream>
#include <string>

class QueueException {
    public:
	QueueException(std::string msg) { reason = msg; };
	~QueueException() {};
	std::string reason;
};

// These two forward declaration are the best practice for templated friends.
template <class TYPE, int SIZE>
class Queue;

template <class TYPE, int SIZE>
std::ostream& operator<<(std::ostream&, const Queue<TYPE, SIZE>&);



template <class TYPE, int SIZE>
class Queue {
    private:
    protected:
	TYPE items[SIZE];
	int first, last;
    public:
	Queue() { first = 1; last = 0; };
	~Queue() {};

	Queue(const Queue&) {};			// No copy initialization.
	Queue& operator=(const Queue&) {};	// No assignment.

	void enqueue(TYPE item);
	Queue<TYPE, SIZE>& operator+=(const TYPE& item);
	void dequeue();
	TYPE front();

	bool empty() const { return first == (last + 1) % SIZE; };
	bool full() const { return first == (last + 2) % SIZE; };

	friend std::ostream& operator<< <TYPE, SIZE> (std::ostream&, const Queue&);
};

template <class TYPE, int SIZE>
std::ostream& operator<<(std::ostream& stream, const Queue<TYPE, SIZE>& queue) {
	if (!queue.empty()) {
		for (int i = queue.first; i != (queue.last + 1) % SIZE; i = (i + 1) % SIZE) {
			stream << queue.items[i] << "\n";
		}
	}
	return stream;
}

template <class TYPE, int SIZE>
void Queue<TYPE, SIZE>::enqueue(TYPE item) {
	if (!full()) {
		last = (last + 1) % SIZE;
		items[last] = item;
	} else {
		throw QueueException("Can't enqueue to a full Queue.");
	}
}

template <class TYPE, int SIZE>
Queue<TYPE, SIZE>& Queue<TYPE, SIZE>::operator+=(const TYPE& item) {
	if (!full()) {
		last = (last + 1) % SIZE;
		items[last] = item;
		return *this;
	} else {
		throw QueueException("Can't enqueue to a full Queue.");
	}
}

template <class TYPE, int SIZE>
void Queue<TYPE, SIZE>::dequeue() {
	if (!empty()) {
		first = (first + 1) % SIZE;
	} else {
		throw QueueException("Can't dequeue an empty Queue.");
	}
}

template <class TYPE, int SIZE>
inline TYPE Queue<TYPE, SIZE>::front() {
	if (empty()) {
		throw QueueException("Can't front an empty Queue.");
	}
	return items[first];
}

int main(int argc, char* argv[]) {
	Queue<int, 32> intQ;
	const int SIZE = 24;

	try {
		for (int i = 0; !intQ.full() ; i++) {
			std::cout << "Adding " << i << " to the queue.\n";
			intQ.enqueue(i);
		}
		for (int i = 0; i < SIZE * SIZE; i++) {
			int temp = intQ.front();
			intQ.dequeue();
			std::cout << (intQ += temp).front() << "\n";
		}
		std::cout << "Here are the contents of the queue:\n";
		std::cout << intQ;
		std::cout << "And here they are again:\n";
		while (!intQ.empty()) {
			std::cout << "\t" << intQ.front() << "\n";
			intQ.dequeue();
		}
	} catch (QueueException ex) {
		std::cout << "EXCEPTION: " << ex.reason << "\n";
	}

	return 0;
}
