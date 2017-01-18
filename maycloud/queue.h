#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>

/**
 * Очередь с собственной "кучей"
 */
template <class type>
class Queue
{
protected:
	/**
	 * Вместительность "кучи"
	 */
	size_t capacity;
	
	/**
	 * Количество выделенных из "кучи" элементов
	 */
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
	 * Указатель на первый элемент очереди
	 */
	item_p first;
	
	/**
	 * Указатель на последний элемент очереди
	 */
	item_p last;
	
	item_p allocItem();
	void releaseItem(item_p item);
public:
	Queue(size_t cap);
	~Queue();
	
	bool isEmpty();
	void push(type value);
	type pop();
	
	type GetFirst(); 
	type GetLast();
	
	/**
	 * Освободить очередь
	 */
	void freeAll();
	
	/**
	 * И как же в России обойтись без элементов,
	 * которые добавляются вне очереди? :)
	 */
	void pushFirst(type value);
	
	/**
	 * Поиск с удалением найденного элемента
	 */
	type searchAndDel(type value);
	
	/**
	 * Вывести состояние очереди
	 */
	void dumpState();
};

/**
 * Реализация очереди
 */

template <class type>
Queue<type>::Queue(size_t cap)
{
	capacity = cap;
	count = 0;
	
	heap = new item_t[capacity];
	
	first = NULL;
	last = first;
	
	for(size_t i = 1; i < capacity; i++) heap[i - 1].next = &heap[i];
	heap[capacity - 1].next = NULL;
	free = &heap[0];
}

template <class type>
Queue<type>::~Queue()
{
	delete [] heap;
}

template <class type>
typename Queue<type>::item_p Queue<type>::allocItem()
{
	if(free)
	{
		item_p p = free;
		free = p->next;
		++count;
		return p;
	}
	return NULL;
}

template<class type>
void Queue<type>::releaseItem(item_p item)
{
	item_p p = free;
	free = item;
	free->next = p;
	--count;
}

template<class type>
bool Queue<type>::isEmpty()
{
	return first ? false : true;
}

template<class type>
void Queue<type>::push(type value)
{
	item_p p = allocItem();
	if(p)
	{
		p->value = value;
		p->next = NULL;
		if(!isEmpty()) last->next = p;
		if(isEmpty()) first = p;
		last = p;
	}
}

/**
 * И как же в России обойтись без элементов,
 * которые добавляются вне очереди? :)
 */
template<class type>
void Queue<type>::pushFirst(type value)
{
	item_p p = allocItem();
	if(p)
	{
		p->value = value;
		p->next = first;
		first = p;
	}
}

template<class type>
type Queue<type>::pop()
{
	type value = type();
	if(!isEmpty())
	{
		value = first->value;
		item_p p = first;
		if(first == last) last = p->next;
		first = p->next;
		releaseItem(p);
	}
	return value;
}

template<class type>
type Queue<type>::GetFirst()
{
	type value = type();
	if(!isEmpty()) value = first->value;
	
	return value;
}

template<class type>
type Queue<type>::GetLast()
{
	type value = type();
	if(!isEmpty()) value = last->value;
	
	return value;
}

template<class type>
void Queue<type>::freeAll()
{
	while(!isEmpty()) pop();
}

/**
 * Поиск с удалением найденного элемента
 */
template<class type>
type Queue<type>::searchAndDel(type value)
{
	item_p p = first;
	item_p sp;
	
	type val = type();
	
	if(isEmpty()) return val;
		
	if(value == p->value)
	{
		val = pop();
		return val;
	}
	
	while( p->next )
	{
		if(value == p->next->value)
		{
			sp = p->next;
			val = sp->value;
			p->next = sp->next;
			releaseItem(sp);
			break;
		}
		
		p = p->next;
	}
	
	return val;
}

template<class type>
void Queue<type>::dumpState()
{
	fprintf(stderr, "[Queue] total: %u, allocated: %u\n", capacity, count);
}

#endif // QUEUE_H