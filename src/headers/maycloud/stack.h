#ifndef STACK_H
#define STACK_H

#include <stdio.h>

/**
 * Стек с собственной "кучей"
 */
template <class type>
class Stack
{
protected:
	size_t capacity;
	size_t count;
	
	struct item_t
	{
		type value;
		
		item_t* next;
	};
	typedef item_t *item_p;
	
	/**
	 * "Куча"
	 */
	item_p heap;
	
	/**
	 * Список свободных элементов "кучи"
	 */
	item_p free;
	
	/**
	 * Указатель на вершину стека
	 */
	item_p top;
	
	item_p allocItem();
	void releaseItem(item_p item);
public:
	Stack(size_t cap);
	~Stack();
	
	bool isEmpty();
	void push(type value);
	type pop();
};

/**
 * Реализация стека
 */

template <class type>
Stack<type>::Stack(size_t cap)
{
	capacity = cap;
	
	heap = new item_t[capacity];
	top = NULL;
	for(size_t i = 1; i < capacity; i++) heap[i - 1].next = &heap[i];
	heap[capacity - 1].next = NULL;
	free = &heap[0];
}

template <class type>
Stack<type>::~Stack()
{
	delete [] heap;
}

template <class type>
typename Stack<type>::item_p Stack<type>::allocItem()
{
	item_p p = free;
	free = p->next;
	return p;
}

template<class type>
void Stack<type>::releaseItem(item_p item)
{
	item_p p = free;
	free = item;
	free->next = p;
}

template<class type>
bool Stack<type>::isEmpty()
{
	return top ? false : true;
}

template<class type>
void Stack<type>::push(type value)
{
	item_p p = allocItem();
	p->value = value;
	p->next = top;
	top = p;
}

template<class type>
type Stack<type>::pop()
{
	type value = type();
	if(!isEmpty())
	{
		value = top->value;
		item_p p = top;
		top = p->next;
		releaseItem(p);
	}
	return value;
}

#endif // STACK_H
