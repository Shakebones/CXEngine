/*
 * File name - MyVector
 * Author - Dormy.X.Cui
 * Date - 2012.11.6
 * Desc -
 * 
 * To make a template Sequence list.
 *
 * MODIFYING...
 */
#ifndef _MY_VECTOR_
#define _MY_VECTOR_

#include <iostream>
#include <stdlib.h>

using namespace std;

/*
 * Class name - MyVector
 * Author - CuiXue
 * Updated date - 2012.11.5
 * Desc -
 */
template <class Type>
class MyVector
{
private:
	/*
	************************************************
	**************** Kernel - funcs ****************/
	bool _insert(int pos, const Type& data);
	bool _push_back(const Type& data);

	bool _delete(int pos);
	bool _copy(const MyVector<Type>& source);
	bool _create(int length);

	void _reset(void);
	bool _reset_capacity(const int size);

	inline int _getCapacity(void){ return this->capacity; }
	inline int _getLength(void){ return this->length; }

	/*
	 * Function name - _resizeCapacity
	 * Param - 
	 * Return type - int
	 * Author - CuiXue
	 * Date - 2012.11.5
	 * Desc - According to now length to recompute new capacity.
	 * 
	 * Recompute 1 << ((*(unsigned int* )(&f) >> 23) - 126)
	 * formula source : http://blog.csdn.net/jokenchang2000/article/details/2574115
	 */
	inline int _resizeCapacity()
	{
		if( 1 == this->length )
			return 1;

		float f = (float)(this->length - 1);
		return 1 << ((*(unsigned int* )(&f) >> 23) - 126);
	}

	/*
	 * Function name - _resizeCapacity
	 * Param - int(length)
	 * Return type - int
	 * Author - CuiXue
	 * Date - 2012.11.5
	 * Desc - According to param to recompute capacity.
	 */
	inline int _resizeCapacity(int length)
	{
		if( length < 0 )
			throw exception("An error ");

		if( 1 == length )
			return 1;

		float f = (float)(length - 1);
		return 1 << ((*(unsigned int* )(&f) >> 23) - 126); 
	}

private:
	int length;		
	int capacity;
	Type *first;			

	/*
	*************************************************************
	******************* Interface Declaration *******************/
public:
	MyVector(){
		this->length = 0;
		this->capacity = 0;
		this->first = NULL;
	}

	MyVector(const MyVector<Type>& source);
	MyVector(int length);

	/*
	 * Function name - operator=
	 * Param - const MyVector<Type>& source
	 * Return type - 
	 * Author - CuiXue
	 * Date - 2012.11.5
	 * Desc - = overloading.
	 */
	MyVector<Type>& MyVector<Type>::operator=(const MyVector<Type>& source)
	{
		if( !this->_copy(source) )
			throw exception("An error");

		return *this;
	}

	~MyVector(void){
		this->_reset();
	}

	void Insert(int pos, Type& data);
	bool Pushback(const Type& data);
	void Remove(int pos);
	void Copy(const MyVector<Type>& source);
	void Reset(void);
	void Recreate();
	void Recreate(int length);

	/******** Access-Functions & Tools ********/
	inline int	GetMyVectorLength(void) { return this->_getLength(); }
	inline int	GetMyVectorCapacity(void){ return this->_getCapacity(); }
	inline void	ResetCapacity(const int size);

	inline Type* GetFirstPointerAddr(void){
		if( !this->first )
		{
			throw exception("Error pointer");
			return NULL;
		}

		return this->first;
	}

	inline Type& operator[](int pos){
		if( pos < 0 || pos > this->length )
			throw out_of_range("Out of range");

		return this->first[pos];
	}
};

/*
***********************************************************************
************************* Functions definition*************************/
/*
 * Function name - _copy
 * Param - const MyVector<Type>& source
 * Return type - 
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Tools func
 * 
 * WARNING !
 * Use this func, users must overload customed-datatype's = operator.
 */
template <class Type>
bool MyVector<Type>::_copy(const MyVector<Type>& source)
{
	if( source.length > this->length )
	{
		if( source.capacity > this->capacity )
		{
			Type* temp = new Type[source.length];
			if( !temp )
				throw exception("An error ");

			for( int i = 0; i < source.length; i++ )
				temp[i] = source.first[i];

			if( NULL != this->first )
				delete [] this->first;

			this->first = temp;
			temp = NULL;
			this->capacity = source.capacity;
			this->length = source.length;

			return true;
		}

		for( int i = 0; i < source.length; i++ )
			this->first[i] = source.first[i];
	}

	else
	{
		for( int i = 0; i < source.length; i++ )
			this->first[i] = source.first[i];
	}

	return true;
}

/*
 * Function name - _create
 * Param - int(length)
 * Return type - bool
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Tools for create obj
 */
template <class Type>
bool MyVector<Type>::_create(int length)
{
	if( !length )
	{
		throw exception("An error");
		return false;
	}
	
	int result = this->_resizeCapacity(length);
	Type* temp = NULL;

	temp = new Type[result];
	if( !temp )
		return false;

	this->length = length;
	this->capacity = result;

	if( NULL != this->first )
		delete [] this->first;
	
	this->first = temp;
	temp = NULL;

	return true;
}

/*
 * Function name - MyVector
 * Param - int(length)
 * Return type - 
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Constructor with param
 */
template <class Type>
MyVector<Type>::MyVector(int length)
{
	this->length = 0;
	this->capacity = 0;
	this->first = NULL;

	if( !this->_create(length) )
		throw exception("An error");
}

/*
 * Function name - MyVector
 * Param - const MyVector<Type>& source
 * Return type - 
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Constructor with param
 */
template <class Type>
MyVector<Type>::MyVector(const MyVector<Type>& source)
{
	this->length = 0;
	this->numOfElements = 0;
	this->capacity = 0;
	this->first = NULL;

	if( !this->_copy(&source) )
		throw exception("An error");
}

/*
 * Function name - _insert
 * Param - int(pos), Type&(data)
 * Return type - bool
 * Author - CuiXue
 * Date - 2012.11.14
 * Desc - Tools for interface 'Insert()'
 */
template <class Type>
bool MyVector<Type>::_insert(int pos, const Type& data)
{
	if( pos < 0 || pos > this->length )
		return false;

	// First time
	if( !this->first )
	{
		Type* tempFirst = new Type[1];
		if( !tempFirst )
		{
			throw out_of_range("Out of range");
			return false;
		}

		this->length++;
		this->capacity = 1;
		tempFirst[0] = data;
		this->first = tempFirst;
		tempFirst = NULL;
		return true;
	}

	if( pos == this->length )
	{
		if( !this->_push_back(data) )
			return false;

		return true;
	}

	if( (this->length + 1) > this->capacity )
	{
		int result = this->_resizeCapacity(this->length + 1);
		Type* tempFirst = new Type[result];
		if( !tempFirst )
		{
			this->length--;
			return false;
		}

		this->capacity = result;

		// 复制源数据与新数据	
		// Optimizing...
		int i = 0;			// 跟踪源数据存储序列 this->first
		int j = 0;			// 跟踪新数据存储序列 tempFirst
		while(i < this->length + 1)
		{
			if( i == pos )
			{
				tempFirst[i] = data;
				i++;
			}

			else
			{
				tempFirst[i] = this->first[j];
				i++;
				j++;
			}
		}
		
		if( NULL != this->first )
			delete [] this->first;
		this->first = tempFirst;
		this->length++;
		tempFirst = NULL;
	}

	else
	{
		int assignPos = this->length - 1;
		for( ; assignPos != pos; assignPos-- )
		{
			this->first[assignPos] = this->first[assignPos - 1];
		}

		this->first[assignPos] = data;
		this->length++;
	}

	return true;
}

/*
 * Function name - _push_back
 * Param - Type&(data)
 * Return type - bool
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Tools for interface 'Pushback()'
 */
template <class Type>
bool MyVector<Type>::_push_back(const Type& data)
{
	// 预测 —— 当 顺序容器的当前长度自增1后，大于现有容量， 则进行
	// 重新内存分配，新分配容量大小算法同创建时的算法
	if( this->length + 1 > this->capacity )
	{
		int result = this->_resizeCapacity(this->length + 1);
		Type* tempFirst = new Type[result];
		if( !tempFirst )
		{
			this->length--;
			throw exception("An error");
			return false;
		}

		this->capacity = result;

		for( int j = 0; j < this->length; j++ )
			tempFirst[j] = this->first[j];


		tempFirst[this->length] = data;

		if( NULL != this->first )
			delete [] this->first;

		this->first = tempFirst;
		tempFirst = NULL;

		this->length++;
	}

	else
	{
		this->first[this->length] = data;
		this->length++;
	}

	return true;
}

/*
 * Function name - _reset()
 * Param - 
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Reset
 */
template <class Type>
void MyVector<Type>::_reset()
{
	if( NULL != this->first )
		delete [] this->first;

	this->first = NULL;
	this->length = 0;
	this->capacity = 0;
}

/*
 * Function name - _reset_capacity()
 * Param - int size
 * Return type - 
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Tools for interface 'ResetCapacity()'
 */
template <class Type>
bool MyVector<Type>::_reset_capacity(const int size)
{
	if( size < 0  ||
		size < this->length )
	{
		throw out_of_range("Out of range");
		return false;
	}

	Type* temp = new Type[size];
	if( !temp )
	{
		throw bad_alloc("An error");
		return false;
	}

	for( int i = 0; i < this->length; i++ )
		temp[i] = this->first[i];

	if( NULL != this->first )
		delete [] this->first;

	this->first = temp;
	temp = NULL;

	return true;
}

/*
 * Function name - _delete
 * Param - int(pos)
 * Return type - bool
 * Author - CuiXue
 * Date - 2012.11.14
 * Desc - tools for interface 'Remove()'
 */
template <class Type>
bool MyVector<Type>::_delete(int pos)
{
	if( !this->first || (pos < 0 || pos >= length) )
		return false;

	if( pos == this->length - 1 )
	{
		this->length--;
		return true;
	}

	this->length--;
	for(int i = pos; i < this->length; i++)
	{
		this->first[i] = this->first[i+1];
	}

	return true;
}

/*
*******************************************************************
**************************** Interface ****************************/
/*
 * Function name - Recreate
 * Param - 
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Recreate'
 */
template <class Type>
void MyVector<Type>::Recreate()
{
	if( !this->_create(this->length) )
	{
		throw exception("An error");
		return;
	}
}
/*
 * Function name - Recreate
 * Param - int(pos)
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Recreate()'
 */
template <class Type>
void MyVector<Type>::Recreate(int length)
{
	if( !this->_create(length) )
	{
		throw exception("An error");
		return;
	}
}

/*
 * Function name - Insert
 * Param - int(pos)
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Insert()'
 */
template <class Type>
inline void MyVector<Type>::Insert(int pos, Type& data)
{
	if( !this->_insert(pos, data) )
	{
		throw exception("An error");
		return;
	}
}

/*
 * Function name - Pushback
 * Param - <Type&(data)
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Pushback()'
 */
template <class Type>
inline bool MyVector<Type>::Pushback(const Type& data)
{
	if( !this->_push_back(data) )
	{
		throw exception("An error");
		return false;
	}

	return true;
}

/*
 * Function name - Remove
 * Param - int(pos)
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Remove()'
 */
template <class Type>
inline void MyVector<Type>::Remove(int pos)
{
	if( !this->_delete(pos) )
	{
		throw exception("An error");
		return;
	}
}

/*
 * Function name - Copy
 * Param - const 
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Copy()'
 */
template <class Type>
inline void MyVector<Type>::Copy(const MyVector<Type>& source)
{
	if( !this->_copy(source) )
	{
		throw exception("An error");
		return;
	}
}

/*
 * Function name - Reset
 * Param - const 
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'Reset()'
 */
template <class Type>
inline void MyVector<Type>::Reset(void)
{
	this->_reset();
}

/*
 * Function name - ResetCapacity
 * Param - const int size
 * Return type - void
 * Author - CuiXue
 * Date - 2012.11.5
 * Desc - Interface 'ResetCapacity()'
 */
template <class Type>
inline void MyVector<Type>::ResetCapacity(const int size)
{
	if( !this->_reset_capacity(size) )
	{
		throw exception("An error");
		return;
	}
}

#endif

