#include <cstdio>
#include <iostream>
#include <cassert>

using namespace std;

template<typename T>
class dlist
{ 
public:
	class node
	{
		friend class dlist;
	public:
		T data;
		
		node(T _data, node* next, node* prev) :
		data(_data),
		_next(next),
		_prev(prev)
		{
		}
private:
		node* _next;
		node* _prev;
public:
		inline node* next() { return _next; }
		inline node* prev() { return _prev; }
	};
	
private:
	node* _head;
	node* _tail;
	size_t _size;
	
public:
	dlist() :
	_head(nullptr),
	_tail(nullptr),
	_size(0)
	{
	}
	
	~dlist() 
	{
		while (_head != nullptr) {
			_tail = _head->_next;
			
			delete _head;
			
			_head = _tail;
		}
	}
	
	inline T pop_back()
	{
		assert(_size);
		
		T retval = _tail->data;
		
		if (_size > 1) {
			node* tmp = _tail;
			
			_tail = _tail->prev;
			_tail->next = nullptr;
			
			delete tmp;
		}
		else {
			delete _tail;
			
			_tail = _head = nullptr;
		}
		
		_size--;
		
		return retval;	
	}
	
	inline T pop_front()
	{
		assert(_size);
		
		T retval = _head->data;
		
		if (_size > 1) {
			node* tmp = _head;
			
			_head = _head->next;
			_head->prev = nullptr;
			
			delete tmp;
		}
		else {
			delete _head;
			
			_head = _tail = nullptr;
		}
		
		_size--;
		
		return retval;
	}
	
	inline void push_back(const T& val)
	{
		if (_size) {
			_tail->_next = new node(val, nullptr, _tail);
			_tail = _tail->next();
		}
		else {
			_head = _tail = new node(val, nullptr, nullptr);
		}
			
		_size++;
	}
	
	inline void push_front(const T& val)
	{
		if (_size) {
			_head->_prev = new node(val, _head, nullptr);
			_head = _head->prev();
		}
		else {
			_head = _tail = new node(val, nullptr, nullptr);
		}
			
		_size++;
	}
	
	inline size_t size() { return _size; }
	inline node* head() { return _head; }
	inline node* tail() { return _tail; }
	
	inline node* at(size_t i)
	{
		assert(i < _size);
		
		node* retval = nullptr;
		if (i < _size / 2) {
			retval = _head;
			while (i--)
				retval = retval->next();
		}
		else {
			retval = _tail;
			i = _size - i - 1;
			while (i--)
				retval = retval->prev();
		}
		
		return retval;
	}
	
	inline node* get(T val, node* from = nullptr)
	{
		from = from == nullptr ? _head : from;
		
		node* retval = nullptr;
		
		for (	retval = from; 	
				retval != nullptr && retval->data != val; 
				retval = retval->next());
				
		return retval;
	}
	
	inline int get_i(T val, node* from = nullptr)
	{
		from = from == nullptr ? _head : from;
		
		int retval = 0;
		node* n = nullptr;
		
		for (	n = from; 
				n != nullptr && n->data != val; 
				n = n->next(), retval++);
		
		if (n == nullptr) return -1;
		
		return retval;
	}
	
	inline void insert_at(size_t i, T val)
	{
		assert(i <= _size);
		
		if (_size) {
			if (i == _size) {
				_tail->_next = new node(val, nullptr, _tail);
				_tail = _tail->_next;
			}
			else if (i == 0) {
				_head->_prev = new node(val, _head, nullptr);
				_head = _head->_prev;
			}
			else {
				node* prev = at(i)->prev();
				node* next = prev->next();
				
				prev->_next = new node(val, next, prev);
				next->_prev = prev->_next;
			}
		}
		else {
			_head = _tail = new node(val, nullptr, nullptr);
		}
		
		_size++;
	}
	
	inline void insert_after(node* pos, T val)
	{
		assert(pos);
		
		node* next = pos->next();
		
		pos->_next = new node(val, next, pos);
		if (next) next->_prev = pos->_next;
	}
	
	inline void insert_before(node* pos, T val)
	{
		assert(pos);
		
		node* prev = pos->prev;
		
		pos->_prev = new node(val, pos, prev);
		if (prev) prev->_next = pos->_prev;
	}
	
	template<typename U>
	friend ostream& operator<<(ostream& out, const dlist<U>& list); 
	
	inline void sort();
};

template<typename T>
ostream& operator<<(ostream& out, const dlist<T>& list)
{
	if (list._size) {
		for (	typename dlist<T>::node* tmp = list._head; 
				tmp != nullptr; 
				tmp = tmp->next()) {
			out << tmp->data << " ";
		}
	}
	else {
		out << "<empty list>";
	}
	
	return out;
}

template<typename T>
typename dlist<T>::node* part_sort(typename dlist<T>::node* l, typename dlist<T>::node* r)
{	
	using node = typename dlist<T>::node;

	assert(l);
	assert(r);
	
	T pivot = r->data;
	
	node* ret = l->prev();
	
	for (node* i = l; i != r; i = i->next()) {
		if (i->data <= pivot) {
			ret = (ret == nullptr) ? l : ret->next();
			
			swap(i->data, ret->data);
		}
	}
	
	ret = (ret == nullptr) ? l : ret->next();
	swap(r->data, ret->data);
	
	return ret;
}

template<typename T>
void full_sort(typename dlist<T>::node* l, typename dlist<T>::node* r)
{
	using node = typename dlist<T>::node;
	
	if (l != nullptr && r != nullptr && 
		l != r && l != r->next()) {		
		node* pivot = part_sort<T>(l, r);
		part_sort<T>(l, pivot->prev());
		part_sort<T>(pivot->next(), r);	
	}
}

template<typename T>
void dlist<T>::sort()
{
	full_sort<T>(_head, _tail);
}


int main()
{
	dlist<int> list;
	
	list.push_back(1);
	list.push_back(2);
	list.push_back(56);
	list.push_back(-10);
	list.push_back(100);
	
	cout << list << endl;
	
	list.insert_at(5, 500);
	
	cout << list << endl;
	
	cout << list.get_i(-4) << endl;
}
