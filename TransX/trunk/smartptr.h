

template<class T> class XPtr
{
private:
	T* ptr;
public:
	XPtr(T* pptr){ptr=pptr;};
	T* operator->(){return ptr;};
	~XPtr(){delete ptr;};
};




