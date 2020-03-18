#include <cstdio>
#include <vector>
#include <cassert>
#include <iostream>

using namespace std;

template<typename T>
class alist 
{
private:
	vector<T> data;
	vector<int> next;
	vector<int> prev;
	
	int size;
	int capacity;
	
	int head;
	int tail;
	
	int sfree;
	int efree;
public:
	alist(int capacity = 32) :
	data(capacity),
	next(capacity),
	prev(capacity),
	size(0),
	capacity(capacity),
	tail(-1),
	head(-1),
	sfree(0),
	efree(capacity - 1)
	{
		for (int i = 0; i < capacity; ++i) {
			next[i] = (i + 1) % capacity;
			prev[i] = (i - 1) % capacity;
		}
	}
	
	void insert(int pos, T val)
	{
		assert(pos <= size);
		assert(size < capacity);
		
		data[sfree] = val;
		
		if (head == -1 && tail == -1) {
			head = tail = sfree;
			sfree = next[sfree];
			
			next[head] = prev[head] = -1;
		}
		else if (pos == 0) {
			int tmp = head;
			
			head = sfree;
			sfree = next[sfree];
			
			prev[tmp] = head;
			
			next[head] = tmp;
			prev[head] = -1;
		}
		else if (pos == size) {
			int tmp = tail;
			
			tail = sfree;
			sfree = next[sfree];
			
			next[tmp] = tail;
			
			next[tail] = -1;
			prev[tail] = tmp;
		}
		else {
			int tnext = head;
			while (pos--) tnext = next[tnext];
			int tprev = prev[tnext];
			
			pos = sfree;	
			sfree = next[sfree];
			
			next[pos] = tnext;
			prev[tnext] = pos;
			
			prev[pos] = tprev;
			next[tprev] = pos;
		}
		
		size++;
	}
	
	void erase(int pos)
	{
		assert(pos < size);
		
		if (size == 1) {
			// remove
			data[head] = -1;
			
			next[head] = sfree;
			sfree = head;
			
			head = tail = -1;
		}
		else if (pos == 0) {
			// remove
			data[head] = -1;
			
			int tmp = head;
			
			head = next[head];
			prev[head] = -1;
			
			next[tmp] = sfree;
			sfree = tmp;
		}
		else if (pos == size - 1) {
			// remove
			data[tail] = -1;
			
			int tmp = tail;
			
			tail = prev[tail];
			next[tail] = -1;
			
			next[tmp] = sfree;
			sfree = tmp;
		}
		else {
			int tmp = head;
			while (pos--) tmp = next[tmp];
			
			// remove
			data[tmp] = -1;
			
			next[prev[tmp]] = next[tmp];
			prev[next[tmp]] = prev[tmp];
			
			next[tmp] = sfree;
			sfree = tmp;
		}
		
		size--;
	}
	
	template<typename U>
	friend ostream& operator<<(ostream& out, const alist<U> list);
};

template<typename U>
ostream& operator<<(ostream& out, const alist<U> list)
{
	out << "Head: " << list.head << endl;
	out << "Tail: " << list.tail << endl;
	
	out << "Data:\t";
	for (int i = 0; i < list.capacity; ++i)
		out << list.data[i] << "\t";
	out << endl;
	
	out << "Next:\t";
	for (int i = 0; i < list.capacity; ++i)
		out << list.next[i] << "\t";
	out << endl;
	
	out << "Prev:\t";
	for (int i = 0; i < list.capacity; ++i)
		out << list.prev[i] << "\t";
	out << endl;
	
	return out;
}

int main()
{
	alist<int> list(5);
	
	list.insert(0, 5);
	list.insert(1, 7);
	list.insert(2, 10);
	
	list.erase(1);
	
	cout << list;
}
