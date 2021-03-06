#pragma once

#if UCFG_STDSTL
#	define EXT_HEADER(hname) <hname>
#else
#	define EXT_HEADER(hname) <el/stl/hname>
#endif

#if UCFG_STD_DECIMAL
#	define EXT_HEADER_DECIMAL EXT_HEADER(decimal/decimal)			//!!!?
#else
#	define EXT_HEADER_DECIMAL <el/stl/decimal>
#endif

#ifndef UCFG_CPP11_HAVE_CODECVT
#	define UCFG_CPP11_HAVE_CODECVT (UCFG_LIBCPP_VERSION >= 1100 || UCFG_GNUC_VERSION >= 410 || UCFG_MSC_VERSION >= 1800)
#endif

#if UCFG_CPP11_HAVE_CODECVT
#	define EXT_HEADER_CODECVT EXT_HEADER(codecvt)
#else
#	define EXT_HEADER_CODECVT <el/stl/codecvt>
#endif

#define EXT_HEADER_CONDITION_VARIABLE EXT_HEADER(condition_variable)
#define EXT_HEADER_FUTURE EXT_HEADER(future)
#define EXT_HEADER_ATOMIC EXT_HEADER(atomic)

#if UCFG_STD_SHARED_MUTEX
#	define EXT_HEADER_SHARED_MUTEX EXT_HEADER(shared_mutex)
#else
#	define EXT_HEADER_SHARED_MUTEX <el/stl/shared_mutex>
#endif

#if UCFG_STD_DYNAMIC_BITSET
#	define EXT_HEADER_DYNAMIC_BITSET EXT_HEADER(dynamic_bitset)
#else
#	define EXT_HEADER_DYNAMIC_BITSET <el/stl/dynamic_bitset>
#endif

#if UCFG_STD_SYSTEM_ERROR
#	define EXT_HEADER_SYSTEM_ERROR EXT_HEADER(system_error)
#else
#	define EXT_HEADER_SYSTEM_ERROR <el/stl/system_error>
#endif

#if UCFG_STD_FILESYSTEM
#	define EXT_HEADER_FILESYSTEM EXT_HEADER(filesystem)
#else
#	define EXT_HEADER_FILESYSTEM <el/stl/filesystem>
#endif

#define EXT_HASH_VALUE_NS Ext

#ifdef _M_CEE

using namespace System;

#	define M_INTERFACE interface class
#	define M_REF_CLASS ref class
#	define M_VALUE_STRUCT value struct
#	define M_VIRTUAL 
#	define M_DEF_POINTER(typ) typedef typ ^P##typ;
#	define M_REF(typ) typ%
#	define M_THROW(x) throw gcnew x;
#	define M_NEW gcnew
#	define M_OVERRIDE override
#	define M_ABSTRACT abstract
#	define M_ENUM enum class

typedef Byte byte;


template <class T> void swap(M_REF(T) a, M_REF(T) b) {
	T c = a;
	a = b;
	b = c;
}

#else		// _M_CEE


#	define M_INTERFACE struct
#	define M_REF_CLASS class
#	define M_VALUE_STRUCT struct
#	define M_VIRTUAL  virtual
#	define M_DEF_POINTER(typ) typedef typ *P##typ;
#	define M_REF(typ) typ&
#	define M_THROW(x) throw x;
#	define M_NEW new
#	define M_OVERRIDE
#	define M_ABSTRACT
#	define M_ENUM enum


#if !UCFG_STDSTL && (!defined(_MSC_VER) || _MSC_VER>=1600)
	namespace ExtSTL {
		typedef decltype(__nullptr) nullptr_t;
		using std::nullptr_t;
	}
#endif

#if !UCFG_CPP14_NOEXCEPT
#	define noexcept throw()
#endif

#if defined(_MSC_VER) &&  _MSC_VER<1900
#	define EXT_FAST_NOEXCEPT				//	noexcept prevents inline expansion in VC
#else
#	define EXT_FAST_NOEXCEPT	noexcept
#endif

extern "C" int _cdecl API_uncaught_exceptions() noexcept;

namespace Ext {

template <typename T>
class PtrBase {
public:
	typedef T element_type;
	typedef T *pointer;
	typedef T &reference;

	PtrBase(T *p = 0)
		:	m_p(p)
	{}

	inline T *get() const EXT_FAST_NOEXCEPT { return m_p; }
	inline T *operator->() const EXT_FAST_NOEXCEPT { return m_p; }

	T *release() noexcept {
		pointer r = m_p;
		m_p = nullptr;
		return r;

		//!!!R		return exchange(m_p, nullptr);
	}

	void swap(PtrBase& p) noexcept {
		std::swap(m_p, p.m_p);
	}
protected:
	T *m_p;
};

} // Ext::

#if defined(_VC_CRT_MAJOR_VERSION) && _VC_CRT_MAJOR_VERSION>=14
#	include <vcruntime_exception.h>
#endif

//!!!R #ifndef _CRTBLD
#	include "cpp-old.h"
//!!! #endif

#include "ext-err.h"


#if UCFG_STDSTL
#	include <cstddef>			// to define std::nullptr_t in GCC
#endif

#if UCFG_WDM
extern "C" void __cdecl free(void *p);
#endif

namespace Ext {


AFX_API bool AFXAPI AfxAssertFailedLine(const char* sexp, const char*fileName, int nLine);
}

//!!!R#	if UCFG_EXTENDED
#		if !defined(ASSERT) && !defined(NDEBUG)
#			define ASSERT(f) ((void) (f || Ext::AfxAssertFailedLine(#f, __FILE__, __LINE__)))
#		endif
//!!!R #	endif

#	ifndef ASSERT
#		define ASSERT assert
#	endif


#if (!UCFG_CPP11 /*!!! || defined(_MSC_VER) && _MSC_VER >= 1600 */) && !defined(__GXX_EXPERIMENTAL_CXX0X__) && UCFG_STDSTL
#	define BEGIN_STD_TR1 namespace std { namespace tr1 {
#	define END_STD_TR1 }}
#	ifdef _MSC_VER
#		define TR1_HEADER(name)  <name>
#	else
#		define TR1_HEADER(name)  <tr1/name>
#	endif
#	define EXT_TR1_IS_NS 1
#	define _TR1_NAME(n) std::tr1::##n
#else
#	define TR1_HEADER(name)  <name>
#	define BEGIN_STD_TR1 namespace std {
#	define END_STD_TR1 }
#	define EXT_TR1_IS_NS 0
#	define _TR1_NAME(n) std:: n
#endif

#if !UCFG_CPP11
#	ifndef static_assert
#		define static_assert(e, Msg) STATIC_ASSERT(e)
#	endif
#endif

#	include "interlocked.h"
#undef noexcept

#if UCFG_STL


#	if UCFG_STDSTL

#		if !UCFG_CPP11_HAVE_REGEX
#			define _GLIBCXX_REGEX
#		endif


//!!!	#ifdef  __cplusplus
//!!!		#include <extstl.h>
//!!!	#endif




#		include <algorithm>
#		include <deque>
//!!!R#		include <tr1/functional>

#		if !UCFG_WDM
#			include <functional>
#			include <limits>

#			if !UCFG_WCE
#				include <unordered_map>
#				include <unordered_set>
#			endif

#		endif

#		if UCFG_WCE
#			include <typeinfo>
#		endif

#		include <list>
#		include <map>
#		include <memory>
#		include <queue>
#		include <set>
#		include <stack>
#		include <utility>
#		include <vector>

#		if UCFG_USE_REGEX && UCFG_CPP11_HAVE_REGEX
#			include <regex>
#		endif

#		include <fstream>
#		include <iomanip>
#		include <iostream>
#		include <sstream>

#		include <stdexcept>


#		if UCFG_STD_MUTEX
#			include <mutex>
#		endif

#		if UCFG_STD_THREAD
#			include <thread>
#		endif

//!!!using namespace std;


	namespace ExtSTL {
		using std::pair;
	}

#	else
#		include <el/stl/ext-stl-full.h>

#	endif

#endif //  UCFG_STL

namespace Ext {
	class noncopyable {
	protected:
		noncopyable() {}
	private:
		noncopyable(const noncopyable&);
		noncopyable& operator=(const noncopyable&);
	};
}

#include EXT_HEADER_ATOMIC

namespace Ext {
	using std::atomic;
	using std::atomic_flag;
}

#include "exthelpers.h"		// uses std::swap() from <utility>

namespace Ext {
	class CCriticalSection;
	class CNonRecursiveCriticalSection;

	template <typename P1, typename R> struct UnaryFunction {
		virtual R operator()(P1 p1) =0;
	};

} // Ext::

#if UCFG_CPP11 && UCFG_WCE
#	include <el/stl/type_traits>
#endif


#if UCFG_CPP11_RVALUE
#	define EXT_MOVABLE_BUT_NOT_COPYABLE(TYPE)	\
   private:										\
   TYPE(const TYPE &);							\
   TYPE& operator=(const TYPE &);				\

#	define EXT_RV_REF(typ) typ &&
#else

namespace Ext {

template <class T>
class rv : public T {
   rv();
   ~rv();
   rv(rv const&);
   void operator=(rv const&);
};

template<class T>
struct is_movable : public std::is_convertible<T, rv<T>&> {
};

} // Ext::

#	define EXT_MOVABLE_BUT_NOT_COPYABLE(TYPE)\
   private:\
   TYPE(TYPE &);\
   TYPE& operator=(TYPE &);\
   public:\
   operator ::Ext::rv<TYPE>&() \
   {  return *static_cast< ::Ext::rv<TYPE>* >(this);  }\
   operator const ::Ext::rv<TYPE>&() const \
   {  return *static_cast<const ::Ext::rv<TYPE>* >(this);  }\
   private:

#	define EXT_RV_REF(typ) ::Ext::rv<typ>&

namespace std {

template <class T>
typename enable_if<Ext::is_movable<T>::value, Ext::rv<T>&>::type move(T& x) {
	return *static_cast<Ext::rv<T>* >(&x);
} // std::

}

#endif // UCFG_CPP11_RVALUE

#if !UCFG_CPP14_NOEXCEPT
#	define noexcept throw()		//!!! repeated because somewhere undefined
#endif

#include "ext-ptr.h"

#if !UCFG_MINISTL
#	include "ext-meta.h"
#endif

namespace Ext {
class CInException : noncopyable {
public:
	CInException() noexcept
		:	m_nUncauight(std::uncaught_exceptions()) {
	}

	EXPLICIT_OPERATOR_BOOL() const noexcept {
		return std::uncaught_exceptions() > m_nUncauight ? EXT_CONVERTIBLE_TO_TRUE : 0;
	}
private:
	int m_nUncauight;
};
}

#if UCFG_FRAMEWORK && !defined(_CRTBLD) //!!! || (UCFG_CRT=='U')
#	include "ext-safehandle.h"
#	include "ext-sync.h"
#endif

#define AFX_STATIC_DATA extern __declspec(selectany)

namespace Ext {

class CAlloc {
public:
#if UCFG_INDIRECT_MALLOC
	EXT_DATA static void * (AFXAPI *s_pfnMalloc)(size_t size);
	EXT_DATA static void (AFXAPI *s_pfnFree)(void *p) noexcept ;
	EXT_DATA static size_t (AFXAPI *s_pfnMSize)(void *p);
	EXT_DATA static void * (AFXAPI *s_pfnRealloc)(void *p, size_t size);
	EXT_DATA static void * (AFXAPI *s_pfnAlignedMalloc)(size_t size, size_t align);
	EXT_DATA static void (AFXAPI *s_pfnAlignedFree)(void *p);
#endif
	static void * AFXAPI Malloc(size_t size);
	static void AFXAPI Free(void *p) EXT_NOEXCEPT;
	static size_t AFXAPI MSize(void *p);
	static void * AFXAPI Realloc(void *p, size_t size);
	static void * AFXAPI AlignedMalloc(size_t size, size_t align);
	static void AFXAPI AlignedFree(void *p);

	static void AFXAPI DbgIgnoreObject(void *p)
#if UCFG_ALLOCATOR=='T' && UCFG_HEAP_CHECK
		;
#else
	{}
#endif
};

#if UCFG_WCE
	bool SetCeAlloc();
#endif

	inline void * __fastcall Malloc(size_t size) {
#if UCFG_INDIRECT_MALLOC
		return CAlloc::s_pfnMalloc(size);
#else
		return CAlloc::Malloc(size);
#endif
	}



	/*!!!
	inline void __fastcall Free(void *p) noexcept {
#if UCFG_INDIRECT_MALLOC
		CAlloc::s_pfnFree(p);
#else
		CAlloc::Free(p);
#endif
	}*/

#if UCFG_HAS_REALLOC
	inline void * __fastcall Realloc(void *p, size_t size) {
#	if UCFG_INDIRECT_MALLOC
		return CAlloc::s_pfnRealloc(p, size);
#	else
		return CAlloc::Realloc(p, size);
#	endif
	}
#endif

	inline void * __fastcall AlignedMalloc(size_t size, size_t align) {
#if UCFG_INDIRECT_MALLOC
		return CAlloc::s_pfnAlignedMalloc(size, align);
#else
		return CAlloc::AlignedMalloc(size, align);
#endif
	}

	inline void __fastcall AlignedFree(void *p) {
#if UCFG_INDIRECT_MALLOC
		CAlloc::s_pfnAlignedFree(p);
#else
		CAlloc::AlignedFree(p);
#endif
	}

/*!!!
#if UCFG_USE_POSIX
	inline void * __cdecl _aligned_malloc(size_t size, size_t align) {
		void *r;
		CCheck(posix_memalign(&r, align, size));
		return r;
	}

	inline void __cdecl _aligned_free(void *p) {
		return free(p);
	}
#endif
*/

class AlignedMem {
public:
	AlignedMem(size_t size = 0, size_t align = 0)
		:	m_p(0)
	{
		if (size)
			Alloc(size, align);
	}
	
	~AlignedMem() {
		Free();
	}

	void Alloc(size_t size, size_t align) {
		if (m_p)
			Throw(E_FAIL);
		m_p = AlignedMalloc(size, align);
	}

	void Free() {
		AlignedFree(m_p);
		m_p = 0;
	}

	void* get() { return m_p; }
private:
	void *m_p;
};

} // Ext::

#if UCFG_STL && !defined(_CRTBLD) //!!! || (UCFG_CRT=='U')
//#	define NS std
#	if !UCFG_STDSTL || UCFG_WCE //!!!
#		include <el/stl/exttemplates.h>
#		include <el/stl/clocale>
#		include <el/stl/ext-locale.h>
#	endif
#	include "ext-hash.h"
#	if !UCFG_STDSTL
#		include <el/stl/extstl.h>
#	endif
//#	undef NS
#endif

#if !UCFG_STDSTL
#	include <el/stl/array>
#endif


#if UCFG_STDSTL && UCFG_USE_REGEX && !UCFG_CPP11_HAVE_REGEX
#	include <el/stl/regex>

namespace std {
	using ExtSTL::sub_match;
	using ExtSTL::cmatch;
	using ExtSTL::wcmatch;
	using ExtSTL::wcsub_match;
	using ExtSTL::smatch;
	using ExtSTL::wsmatch;
	using ExtSTL::match_results;
	using ExtSTL::basic_regex;
	using ExtSTL::regex;
	using ExtSTL::wregex;
	using ExtSTL::regex_iterator;
	using ExtSTL::cregex_iterator;
	using ExtSTL::regex_search;
	using ExtSTL::regex_match;
	using ExtSTL::regex_error;

	namespace regex_constants {
		using ExtSTL::regex_constants::ECMAScript;
		using ExtSTL::regex_constants::error_escape;
		using ExtSTL::regex_constants::icase;
		using ExtSTL::regex_constants::match_flag_type;
		using ExtSTL::regex_constants::match_default;
		using ExtSTL::regex_constants::match_any;
		using ExtSTL::regex_constants::syntax_option_type;
	}

} // std::

#endif


namespace Ext {

class AFX_CLASS CNoTrackObject {
public:
	CNoTrackObject();
	virtual ~CNoTrackObject();
};

#if UCFG_FRAMEWORK && !defined(_CRTBLD) || (UCFG_CRT=='U')

class EXT_CLASS CLocalIgnoreBase {
public:
	CLocalIgnoreBase *Next;

	CLocalIgnoreBase() noexcept;
	~CLocalIgnoreBase() noexcept;
	static bool AFXAPI ErrorCodeIsIgnored(const std::error_code& ec) noexcept;
	virtual bool equivalent(const std::error_code& ec) const noexcept =0;
};

template <class T>
class CLocalIgnore : public CLocalIgnoreBase {
public:
	T m_ec;

	CLocalIgnore(const T& ec)
		: m_ec(ec) {
	}

	bool equivalent(const std::error_code& ec) const noexcept override {
		return ec == m_ec;
	}
};

#endif	// UCFG_FRAMEWORK && !defined(_CRTBLD)

//!!!R AFX_API void AFXAPI DbgAddIgnoredHResult(HRESULT hr);



	//!!!extern vector<HRESULT> g_arIgnoreExceptions;

} // Ext::


inline int64_t abs(const int64_t& v) {
	return v>=0 ? v : -v;
}



//!!!R #include "ext-meta.h"

#define HRESULT_OF_WIN32(x) ((HRESULT) ((x) & 0x0000FFFF | (FACILITY_WIN32 << 16) | 0x80000000))			// variant of HRESULT_FROM_WIN32 without condition


#if !defined(_CRTBLD) || UCFG_CRT=='U'

#if UCFG_CPP11 && UCFG_STDSTL && !UCFG_MINISTL
#	include <array>
#	include <type_traits>

#	ifdef _HAS_TR1_NS
		namespace std {
			using tr1::is_pod;
		}
#	endif
#endif

#if UCFG_MSC_VERSION && UCFG_MSC_VERSION <= 1700
namespace std {
template <> struct is_scalar<std::nullptr_t> { //!!! bug in VC
	typedef true_type type;
	static const bool value = true;
};
} // std::
#endif // UCFG_MSC_VERSION <= 1800


#if !UCFG_STD_IDENTITY
namespace std {

template <typename T>
struct identity {
   template <typename U> T &&operator() (U&& v) {
	   return forward<T>(v);
   }
};

} // std::
#endif // !UCFG_STD_IDENTITY

namespace Ext {

#if !UCFG_MINISTL
	using std::exchange;
#endif

#if UCFG_FULL && !UCFG_MINISTL
template <class T, size_t MAXSIZE>
class vararray : public std::array<T, MAXSIZE> {
	typedef std::array<T, MAXSIZE> base;
public:
	using base::begin;
	using base::data;

	vararray(size_t size = 0)
		:	m_size(size)
	{}    

	vararray(const T *b, const T *e)
		:	m_size(e-b)
	{
		std::copy_n(b, m_size, begin());
	}

	vararray(const T *b, size_t size)
		:	m_size(size)
	{
		std::copy_n(b, m_size, begin());
	}

	size_t max_size() const { return MAXSIZE; }

	size_t size() const { return m_size; }
	void resize(size_t n) { m_size = n; }

	void push_back(const T& v) {
		(*this)[m_size++] = v;
	}
	
	const T *constData() const { return data(); }

	typename base::const_iterator end() const { return begin() + size(); }
	typename base::iterator end() { return begin() + size(); }
private:
	size_t m_size;
};
#endif // UCFG_FULL

struct ConstBuf {
	typedef const unsigned char *const_iterator;

	const unsigned char *P;
	size_t Size;

	ConstBuf(const Buf& mb)
		:	P(mb.P)
		,	Size(mb.Size)
	{
	}

	ConstBuf()
		:	P(0)
		,	Size(0)
	{}
	
	ConstBuf(const void *p, size_t len)
		:	P((const unsigned char*)p)
		,	Size(len)
	{}

#if UCFG_STL && !defined(_CRTBLD)
	ConstBuf(const std::vector<byte>& v)
		:	P(&v[0])
		,	Size(v.size())
	{}

#	if UCFG_FULL
	template <size_t N>
	ConstBuf(const std::array<byte, N>& ar)
		:	P(ar.data())
		,	Size(ar.size())
	{}

	template <size_t N>
	ConstBuf(const vararray<byte, N>& ar)
		:	P(ar.data())
		,	Size(ar.size())
	{}
#	endif
#endif

	const unsigned char *begin() const { return P; }
	const unsigned char *end() const { return P+Size; }

	const unsigned char *Find(const ConstBuf& mb) const;
};

EXTAPI bool AFXAPI operator==(const ConstBuf& x, const ConstBuf& y);
inline bool AFXAPI operator!=(const ConstBuf& x, const ConstBuf& y) { return !(x == y); }

EXTAPI void AFXAPI CCheckErrcode(int en);
EXTAPI int AFXAPI CCheck(int i, int allowableError = INT_MAX);
EXTAPI int AFXAPI NegCCheck(int rc);
EXTAPI void AFXAPI CFileCheck(int i);

template <class T>
struct totally_ordered {
	friend bool operator!=(const T& x, const T& y) { return !(x == y); }
	friend bool operator>(const T& x, const T& y) { return y < x; }
	friend bool operator>=(const T& x, const T& y) { return !(x < y); }
	friend bool operator<=(const T& x, const T& y) { return !(y < x); }
};


} // Ext::

namespace EXT_HASH_VALUE_NS {
inline size_t hash_value(const Ext::ConstBuf& mb) {
    return Ext::hash_value(mb.P, mb.Size);
}
}

#ifdef _LIBCPP_VERSION
#	define EXT_LIBCXX_NOEXCEPT noexcept
#elif defined(_GLIBCXX_USE_NOEXCEPT)
#	define EXT_LIBCXX_NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#else
#	define EXT_LIBCXX_NOEXCEPT
#endif


//!!! #if UCFG_WDM

#if UCFG_DEFINE_NEW

inline void * __cdecl operator new(size_t sz) {
	return Ext::Malloc(sz);
}

inline void __cdecl operator delete(void *p) {
	free(p);
}

inline void __cdecl operator delete[](void* p) {
	free(p);
}

inline void * __cdecl operator new[](size_t sz) {
	return Ext::Malloc(sz);
}

#else

void * __cdecl operator new(size_t sz);
void __cdecl operator delete(void *p) EXT_LIBCXX_NOEXCEPT;
void __cdecl operator delete[](void* p) EXT_LIBCXX_NOEXCEPT;
void * __cdecl operator new[](size_t sz);

#endif // UCFG_DEFINE_NEW

//!!!#endif


#ifdef WDM_DRIVER
#	define EXT_DEF_HASH(T)
#else
#	define EXT_DEF_HASH(T) BEGIN_STD_TR1																\
		template<> class hash<T>{ public: size_t operator()(const T& v) const { return EXT_HASH_VALUE_NS::hash_value(v); } };						\
		template<> class hash<const T&>{ public: size_t operator()(const T& v) const { return EXT_HASH_VALUE_NS::hash_value(v); } };		\
		END_STD_TR1
#endif

#define EXT_DEF_HASH_NS(NS, T) } BEGIN_STD_TR1																\
	template<> class hash<NS::T>{ public: size_t operator()(const NS::T& v) const { return v.GetHashCode(); } };						\
	END_STD_TR1 namespace NS {


#include "ext-str.h"


#if UCFG_FRAMEWORK

#	include "ext-blob.h"
#	include "ext-string.h"

#	if UCFG_USE_BOOST
#		include <boost/functional/hash.hpp>
#	endif

#	if _HAS_EXCEPTIONS
#		ifdef _MSC_VER
#			define tr1 C_tr1
#				include <functional>
#			undef tr1
#		else
#				include <functional>
#		endif
#	endif


/*!!!R
#	if !UCFG_WDM
#		include <stdexcept>
#	endif
*/


#	if UCFG_USE_TR1 && defined(_HAS_TR1) && EXT_TR1_IS_NS && !defined(WDM_DRIVER)
	namespace std {
#if		UCFG_STDSTL
			using std::tr1::unordered_map;
			using std::tr1::unordered_set;
#endif
			using std::tr1::array;
			using std::tr1::hash;
	}
#	endif

	
#endif

#if UCFG_FRAMEWORK && !defined(_CRTBLD)
#	include "afterstl.h"
#	include "ext-lru.h"

namespace Ext {
class CIgnoreList {
public:
	typedef IntrusiveList<CLocalIgnoreBase> CIgnoredExceptions;
	CIgnoredExceptions IgnoredExceptions;
};
//extern thread_specific_ptr<CIgnoreList> t_ignoreList;

#	if UCFG_EH_SUPPORT_IGNORE
//!!!?		extern EXT_THREAD_PTR(HResultItem) t_ignoreList;
#	endif

}	// Ext::

#endif	// UCFG_FRAMEWORK && !defined(_CRTBLD)

			

#	if UCFG_FRAMEWORK
		extern std::ostream g_osDebug;
#	endif



namespace ww {
    template<bool> class compile_time_check {
    public:
        compile_time_check(...) {}
    };

    template<> class compile_time_check<false> {
    };
}




namespace Ext {


class CDataExchange;
class String;
class CTime; //!!! need by MFC headers
class CWnd;

#if defined(_MSC_VER) && defined (WIN32)

#	if UCFG_EXTENDED
#		include "el/extres.h"
#	endif

#	if	UCFG_GUI
//#		include "win32/ext-afxdd_.h"
#	endif


#	if UCFG_WCE
#		include <atldefce.h>
#		define _WIN32_WINNT 0
#	endif



#	if UCFG_WCE
#		undef _WIN32_WINNT
#	endif


  // The WinCE OS headers #defines the following, 
  // but it interferes with MFC member functions.
  #undef TrackPopupMenu
  #undef DrawIcon
//!!!  #undef SendDlgItemMessage

#if !UCFG_WCE
//!!!  #undef SetDlgItemText
//!!!  #undef GetDlgItemText
#endif
  //#undef LoadCursor



  #define SM_REMOTESESSION        0x1000

#endif  
} // Ext::


#if defined(_MSC_VER) && !UCFG_EXT_C && !UCFG_WCE
#	include <xhash>
#endif



#if UCFG_FRAMEWORK

#if defined(_HAS_TR1) || _MSC_VER>=1500

#	ifdef _HAS_TR1
	BEGIN_STD_TR1
#	else
	namespace std {
#	endif
//namespace Ext {

//!!!R #if UCFG_STDSTL

#	ifdef _ARRAY_

template <size_t N>
size_t hash_value(const array<unsigned char, N>& vec) {
	return Ext::hash_value(&vec[0], vec.size());
}

template<size_t N> class hash<array<unsigned char, N> > { public: size_t operator()(array<unsigned char, N> v) const { return hash_value(v); } };
template<size_t N> class hash<const array<unsigned char, N>& > { public: size_t operator()(const array<unsigned char, N>& v) const { return hash_value(v); } };

#	endif // _ARRAY_

//!!!R #endif

template<class U, class V> class hash<pair<U, V> > {
public:
	size_t operator()(const pair<U, V>& v) const {
		return hash<U>()(v.first) ^ hash<V>()(v.second);
	}
};

template<class T> class hash<Ext::ptr<T> >{ public: size_t operator()(const Ext::ptr<T>& v) const { return hash_value(v); } };

//}

#	ifdef _HAS_TR1
	END_STD_TR1
#	else
	}	
#	endif
	
#endif // defined(_HAS_TR1) || _MSC_VER>=1500


#endif

namespace std {

#if 0
#	if defined(_MSC_VER) && _MSC_VER >= 1500
	namespace tr1 {
		template <class T> class hash : public unary_function<T, size_t> {
		public:
			size_t operator()(const T& _Keyval) const { return hash_value(_Keyval); }
		};

	}
	using stdext::hash_value;
#	elif !defined(_MSC_VER)
	namespace tr1 {
		using Ext::hash_value;


		/*!!!
		template<class T> struct hash<const T&> {
			size_t operator()(const T& v) const {
				return hash_value(v);
			}
		};*/
	}
#	endif
#endif



#	ifndef _HASH_SEED
#		define _HASH_SEED	((size_t)0xdeadbeef)
#	endif

}

#if !UCFG_STDSTL || UCFG_WCE//!!!
#	if UCFG_FRAMEWORK
#		include "ext-hashimp.h"
#		if !UCFG_STDSTL || UCFG_WCE //!!! && !UCFG_WDM
#			include <el/stl/unordered_map>
#		endif
#	endif // UCFG_FRAMEWORK

#endif // !UCFG_STDSTL


#if !defined(_CRTBLD) || (UCFG_CRT=='U')


//!!!R #include "ext_messages.h"

namespace Ext {

class Stream;

#if !UCFG_MINISTL && !defined(_CRTBLD)
class EXT_CLASS CTraceWriter {
public:
	CTraceWriter(int level, const char* funname = 0) noexcept;
	CTraceWriter(Stream *pos) noexcept;
	~CTraceWriter() noexcept;
	std::ostream& Stream() noexcept;
	void VPrintf(const char* fmt, va_list args);
	void Printf(const char* fmt, ...);
	static CTraceWriter& AFXAPI CreatePreObject(char *obj, int level, const char* funname);
	static void AFXAPI StaticPrintf(int level, const char* funname, const char* fmt, ...);
private:
	std::ostringstream m_os;
	Ext::Stream *m_pos;
	bool m_bPrintDate;

	void Init(const char* funname);
};
#endif // !UCFG_MINISTL

#if UCFG_WIN32 && !defined(_CRTBLD)
	AFX_API int AFXAPI Win32Check(LRESULT i);
	AFX_API bool AFXAPI Win32Check(BOOL b, DWORD allowableError);
#endif

template <size_t n>
struct int_presentation {
};

template <> struct int_presentation<4> {
	typedef int32_t type;
};

template <> struct int_presentation<8> {
	typedef int64_t type;
};

/*!!!
template<> struct type_presentation<int> {
#if INT_MAX == 2147483647
	typedef int32_t type;
#endif
};

template<> struct type_presentation<long> {
#if LONG_MAX == 2147483647
	typedef int32_t type;
#endif
};

template<> struct type_presentation<unsigned long> {
#if ULONG_MAX == 0xffffffffUL
	typedef uint32_t type;
#endif
};
*/

} // Ext::

#if UCFG_FRAMEWORK
#	include "ext-stream.h"

#	if defined(WIN32)		// && UCFG_EXTENDED //!!!
#		include "win32/ext-cmd.h"
#	endif

#	include "ext-base.h"
#	include "ext-os.h"
#	include "ext-core.h"
#	include "binary-reader-writer.h"
#	include "datetime.h"


#	ifdef _WIN32
#		include "win32/ext-registry.h"
#	endif


#	if !UCFG_WDM

#		include "ext-fw.h"
//#		include "ext-net.h"

#		if UCFG_EXTENDED || UCFG_USE_LIBCURL
//#			include "ext-http.h"
#		endif

#		if UCFG_WIN32
//#			include "win32/ext-win.h"	
#			if !UCFG_WCE
//#				include "win32/ext-full-win.h"
#			endif

#			if UCFG_WND
//#				include "win32/extwin32.h"
//#				include "win32/ext-wnd.h"

#				if UCFG_EXTENDED && UCFG_WIN_HEADERS
//#					include "el/gui/ext-image.h"
//#					include "win32/extmfc.h"
#				endif

#			endif

#			if UCFG_EXTENDED
#				if UCFG_COM_IMPLOBJ
//#					include "win32/extctl.h"
#				endif
//!!!R				include "win32/simplemapi.h"
//!!!R#				if !UCFG_WCE
//!!!R#					include "win32/toolbar.h"
//!!!R#				endif
//!!!R#				include "lng.h"
#			endif
#		endif

#		include "ext-app.h"

/*!!!?
#		if UCFG_XML
#			include "xml.h"
#		endif
*/

#		if UCFG_WIN32 && UCFG_EXTENDED && !UCFG_WCE
//!!!R#			include "lng.h"
//!!!R#			include "docview.h"
#		endif


#	include "ext-protocols.h"

#	endif

#endif // UCFG_FRAMEWORK


namespace Ext {

#if UCFG_FRAMEWORK

class StreamToBlob : MemoryStream, public BinaryWriter {
public:
	using MemoryStream::get_Blob;
	using MemoryStream::operator ConstBuf;

	StreamToBlob()
		:	BinaryWriter(static_cast<MemoryStream&>(*this))
	{}

	StreamToBlob& Ref() { return *this; }
};

class CTraceCategory {
public:
	String m_name;
	bool Enabled;

	explicit CTraceCategory(RCString name)
		:	m_name(name)
		,	Enabled(true)
	{}
};

struct TraceThreadContext;

class DbgFun {
public:
	DbgFun(const char *funName)
		:	m_funName(funName)
	{
		OutFormat("%d%% %ds>%%s\n", true);
	}

	~DbgFun() {
		OutFormat("%d%% %ds<%%s\n", false);
	}
private:
	TraceThreadContext *m_ctx;
	const char *m_funName;

	void OutFormat(const char *fs, bool bEnter);
};
#endif

#ifdef _DEBUG
#	define DBG_FUN  DbgFun _dbgFun(__FUNCTION__);
#else
#	define DBG_FUN
#endif


inline int BitScanReverse(uint32_t v) {
	unsigned long idx;
	bool b = ::_BitScanReverse(&idx, v);
	return int(idx | -!b);
}

#ifdef _M_X64
inline int BitScanReverse64(uint64_t v) {
	unsigned long idx;
	bool b = ::_BitScanReverse64(&idx, v);
	return int(idx | -!b);
}
#endif


} // Ext::


#define EXT_STR(expr) (static_cast<std::ostringstream&>(const_cast<std::ostringstream &>(static_cast<const std::ostringstream &>(std::ostringstream())) << expr)).str()
#define EXT_BIN(expr) ConstBuf(static_cast<Ext::StreamToBlob&>(Ext::StreamToBlob().Ref() << expr))


#	if UCFG_ATL_EMULATION && defined(WIN32) && UCFG_FRAMEWORK && !defined(WDM_DRIVER)
//#		include "win32/ext-atl.h"
#	endif

#endif  // _CRTBLD



#endif // _CRTBLD

#endif // _M_CEE

inline void * __cdecl operator new(size_t size, int id, const char *file, int line) {
	return operator new(size);
}

inline void * __cdecl operator new[](size_t size, int id, const char *file, int line) {
	return operator new[](size);
}


#if UCFG_DETECT_MISMATCH
#	if UCFG_COMPLEX_WINAPP
#		pragma detect_mismatch("UCFG_COMPLEX_WINAPP", "1")
#	else
#		pragma detect_mismatch("UCFG_COMPLEX_WINAPP", "0")
#	endif
#	if UCFG_EXTENDED
#		pragma detect_mismatch("UCFG_EXTENDED", "1")
#	else
#		pragma detect_mismatch("UCFG_EXTENDED", "0")
#	endif
#	if UCFG_WIN_MSG
#		pragma detect_mismatch("UCFG_WIN_MSG", "1")
#	else
#		pragma detect_mismatch("UCFG_WIN_MSG", "0")
#	endif
#	if UCFG_GUI
#		pragma detect_mismatch("UCFG_GUI", "1")
#	else
#		pragma detect_mismatch("UCFG_GUI", "0")
#	endif
#	if UCFG_OLE
#		pragma detect_mismatch("UCFG_OLE", "1")
#	else
#		pragma detect_mismatch("UCFG_OLE", "0")
#	endif
#endif // UCFG_DETECT_MISMATCH

#if UCFG_STL && !UCFG_STDSTL && !defined(_CRTBLD) && UCFG_FRAMEWORK && _HAS_EXCEPTIONS
#	include EXT_HEADER_FUTURE			// uses AutoResetEvent
#endif // UCFG_STL && !UCFG_STDSTL

