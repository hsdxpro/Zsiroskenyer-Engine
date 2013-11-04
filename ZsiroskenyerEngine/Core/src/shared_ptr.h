////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This is a reference counting pointer, that calls a deleter when no one
//	points to the object any longer.
//	It's totally like STL, EXCEPT that it'll always know the reference count, no
//	matter where and how you assign it a value.
////////////////////////////////////////////////////////////////////////////////


#include <unordered_map>
#include <functional>
#include <cassert>


template <class T>
class zs_shared_ptr {
public:
	// construct
	zs_shared_ptr();
	template <class Y>
	explicit zs_shared_ptr(Y* ptr);
	template <class Y, class Deleter>
	zs_shared_ptr(Y* ptr, Deleter d);

	zs_shared_ptr(const zs_shared_ptr& r);
	zs_shared_ptr(zs_shared_ptr&& r);
	// destruct
	~zs_shared_ptr();

	// assignment
	zs_shared_ptr& operator=(const zs_shared_ptr& r);
	zs_shared_ptr& operator=(zs_shared_ptr&& r);
private:
	void register_me();
	size_t unregister_me();
	void destroy_me();
	T* ptr;
	static std::unordered_map<T*, size_t> counts;
	std::function<void(T*)> deleter;
};


// register new pointer
template <class T>
void zs_shared_ptr<T>::register_me() {
	if (ptr != nullptr) {
		auto it = counts.insert(std::pair<T*, size_t>(ptr, 0));
		it.first->second++;
	}
}
template <class T>
size_t zs_shared_ptr<T>::unregister_me() {
	if (ptr != nullptr) {
		auto it = counts.find(ptr);
		assert(it != counts.end());
		it->second--;
		if (it->second != 0) {
			return it->second;
		}
		else {
			counts.erase(it);
			return 0;
		}
	}
	return 0;
}



// constructors
template <class T>
zs_shared_ptr<T>::zs_shared_ptr() : ptr(nullptr) {
}

template <class T>
template <class Y>
zs_shared_ptr<T>::zs_shared_ptr(Y* ptr) : ptr(ptr) {
	register_me();
}

template <class T>
template <class Y, class Deleter>
zs_shared_ptr<T>::zs_shared_ptr(Y* ptr, Deleter d) : ptr(ptr), deleter(d) {
	register_me();
}


template <class T>
zs_shared_ptr<T>::zs_shared_ptr(const zs_shared_ptr& r) : ptr(other.ptr), deleter(other.deleter) {
	register_me();
}
template <class T>
zs_shared_ptr<T>::zs_shared_ptr(zs_shared_ptr&& r) : ptr(other.ptr), deleter(other.deleter) {
	other.ptr = nullptr;
}

// destruct
template <class T>
zs_shared_ptr<T>::~zs_shared_ptr() {
	unregister_me();
}

// assign
template <class T>
zs_shared_ptr<T>& zs_shared_ptr<T>::operator=(const zs_shared_ptr& r) {
	unregister_me();
	ptr = other.ptr;
	if (other.deleter)
		deleter = other.deleter;
	register_me();
}

template <class T>
zs_shared_ptr<T>& zs_shared_ptr<T>::operator=(zs_shared_ptr&& r) {
	unregister_me();
	ptr = other.ptr;
	other.ptr = nullptr;
	if (other.deleter)
		deleter = other.deleter;
}
