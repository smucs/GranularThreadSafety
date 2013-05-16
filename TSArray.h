// A thread safe granular-locking array by Tim Bashford, 10/05/13

#pragma once
#include <set>
#include <boost\thread.hpp>

template<typename T>
class TSArray
{
public:
	TSArray(unsigned s);
	~TSArray(void);

	T& operator[](unsigned i);

	void Lock(unsigned ind);
	void Unlock(unsigned ind);

private:
	T* d;
	std::set<T*> lv;
	boost::mutex mut;
	boost::condition_variable cond;
};


template<typename T>
TSArray<T>::TSArray(unsigned s)
{
	d = new T[s];
}

template<typename T>
TSArray<T>::~TSArray(void)
{
	delete d;
}

template<typename T>
T& TSArray<T>::operator[](unsigned i)
{
	return d[i];
}

template<typename T>
void TSArray<T>::Lock(unsigned ind)
{
	boost::unique_lock<boost::mutex> l(mut);
	while(lv.find(&(d[ind])) != lv.end())
	{
		cond.wait(l);
	}

	lv.insert(&(d[ind]));
}

template<typename T>
void TSArray<T>::Unlock(unsigned ind)
{
	boost::lock_guard<boost::mutex> l(mut);
	lv.erase(&(d[ind]));
	cond.notify_all();
}
