#include <iostream>
#include <string>

class QueueException {
    public:
	QueueException(std::string msg) { reason = msg; };
	~QueueException() {};
	std::string reason;
};

// These two forward declaration are the best practice for templated friends.
template <class TYPE>
class Queue;

template <class TYPE>
std::ostream& operator<<(std::ostream&, const Queue<TYPE>&);



template <class TYPE>
class Queue {
    private:
	class node {
	    public:
	        TYPE value;
	        node *next;
	};
    protected:
	node *first, *last;
    public:
	Queue() { first = last = NULL; };
	~Queue() {};

	Queue(const Queue&) {};			// No copy initialization.
	Queue& operator=(const Queue&) {};	// No assignment.

	void enqueue(TYPE item);
	Queue<TYPE>& operator+=(const TYPE& item);
	void dequeue();
	TYPE front();

	bool empty() const { return first == NULL; };
	bool full() const {
		node *tmp = new node;
		bool fullHeap = (tmp == NULL);
		if (tmp != NULL) delete tmp;
		return fullHeap;
	};

	friend std::ostream& operator<< <TYPE> (std::ostream&, const Queue&);
};

template <class TYPE>
std::ostream& operator<<(std::ostream& stream, const Queue<TYPE>& queue) {
	typename Queue<TYPE>::node *tmp = queue.first;
	while (tmp != NULL) {
		stream << (TYPE) tmp->value << "\n";
		tmp = tmp->next;
	}
	return stream;
}

template <class TYPE>
void Queue<TYPE>::enqueue(TYPE item) {
	if (!full()) {
		node *tmp = new node;
		(*tmp).value = item;		// Same as "tmp->value = item;"
		tmp->next = NULL;
		if (first == NULL) {
			first = last = tmp;
		} else {
			last = last->next = tmp;
		}
	} else {
		throw QueueException("Can't enqueue to a full Queue.");
	}
}

template <class TYPE>
Queue<TYPE>& Queue<TYPE>::operator+=(const TYPE& item) {
	enqueue(item);
	return *this;
}

template <class TYPE>
void Queue<TYPE>::dequeue() {
	if (!empty()) {
		first = first->next;
	} else {
		throw QueueException("Can't dequeue an empty Queue.");
	}
}

template <class TYPE>
inline TYPE Queue<TYPE>::front() {
	if (empty()) {
		throw QueueException("Can't front an empty Queue.");
	}
	return first->value;
}

int main(int argc, char* argv[]) {
	Queue<int> intQ;
	const int NUM_ELEMENTS = 31;
	const int SIZE = 24;

	try {
		for (int i = 0; i < NUM_ELEMENTS ; i++) {
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
