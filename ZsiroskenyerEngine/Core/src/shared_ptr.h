////////////////////////////////////////////////////////////////////////////////
//	Zsiroskenyer Team
////////////////////////////////////////////////////////////////////////////////
//	This is a reference counting pointer, that calls a deleter when no one
//	points to the object any longer.
//	It's totally like STL, EXCEPT that it'll always know the reference count, no
//	matter where and how you assign it a value.
////////////////////////////////////////////////////////////////////////////////


#include <unordered_map>

template <class T>
class zs_shared_ptr {
public:
	// construct
	zs_shared_ptr();
	template <class Y>
	explicit zs_shared_ptr(Y* ptr);
	template <class Y, class Deleter>
	zs_shared_ptr(Y* ptr, Deleter d);
	// destruct
	~zs_shared_ptr();

	// assignement
	zs_shared_ptr& operator=(const zs_shared_ptr& r);
	template <class Y>
	zs_shared_ptr& operator=(const zs_shared_ptr<Y>& r);
	zs_shared_ptr& operator=(zs_shared_ptr&& r);
	template <class Y>
	zs_shared_ptr& operator=(zs_shared_ptr<Y>&& r);
private:
	T* ptr;
	static std::unordered_map<T*, size_t> counts;
};