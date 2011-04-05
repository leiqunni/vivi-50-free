//----------------------------------------------------------------------
//
//			File:			"gap_vector.h"
//			Created:		28-Jun-2009
//			Author:			津田伸秀
//			Description:
//
//----------------------------------------------------------------------

/*

	Copyright (C) 2011 by Nobuhide Tsuda

	本ソースコードは基本的に MIT ライセンスに従う。
	http://www.opensource.org/licenses/mit-license.php
	http://sourceforge.jp/projects/opensource/wiki/licenses%2FMIT_license

	ただし、おいらは不自由で使い勝手の悪い GPL が大嫌いなので、
	GPL ライセンスプロジェクトが本ソースを流用することを禁じる

*/

#pragma once

#define	GV_USE_ITR_FACADE		1

//#include "stdafx.h"
#include	<iostream>
#include <memory>
//#include <boost/static_assert.hpp>
#include <boost/shared_ptr.hpp>
//#include <typeinfo>
#include <boost/type_traits.hpp>

#if	GV_USE_ITR_FACADE
#include	<boost/iterator/iterator_facade.hpp>
#endif

#define		PTR_ALLOCATOR		0

#ifndef ASSERT
#define	ASSERT(x)	if( !x ) { cout << "Assertsion Failed.\n"; }
#endif

#if 0
template<typename InputIterator, typename OutputItearator, class Allocaotr>
OutputItearator _copy_data(OutputItearator dst, InputIterator first, InputIterator last, Allocaotr allocator)
{
	while( first != last ) {
		allocator.construct(dst, *first);
		//*dst = *first;
		++dst;
		++first;
	}
	return dst;
}
#endif

typedef unsigned int uint;
typedef size_t index_t;

//	std::allocater などを使用するので、名前空間は std にしておく
namespace std {

//template<typename Type, class _A = allocator<Type>> class gap_vector;

#if GV_USE_ITR_FACADE
template<typename Type, class _A = allocator<Type>>
class _Gap_Vector_Iterator //: public std::iterator<std::random_access_iterator_tag, Type>
	: public boost::iterator_facade<_Gap_Vector_Iterator<Type>,
									Type, std::random_access_iterator_tag, Type&>
{
public:
	typedef Type* pointer;
	typedef const Type* const_pointer;
public:
	pointer m_ptr;
	pointer m_gapBegin;
	pointer m_gapEnd;
public:
	_Gap_Vector_Iterator() : m_ptr(0), m_gapBegin(0), m_gapEnd(0) {}
	_Gap_Vector_Iterator(const _Gap_Vector_Iterator &x)
		: m_ptr(x.m_ptr), m_gapBegin(x.m_gapBegin), m_gapEnd(x.m_gapEnd) {}
	_Gap_Vector_Iterator(pointer ptr, pointer gapBegin, pointer gapEnd)
		: m_ptr(ptr), m_gapBegin(gapBegin), m_gapEnd(gapEnd) {}
public:
	pointer get() { return m_ptr; }
	const_pointer get() const { return (const_pointer)m_ptr; }

	Type &dereference() const { return *m_ptr; }
	void	increment() { if( ++m_ptr == m_gapBegin ) m_ptr = m_gapEnd; }
	void	decrement() { if( m_ptr == m_gapEnd ) { m_ptr = m_gapBegin; } --m_ptr; }
	bool	equal(const _Gap_Vector_Iterator &rhs) const { return m_ptr == rhs.m_ptr; }
	void	advance(int n)
	{
		if( n >= 0 ) {
			if( m_ptr < m_gapBegin && m_ptr + n >= m_gapBegin ) n += (m_gapEnd - m_gapBegin);
		} else {
			if( m_ptr >= m_gapBegin && m_ptr + n < m_gapBegin ) n -= (m_gapEnd - m_gapBegin);
		}
		m_ptr += n;
	}
	int		distance_to(const _Gap_Vector_Iterator &x) const
	{
		if( m_ptr < m_gapBegin && x.m_ptr >= m_gapEnd )
			return (x.m_ptr - m_ptr - (m_gapEnd - m_gapBegin));
		if( x.m_ptr < m_gapBegin && m_ptr >= m_gapEnd )
			return -(m_ptr - x.m_ptr - (m_gapEnd - m_gapBegin));
		return x.m_ptr - m_ptr;
	}
};
template<typename Type, class _A = allocator<Type>>
class _Gap_Vector_Const_Iterator //: public std::iterator<std::random_access_iterator_tag, Type>
	: public boost::iterator_facade<_Gap_Vector_Const_Iterator<Type>,
									Type const, std::random_access_iterator_tag, Type>
{
public:
	typedef const Type* const_pointer;
public:
	const_pointer m_ptr;
	const_pointer m_gapBegin;
	const_pointer m_gapEnd;
public:
	_Gap_Vector_Const_Iterator() : m_ptr(0), m_gapBegin(0), m_gapEnd(0) {}
	_Gap_Vector_Const_Iterator(const _Gap_Vector_Const_Iterator &x)
		: m_ptr(x.m_ptr), m_gapBegin(x.m_gapBegin), m_gapEnd(x.m_gapEnd) {}
	_Gap_Vector_Const_Iterator(const _Gap_Vector_Iterator<Type, _A> &x)			//	非コンスト → コンスト変換
		: m_ptr(x.m_ptr), m_gapBegin(x.m_gapBegin), m_gapEnd(x.m_gapEnd) {}
	_Gap_Vector_Const_Iterator(const_pointer ptr, const_pointer gapBegin, const_pointer gapEnd)
		: m_ptr(ptr), m_gapBegin(gapBegin), m_gapEnd(gapEnd) {}
public:
	const_pointer get() const { return m_ptr; }

	Type dereference() const { return *m_ptr; }
	void	increment() { if( ++m_ptr == m_gapBegin ) m_ptr = m_gapEnd; }
	void	decrement() { if( m_ptr == m_gapEnd ) { m_ptr = m_gapBegin; } --m_ptr; }
	bool	equal(const _Gap_Vector_Const_Iterator &rhs) const { return m_ptr == rhs.m_ptr; }
	void	advance(int n)
	{
		if( n >= 0 ) {
			if( m_ptr < m_gapBegin && m_ptr + n >= m_gapBegin ) n += (m_gapEnd - m_gapBegin);
		} else {
			if( m_ptr >= m_gapBegin && m_ptr + n < m_gapBegin ) n -= (m_gapEnd - m_gapBegin);
		}
		m_ptr += n;
	}
	int		distance_to(const _Gap_Vector_Const_Iterator &x) const
	{
		if( m_ptr < m_gapBegin && x.m_ptr >= m_gapEnd )
			return (x.m_ptr - m_ptr - (m_gapEnd - m_gapBegin));
		if( x.m_ptr < m_gapBegin && m_ptr >= m_gapEnd )
			return -(m_ptr - x.m_ptr - (m_gapEnd - m_gapBegin));
		return x.m_ptr - m_ptr;
	}
};
#else
template<typename Type, class _A = allocator<Type>>
class _Gap_Vector_Const_Iterator : public std::iterator<std::random_access_iterator_tag, Type>
{
public:
	typedef const Type* const_pointer;
protected:
	const_pointer m_ptr;
	//const_pointer m_first;
	const_pointer m_gapBegin;
	const_pointer m_gapEnd;
	//const_pointer m_last;
public:
	_Gap_Vector_Const_Iterator() : m_ptr(0), m_gapBegin(0), m_gapEnd(0) {}
	_Gap_Vector_Const_Iterator(const_pointer ptr, const_pointer gapBegin, const_pointer gapEnd)
		: m_ptr(ptr), m_gapBegin(gapBegin), m_gapEnd(gapEnd) {}
public:
	bool	operator==(const _Gap_Vector_Const_Iterator &x) const { return m_ptr == x.m_ptr; }
	bool	operator!=(const _Gap_Vector_Const_Iterator &x) const { return m_ptr != x.m_ptr; }
	bool	operator> (const _Gap_Vector_Const_Iterator &x) const { return m_ptr > x.m_ptr; }
	bool	operator>=(const _Gap_Vector_Const_Iterator &x) const { return m_ptr >= x.m_ptr; }
	bool	operator< (const _Gap_Vector_Const_Iterator &x) const { return m_ptr < x.m_ptr; }
	bool	operator<=(const _Gap_Vector_Const_Iterator &x) const { return m_ptr <= x.m_ptr; }
public:
	const_pointer get() const { return m_ptr; }
	const_pointer operator->() const { return m_ptr; }
	Type operator*() const { return *m_ptr; }
	_Gap_Vector_Const_Iterator &operator++()
	{
		if( ++m_ptr == m_gapBegin ) m_ptr = m_gapEnd;
		return *this;
	}
	_Gap_Vector_Const_Iterator &operator--()
	{
		if( m_ptr == m_gapEnd ) m_ptr = m_gapBegin;
		--m_ptr;
		return *this;
	}
	_Gap_Vector_Const_Iterator &operator+=(int n)
	{
		if( n >= 0 ) {
			if( m_ptr < m_gapBegin && m_ptr + n >= m_gapBegin ) n += (m_gapEnd - m_gapBegin);
		} else {
			if( m_ptr >= m_gapBegin && m_ptr + n < m_gapBegin ) n -= (m_gapEnd - m_gapBegin);
		}
		m_ptr += n;
		return *this;
	}
	_Gap_Vector_Const_Iterator &operator-=(int n) { return *this += -n; }
	_Gap_Vector_Const_Iterator operator+(int n) const
	{
		_Gap_Vector_Const_Iterator i(*this);
		i += n;
		return i;
	}
	_Gap_Vector_Const_Iterator operator-(int n) const
	{
		_Gap_Vector_Const_Iterator i(*this);
		i += -n;
		return i;
	}
	int operator-(const _Gap_Vector_Const_Iterator &x) const
	{
		if( m_ptr < m_gapBegin && x.m_ptr >= m_gapEnd )
			return -(x.m_ptr - m_ptr - (m_gapEnd - m_gapBegin));
		if( x.m_ptr < m_gapBegin && m_ptr >= m_gapEnd )
			return m_ptr - x.m_ptr - (m_gapEnd - m_gapBegin);
		return m_ptr - x.m_ptr;
	}

	//friend class gap_vector<Type, _A>;
};
template<typename Type, class _A = allocator<Type>>
class _Gap_Vector_Iterator : public _Gap_Vector_Const_Iterator<Type, _A>
{
public:
	typedef const Type* const_pointer;
	typedef Type* pointer;
public:
	_Gap_Vector_Iterator() {}
	_Gap_Vector_Iterator(const_pointer ptr, const_pointer gapBegin, const_pointer gapEnd)
		: _Gap_Vector_Const_Iterator(ptr, gapBegin, gapEnd) {}
public:
	Type &operator*() const { return *const_cast<pointer>(m_ptr); }
	pointer operator->() { return const_cast<pointer>(m_ptr); }

#if 0
	_Gap_Vector_Iterator &operator+=(int n)
	{
		if( n >= 0 ) {
			if( m_ptr < m_gapBegin && m_ptr + n >= m_gapBegin ) n += (m_gapEnd - m_gapBegin);
		} else {
			if( m_ptr >= m_gapBegin && m_ptr + n < m_gapBegin ) n -= (m_gapEnd - m_gapBegin);
		}
		m_ptr += n;
		return *this;
	}
	_Gap_Vector_Iterator &operator-=(int n) { return *this += -n; }
#endif
	_Gap_Vector_Iterator operator+(int n) const
	{
		_Gap_Vector_Iterator i(*this);
		i += n;
		return i;
	}
	_Gap_Vector_Const_Iterator operator-(int n) const
	{
		_Gap_Vector_Iterator i(*this);
		i += -n;
		return i;
	}
	int operator-(const _Gap_Vector_Iterator &x) const
	{
		if( m_ptr < m_gapBegin && x.m_ptr >= m_gapEnd )
			return -(x.m_ptr - m_ptr - (m_gapEnd - m_gapBegin));
		if( x.m_ptr < m_gapBegin && m_ptr >= m_gapEnd )
			return m_ptr - x.m_ptr - (m_gapEnd - m_gapBegin);
		return m_ptr - x.m_ptr;
	}
};
#endif

template<typename Type, class _A = allocator<Type>>
class gap_vector
{
public:
	typedef Type value_type;
	typedef uint size_type;
	typedef int difference_type;		//	ギャップが無いとみなした場合のインデックス差
	typedef uint index_type;			//	バッファインデックス [0, size()]
	typedef Type& reference;
	typedef const Type& const_reference;
	typedef Type* pointer;
	typedef const Type* const_pointer;
	typedef _A Allocator;
public:
	//	コンストラクタ
	gap_vector() : m_first(0), m_gapBegin(0), m_gapEnd(0), m_last(0)
#if	!PTR_ALLOCATOR
	, _allocator(_A())
#endif
	{
#if PTR_ALLOCATOR
		if( _pAllocator == NULL ) _pAllocator = new _A();
		//m_allocator = boost::shared_ptr<Allocator>(_pAllocator);
#endif
	}
	gap_vector(size_type n, value_type v = Type())
		: m_first(0), m_gapBegin(0), m_gapEnd(0), m_last(0)
#if	!PTR_ALLOCATOR
	, _allocator(_A())
#endif
	{
#if PTR_ALLOCATOR
		if( _pAllocator == NULL ) _pAllocator = new _A();
		//m_allocator = boost::shared_ptr<Allocator>(_pAllocator);
#endif
		reserve(n);
		while( m_gapBegin != m_gapEnd ) *m_gapBegin++ = v;
	}
	//	コピーコンストラクタ
	gap_vector(const gap_vector &x) : m_first(0), m_gapBegin(0), m_gapEnd(0), m_last(0)
#if	!PTR_ALLOCATOR
	, _allocator(x._allocator)
#endif
	{
		if( !x.empty() )
			assign(x.begin(), x.end());
	}
	//	デストラクタ
	~gap_vector()
	{
		if( m_first != 0 ) {
			_Destroy(m_first, m_gapBegin);
			_Destroy(m_gapEnd, m_last);
#if PTR_ALLOCATOR
			_pAllocator->deallocate(m_first, m_last - m_first);
#else
			_allocator.deallocate(m_first, m_last - m_first);
#endif
		}
	}
	gap_vector &operator=(const gap_vector &x)
	{
		//if( x.empty() )
		//	clear();
		//else
			assign(x.begin(), x.end());
		return *this;
	}

public:
	typedef _Gap_Vector_Const_Iterator<Type, _A> const_iterator;
	typedef _Gap_Vector_Iterator<Type, _A> iterator;
	const_iterator begin() const { return const_iterator(m_first != m_gapBegin ? m_first : m_gapEnd, /*m_first,*/ m_gapBegin, m_gapEnd /*, m_last*/); }
	const_iterator end() const { return const_iterator(m_last, /*m_first,*/ m_gapBegin, m_gapEnd /*, m_last*/); }
	const_iterator cbegin() const { return const_iterator(m_first != m_gapBegin ? m_first : m_gapEnd, /*m_first,*/ m_gapBegin, m_gapEnd /*, m_last*/); }
	const_iterator cend() const { return const_iterator(m_last, /*m_first,*/ m_gapBegin, m_gapEnd /*, m_last*/); }
	iterator begin() { return iterator(m_first != m_gapBegin ? m_first : m_gapEnd, m_gapBegin, m_gapEnd); }
	iterator end() { return iterator(m_last, m_gapBegin, m_gapEnd); }
public:
	reference operator[](size_type n) { return *get_pointer(n); }
	const_reference operator[](size_type n) const { return *get_pointer(n); }
	//Type operator[](size_type n) const { return *get_pointer(n); }
public:
	bool	empty() const { return !m_first || m_first == m_gapBegin && m_gapEnd == m_last; }
	size_type	size() const { return !m_first ? 0 : (m_gapBegin - m_first) + (m_last - m_gapEnd); }
	size_type	capacity() const { return /*!m_first ? 0 :*/ m_last - m_first; }
	const_pointer get_first() const { return m_first; }
	const_pointer get_last() const { return m_last; }
	const_pointer get_gapBegin() const { return m_gapBegin; }
	const_pointer get_gapEnd() const { return m_gapEnd; }
public:
	void clear()
	{
		if( m_first != 0 ) {
			_Destroy(m_first, m_gapBegin);
			_Destroy(m_gapEnd, m_last);
			m_gapBegin = m_first;
			m_gapEnd = m_last;
		}
	}
	void reserve(size_type n)
	{
		if( n > capacity() ) {
#if PTR_ALLOCATOR
			pointer newvec = _pAllocator->allocate(n);
#else
			pointer newvec = _allocator.allocate(n);
#endif
			if( empty() ) {
				m_gapBegin = newvec;
				//m_gapEnd = newvec + n;
			} else {
				//	[m_first, m_gapBegin), [m_gapEnd, m_last) を newvec にコピ
				uint ms = m_gapBegin - m_first;
				if( ms != 0 )
					move_data(newvec, m_first, ms);
				uint ms2 = m_last - m_gapEnd;
				if( ms2 != 0 )
					move_data(newvec + ms, m_gapEnd, ms2);
				m_gapBegin = newvec + ms + ms2;
			}
			if( m_first != NULL ) {
#if PTR_ALLOCATOR
				_pAllocator->deallocate(m_first, m_last - m_first);
#else
				_allocator.deallocate(m_first, m_last - m_first);
#endif
			}
			m_last = m_gapEnd = (m_first = newvec) + n;
		}
	}
	void resize(size_type n, value_type v = value_type())
	{
		move_gap_to_tail();
		const size_t sz = size();
		if( n < sz ) {
			//	[n, sz) をデストロイ
			pointer ptr = m_first + sz;
			m_gapBegin = m_first + n;
			while( ptr != m_gapBegin ) {
				--ptr;
#if PTR_ALLOCATOR
				_pAllocator->destroy(ptr);
#else
				_allocator.destroy(ptr);
#endif
			}
		} else if( n > sz ) {
			//	[sz, n) をコンストラクト
			reserve(n);
			move_gap_to_tail();
			pointer ptr = m_first + sz;
			m_gapBegin = m_first + n;
			while( ptr < m_gapBegin ) {
				*ptr = v;
				++ptr;
			}
		}
		ASSERT( size() == n );
	}
	const_pointer data()
	{
		move_gap_to_tail();
		return m_first;
	}
	template<typename InputIterator>
	void assign(InputIterator first, InputIterator last)
	{
		clear();
		size_t sz = last - first;
		if( sz > capacity() )
			reserve(sz);
		m_gapBegin = m_first;
#if 1
		m_gapBegin = copy_data(m_gapBegin, first, last);
#else
		while( first != last ) {
#if PTR_ALLOCATOR
		_pAllocator->construct(m_gapBegin, *first);
#else
		_allocator.construct(m_gapBegin, *first);
#endif
			//*++ = *first;
			++m_gapBegin;
			++first;
		}
#endif
	}
#if 1
	template<>
	void assign(value_type *first, value_type *last)
	{
		clear();
		size_t sz = last - first;
		if( sz > capacity() )
			reserve(sz);
		m_gapBegin = m_first;
		if( !boost::is_class<value_type>::value ) {
			const size_t sz = last - first;
			memmove((void*)m_gapBegin, (void*)first, sz * sizeof(value_type));
			m_gapBegin += sz;
		} else {
			m_gapBegin = copy_data(m_gapBegin, first, last);
		}
	}
#endif
	void push_back(Type v)
	{
		assert_valid();
		if( m_gapBegin == m_gapEnd )	//	ギャップが無い場合：バッファをアロケート
			expand_buffer();
		else
			move_gap_to_tail();	//	ギャップを末尾に移動
#if PTR_ALLOCATOR
		_pAllocator->construct(m_gapBegin, v);
#else
		_allocator.construct(m_gapBegin, v);
#endif
		//*m_gapBegin = v;
		++m_gapBegin;
		assert_valid();
	}
	iterator erase(const iterator &itr)
	{
		assert_valid();
		assert_valid(itr);
		if( itr == end() || empty() ) return end();
#if	0	//def	_DEBUG
		if( itr.get() < m_first || itr.get() >= m_last )
			TRACE(_T("???\n"));
#endif
		move_gap(itr);
#if PTR_ALLOCATOR
		_pAllocator->destroy(m_gapBegin);
#else
		_allocator.destroy(m_gapBegin);
#endif
		//_Destroy(m_gapEnd, m_gapEnd + 1);
		++m_gapEnd;
		assert_valid();
		return iterator(m_gapEnd, m_gapBegin, m_gapEnd);
	}
	void erase(index_type ix)
	{
		assert_valid();
		if( empty() || ix >= size() ) return;
		move_gap(ix);
#if PTR_ALLOCATOR
		_pAllocator->destroy(m_gapBegin);
#else
		_allocator.destroy(m_gapBegin);
#endif
		//_Destroy(m_gapEnd, m_gapEnd + 1);
		++m_gapEnd;
		assert_valid();
		return;
	}
	void erase(index_type first, index_type last)
	{
		assert_valid();
		if( empty() || first >= size() ) return;
		move_gap(first);
		last = min(last, size());
		const size_t n = last - first;
		_Destroy(m_gapEnd, m_gapEnd + n);
		m_gapEnd += n;
#if 0
		while( first != last ) {
#if PTR_ALLOCATOR
			_pAllocator->destroy(m_gapBegin);
#else
			_allocator.destroy(m_gapBegin);
#endif
			//_Destroy(m_gapEnd, m_gapEnd + 1);
			++m_gapEnd;
			++first;
		}
#endif
		assert_valid();
		return;
	}
	iterator erase(const iterator &first, const iterator &last)
	{
		assert_valid();
		assert_valid(first);
		assert_valid(last);
		if( empty() ) return end();
		if( first == begin() ) {
			if( last == end() ) {
				clear();
				return end();
			}
			move_gap(last);
			_Destroy(m_first, m_gapBegin);
			m_gapBegin = m_first;
			assert_valid();
			return iterator(m_gapEnd, m_gapBegin, m_gapEnd);
		}
		if( last == end() ) {
			move_gap(first);
			_Destroy(m_gapEnd, m_last);
			m_gapEnd = m_last;
			assert_valid();
			return end();
		}
		const size_t n = last - first;
		move_gap(first);
		_Destroy(m_gapEnd, m_gapEnd + n);
		m_gapEnd += n;
		return iterator(m_gapEnd, m_gapBegin, m_gapEnd);
	}
	iterator insert(const iterator &itr, Type ch)
	{

		assert_valid();
		assert_valid(itr);
		const size_type sz = size();
		const size_type ix = get_index(itr);	//	itr はバッファ拡張で無効になるので、今のうちに参照しておく
#ifdef	_DEBUG
		if( ix < 0 || ix > size() ) {
			std::cout << "\x9 ??? bad ix\n";
			ASSERT( 0 );
		}
#endif
		if( m_gapBegin == m_gapEnd )
			expand_buffer();			//	本当はギャップ位置を考慮して新バッファを作成する方が
										//	いいけど、面倒なので手抜き
		move_gap(ix);	//	itr 位置にギャップ移動
#if PTR_ALLOCATOR
		_pAllocator->construct(m_gapBegin, ch);
#else
		_allocator.construct(m_gapBegin, ch);
#endif
		//*m_gapBegin = ch;
		++m_gapBegin;
		assert_valid();
		return iterator(m_gapBegin - 1, m_gapBegin, m_gapEnd);
	}
	void insert(index_type ix, Type ch)
	{

		assert_valid();
		const size_type sz = size();
		if( ix > sz ) ix = sz;
		if( m_gapBegin == m_gapEnd )
			expand_buffer();			//	本当はギャップ位置を考慮して新バッファを作成する方が
										//	いいけど、面倒なので手抜き
		move_gap(ix);	//	itr 位置にギャップ移動
#if PTR_ALLOCATOR
		_pAllocator->construct(m_gapBegin, ch);
#else
		_allocator.construct(m_gapBegin, ch);
#endif
		//*m_gapBegin = ch;
		++m_gapBegin;
		assert_valid();
		return;
	}
	template<typename InputIterator>
	iterator insert(const iterator &itr, InputIterator first, InputIterator last)
	{
		assert_valid(itr);
		size_t ix = get_index(itr);		//	reserve により イテレータが無効になる前にインデックス取得
		return insert(ix, first, last);
	}
	template<typename InputIterator>
	iterator insert(size_t ix, InputIterator first, InputIterator last)
	{
		//##uint sz = last - first;
		//##reserve(size() + sz);
		iterator i = begin() + ix;
		while( first != last ) {
			i = insert(i, *first);
			++i;
			++first;
		}
		--i;
		return iterator(i.get(), m_gapBegin, m_gapEnd);
	}
public:
	//	ギャップ先頭生インデックスを返す。テスト用
	size_t get_raw_index_of_gap() const { return m_gapBegin - m_first; }
	template<typename Iterator>
	size_t	get_index(const Iterator &itr) const		//	ギャップが無い場合のインデックスを返す
	{
		assert_valid();
		assert_valid(itr);
		const_pointer ptr = itr.get();
		if( ptr >= m_gapBegin ) {
			//if( m_gapEnd < m_last )
				ptr -= (m_gapEnd - m_gapBegin);
		}
		return ptr - m_first;
	}
	template<typename Iterator>
	void	move_gap(const Iterator &itr)
	{
		assert_valid();
		assert_valid(itr);
		move_gap(get_index(itr));
	}
	//	gap を ix (0,size()) 位置へ移動
	void	move_gap(size_type ix)
	{
		assert_valid();
		if( m_gapBegin == m_gapEnd ) {
			m_gapBegin = m_gapEnd = m_first + ix;
			return;
		}
		if( ix > size() ) ix = size();
		pointer newGap = m_first + ix;
		//g_newGap = newGap;
		const size_type gapSize = m_gapEnd - m_gapBegin;
		//g_gapSize = gapSize;
		if( newGap < m_gapBegin ) {
			//  ix ＜ ギャップ位置 の場合:
			//
			//	□□□□□□■■■■■□□□
			//        ↑│           ｜
			//        ix└────┐ ｜
			//        ↓          ↓ ｜
			//	□□□■■■■■□□□□□□
			uint sz = m_gapBegin - newGap;	//	移動要素数
			move_data_backward(newGap + gapSize, newGap, sz);
			//memmove((void*)(newGap + gapSize), (void*)newGap, sz * sizeof(Type));
			m_gapEnd = (m_gapBegin = newGap) + gapSize;
			//pointer to = ix + m_gapSize;
			//copy_data_backward(to, ix, sz);
			//sz = min(sz, m_gapSize);
			//clear_data(ix, sz);
			//m_gapBegin = ix;
		} else
		if( newGap == m_gapBegin ) {
		} else if( newGap < m_last - gapSize ) {		//	ギャップサイズ分がリミットとなる
			//static size_t g_sz = 0;
			//static pointer g_temp = 0;
			//              ix
			//              ↓
			//	□□□■■■■■□□□□□□
			//       │           │ ｜
			//       │ ┌────┘ ｜
			//       │ ↓           ｜
			//	□□□□□□■■■■■□□□
			//              ↑
			//              ix
			uint sz = newGap - m_gapBegin;	//	移動要素数
			//g_sz = sz;
			pointer src = m_gapEnd;
			//if( src < m_first ) src = m_first;
			move_data(m_gapBegin, src, sz);
			//memmove((void*)m_gapBegin, (void*)(m_gapEnd - sz), sz * sizeof(Type));
			m_gapEnd = (m_gapBegin = newGap) + gapSize;
			//ASSERT( sz > 0 );
			//copy_data(m_gapBegin, gapEnd, sz);
			//m_gapBegin = ix;
			//uint tail = max(gapEnd, m_gapBegin);
			//clear_data(tail, m_gapBegin + m_gapSize - tail);
		} else
			move_gap_to_tail();
		assert_valid();
	}
	void	move_gap_to_tail()	//	ギャップを末尾に移動
	{
		assert_valid();
		//BOOST_ASSERT( m_gapEnd <= m_last );
		const int sz = m_last - m_gapEnd;
		if( sz != 0 && m_gapBegin < m_gapEnd ) {
			//     gapBegin   gapEnd
			//        ↓        ↓
			//	□□□■■■■■□□□
			//       │           │
			//       │ ┌────┘
			//       │ ↓          
			//	□□□□□□■■■■■
			//              ↑
			//            nLines
			move_data(m_gapBegin, m_gapEnd, sz);
			//memmove((void*)m_gapBegin, (void*)m_gapEnd, sz*sizeof(Type));
			m_gapBegin += sz;
			m_gapEnd = m_last;
		}
		//ASSERT( m_gapEnd <= m_last );
	}
protected:
	void expand_buffer()
	{
		size_type sz = size();
		sz = !sz ? 1 : sz + sz;
#if PTR_ALLOCATOR
		pointer newvec = _pAllocator->allocate(sz);
#else
		pointer newvec = _allocator.allocate(sz);
#endif
		if( empty() ) {
			m_gapBegin = newvec;
			m_gapEnd = newvec + sz;
		} else {
			//	[m_first, m_gapBegin), [m_gapEnd, m_last) を newvec にコピ
			pointer dst = newvec;
			size_t d = m_gapBegin - m_first;
			if( d != 0 ) {
				move_data(dst, m_first, d);
				//memmove((void*)dst, (void*)m_first, d*sizeof(Type));
				dst += d;
			}
			if( (d = m_last - m_gapEnd) != 0 ) {
				move_data(dst, m_gapEnd, d);
				//memmove((void*)dst, (void*)m_gapEnd, d*sizeof(Type));
				dst += d;
			}
			m_gapBegin = dst;
			m_gapEnd = newvec + sz;
		}
		if( m_first != NULL ) {
#if PTR_ALLOCATOR
			_pAllocator->deallocate(m_first, m_last - m_first);
#else
			_allocator.deallocate(m_first, m_last - m_first);
#endif
		}
		m_last = (m_first = newvec) + sz;
		assert_valid();
	}
	template<typename OutputItearator, typename InputIterator>
	OutputItearator copy_data(OutputItearator dst, InputIterator src, InputIterator last)
	{
		if( !boost::is_class<value_type>::value ) {
#if 0
			if( boost::is_pointer<InputIterator>::value ) {
				const size_t sz = last - src;
				memmove((void*)dst, (void*)src, sz * sizeof(value_type));
				dst += sz;
			} else {
#endif
				while( src != last ) {
					*dst = *src;
					++dst;
					++src;
				}
			//}
		} else {
		while( src != last ) {
#if PTR_ALLOCATOR
			_pAllocator->construct(dst, *src);
			//_pAllocator->destroy(src);
#else
			_allocator.construct(dst, *src);
			//_allocator.destroy(src);
#endif
			++dst;
			++src;
		}
		}
		return dst;
	}
#if 0
	template<>
	pointer copy_data(pointer dst, const_pointer src, const_pointer last)
	{
		while( src != last ) {
			*dst = *src;
			++dst;
			++src;
		}
		return dst;
	}
	template<>
	pointer copy_data(pointer dst, pointer src, pointer last)
	{
		while( src != last ) {
			*dst = *src;
			++dst;
			++src;
		}
		return dst;
	}
#endif
	void move_data(pointer dst, pointer src, int sz)
	{
		//static pointer g_src = 0;
		//static pointer g_dst = 0;
		//g_src = src;
		//g_dst = dst;
#if 0
		if( !dst || !src ) {
			cout << "??? dst or src is NULL\n";
			return;
		}
#endif
		while( --sz >= 0 ) {
#if PTR_ALLOCATOR
			_pAllocator->construct(dst, *src);
			_pAllocator->destroy(src);
#else
			_allocator.construct(dst, *src);
			_allocator.destroy(src);
#endif
			++dst;
			++src;
		}
	}
	void move_data_backward(pointer dst, pointer src, int sz)
	{
		dst += sz;
		src += sz;
		while( --sz >= 0 ) {
			--dst;
			--src;
#if PTR_ALLOCATOR
			_pAllocator->construct(dst, *src);
			_pAllocator->destroy(src);
#else
			_allocator.construct(dst, *src);
			_allocator.destroy(src);
#endif
		}
	}
	pointer	get_pointer(size_type n)
	{
		pointer p = m_first + n;
		if( p >= m_gapBegin ) p += (m_gapEnd - m_gapBegin);
		return p;
	}
	const_pointer	get_pointer(size_type n) const
	{
		const_pointer p = m_first + n;
		if( p >= m_gapBegin ) p += (m_gapEnd - m_gapBegin);
		return p;
	}
#if PTR_ALLOCATOR
	void _Destroy(pointer m_first, pointer m_last) { _Destroy_range(m_first, m_last, *_pAllocator); }
#else
	void _Destroy(pointer m_first, pointer m_last) { _Destroy_range(m_first, m_last, _allocator); }
#endif
	void assert_valid() const
	{
#ifdef	_DEBUG
		bool f = false;
		if( !m_first ) {
			if( m_last != 0 ) { f = true, cout << "??? m_last is't NULL\n"; }
			if( m_gapBegin != 0 ) { f = true, cout << "??? m_gapBegin is't NULL\n"; }
			if( m_gapEnd != 0 ) { f = true, cout << "??? m_gapEnd is't NULL\n"; }
		} else {
			if( m_first > m_gapBegin ) { f = true, cout << "??? m_first > m_gapBegin\n"; }
			if( m_gapBegin > m_gapEnd ) { f = true, cout << "??? m_gapBegin > m_gapEnd\n"; }
			if( m_gapEnd > m_last ) { f = true, cout << "??? m_gapEnd > m_last\n"; }
		}
		if( f ) {
			cout << "failed.\n";
			ASSERT( 0 );
		}
#endif
	}
	template<typename Iterator>
	void assert_valid(const Iterator &itr) const
	{
#ifdef	_DEBUG
		bool f = false;
		const_pointer ptr = itr.get();
		if( !m_first ) {
			if( itr != end() ) { f = true; cout << "??? itr should be end()\n"; }
		} else {
			if( ptr < m_first || m_gapBegin <= ptr && ptr < m_gapEnd || m_last < ptr ) {
				f = true;
				cout << "??? bad iterator\n";
			}
		}
		if( f ) {
			cout << "failed.\n";
			ASSERT( 0 );
		}
#endif
	}

private:
	pointer	m_first;		//   ↓first   ↓gapBegi n   ↓gapEnd        ↓m_last
	pointer	m_gapBegin;		//	┌────┬──────┬───────┐  
	pointer	m_gapEnd;		//  │  data  │    gap     │     data     │
	pointer	m_last;			//  └────┴──────┴───────┘

#if		PTR_ALLOCATOR
	//boost::shared_ptr<Allocator>	m_allocator;	//	09/07/12 うまくいかなかた
	static Allocator	*_pAllocator;		//	メモリアロケータ
#else
	Allocator	_allocator;		//	メモリアロケータ
#endif
};

#if PTR_ALLOCATOR
template<typename Type, class _A> _A *gap_vector<Type, _A>::_pAllocator = NULL;
#endif

}
