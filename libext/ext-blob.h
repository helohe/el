/*######   Copyright (c) 1997-2015 Ufasoft  http://ufasoft.com  mailto:support@ufasoft.com,  Sergey Pavlov  mailto:dev@ufasoft.com ####
#                                                                                                                                     #
# 		See LICENSE for licensing information                                                                                         #
#####################################################################################################################################*/

#pragma once

#include EXT_HEADER_SYSTEM_ERROR

#if UCFG_COM
	typedef WCHAR OLECHAR;
	typedef OLECHAR *BSTR;
	typedef OLECHAR *LPOLESTR;
	typedef double DATE;

	typedef struct tagVARIANT VARIANT;
	class _bstr_t;
	typedef unsigned short VARTYPE;
#endif

//!!!#include "libext.h"

namespace Ext {


const std::error_category& AFXAPI ext_category();

inline std::error_code make_error_code(ExtErr v) { return std::error_code(int(v), ext_category()); }
inline std::error_condition make_error_condition(ExtErr v) { return std::error_condition(int(v), ext_category()); }


} // Ext::

namespace std { template<> struct std::is_error_condition_enum<Ext::ExtErr> : true_type {}; }

namespace Ext {

DECLSPEC_NORETURN __forceinline void ThrowImp(ExtErr errval) { ThrowImp(make_error_code(errval)); }
DECLSPEC_NORETURN __forceinline void ThrowImp(ExtErr errval, const char *funname, int nLine) { ThrowImp(make_error_code(errval), funname, nLine); }

#if !UCFG_DEFINE_THROW
DECLSPEC_NORETURN __forceinline void AFXAPI Throw(ExtErr v) { ThrowImp(v); }
#endif

using std::atomic;

class CStringBlobBuf;

class COleVariant;



class CBlobBufBase {
public:
	atomic<int> m_aRef;

#if UCFG_STRING_CHAR == 16
	typedef Char16 Char;
#elif UCFG_STRING_CHAR == 32
	typedef Char32 Char;
#else
	typedef wchar_t Char;
#endif	

	CBlobBufBase()
		: m_aRef(1)
	{}

	void AddRef() EXT_FAST_NOEXCEPT {
		++m_aRef;
	}

#if UCFG_BLOB_POLYMORPHIC
	virtual ~CBlobBufBase() {}
	virtual Char *GetBSTR() =0; //!!!was BSTR
	virtual CBlobBufBase *Clone() =0;
	virtual size_t GetSize() =0;
	virtual CBlobBufBase *SetSize(size_t size) =0;

	virtual CStringBlobBuf *AsStringBlobBuf();
	virtual void Attach(Char *bstr); //!!!was BSTR
	virtual Char *Detach(); //!!!was BSTR
#endif
};

class EXTAPI CStringBlobBuf : public CBlobBufBase {
public:
#ifdef WDM_DRIVER
	UNICODE_STRING m_us;
#endif
	atomic<char*> m_apChar;
#ifdef _WIN64  //!!!
	uint32_t m_pad;
#endif
	uint32_t m_size;

	CStringBlobBuf(size_t len = 0);
	CStringBlobBuf(const void *p, size_t len);
	CStringBlobBuf(size_t len, const void *buf, size_t copyLen);

	~CStringBlobBuf() noexcept {
		if (char *p = m_apChar)
			free(p);
	}

	void * AFXAPI operator new(size_t sz);
	__forceinline void operator delete(void *p) { free(p); }
	void * AFXAPI operator new(size_t sz, size_t len, bool);
	__forceinline void operator delete(void *p, size_t, bool) { free(p); }

	CStringBlobBuf *AsStringBlobBuf() { return this; }

	Char *GetBSTR() EXT_FAST_NOEXCEPT { return (Char*)(this+1); } //!!! was BSTR
	size_t GetSize() EXT_FAST_NOEXCEPT { return m_size; }

	CStringBlobBuf *Clone();
	CStringBlobBuf *SetSize(size_t size);
	static CStringBlobBuf* AFXAPI RefEmptyBlobBuf();

};

#if UCFG_BLOB_POLYMORPHIC
	inline void Release(CBlobBufBase *bb) EXT_FAST_NOEXCEPT {				// Allowed only if destructor is virtual
#else
	inline void Release(CStringBlobBuf *bb) EXT_FAST_NOEXCEPT {
#endif
		if (bb && !--(bb->m_aRef))
			delete bb;
	}


#if UCFG_WIN32 && UCFG_BLOB_POLYMORPHIC && UCFG_COM
class COleBlobBuf : public CBlobBufBase {
public:
	COleBlobBuf();
	~COleBlobBuf();
	void Init(size_t len, const void *buf = 0);
	void Init2(size_t len, const void *buf, size_t copyLen);
	void Empty();
	void Attach(BSTR bstr);
	BSTR Detach();

	Char *GetBSTR() {
		return m_bstr;
	}

	size_t GetSize() { return *((uint32_t*)GetBSTR()-1); }

	CBlobBufBase *Clone();
	CBlobBufBase *SetSize(size_t size);
private:
	BSTR m_bstr;
};
#endif

class EXTAPI Blob {
	typedef Blob class_type;
public:
	typedef const byte const_iterator;

#if UCFG_BLOB_POLYMORPHIC
	typedef CBlobBufBase impl_class;
#else
	typedef CStringBlobBuf impl_class;
#endif

//!!!?	impl_class * volatile m_pData;				//!!!  not optimal  atomic only for some rare operations
	impl_class * m_pData;				// atomic only for some rare operations

	Blob()
		:	m_pData(CStringBlobBuf::RefEmptyBlobBuf())
	{}

	Blob(const Blob& blob) noexcept;

	Blob(impl_class *pData)
		:	m_pData(pData)
	{}

	Blob(std::nullptr_t)
		:	m_pData(0)
	{}

	Blob(const void *buf, size_t len);
	Blob(const ConstBuf& mb);
	Blob(const Buf& mb);
#if UCFG_COM
	Blob(BSTR bstr);
#endif

#if UCFG_COM
	__forceinline Blob(const VARIANT& v)
		:	m_pData(new CStringBlobBuf)
	{
		SetVariant(v);
	}

	operator COleVariant() const;
#endif

	~Blob();

	Blob(EXT_RV_REF(Blob) rv)
		:	m_pData(exchange(rv.m_pData, nullptr))
	{
	}

	Blob& operator=(EXT_RV_REF(Blob) rv) {
		Release(exchange(m_pData, exchange(rv.m_pData, nullptr)));
		return *this;
	}

	void swap(Blob& x) noexcept {
		std::swap(m_pData, x.m_pData);
	}

	operator ConstBuf() const noexcept { return m_pData ? ConstBuf(constData(), Size) : ConstBuf(0, 0); }

	void AssignIfNull(const Blob& val);
	Blob& operator=(const Blob& val);
	Blob& operator=(const ConstBuf& mb) { return operator=(Blob(mb)); }

	bool operator==(const ConstBuf& cbuf) const {
		return Ext::operator==(operator ConstBuf(), cbuf);
	}

	bool operator==(const Blob& blob) const noexcept;
	bool operator<(const Blob& blob) const noexcept;

	bool operator!=(const Blob& blob) const { return !operator==(blob); }

	Blob& operator+=(const ConstBuf& mb);

	bool operator!() const { return !m_pData; }
	
	static Blob AFXAPI FromHexString(RCString s);

	size_t get_Size() const EXT_FAST_NOEXCEPT { return m_pData->GetSize(); }
	void put_Size(size_t size);
	DEFPROP_CONST(size_t, Size);

	size_t max_size() const noexcept { return SIZE_MAX - 2; }

	// we don't use property feature to explicit call constData() for efficiency
	byte *data();
	__forceinline const byte *data() const noexcept { return (const byte*)m_pData->GetBSTR(); }
	__forceinline const byte *constData() const noexcept { return (const byte*)m_pData->GetBSTR(); }

	const byte *begin() const { return constData(); }
	const byte *end() const { return constData()+Size; }

	byte operator[](size_t idx) const {
		if (idx >= Size)
			Throw(ExtErr::IndexOutOfRange);
		return constData()[idx];
	}

	byte& operator[](size_t idx) {
		if (idx >= Size)
			Throw(ExtErr::IndexOutOfRange);
		return data()[idx];
	}

	void Replace(size_t offset, size_t size, const ConstBuf& mb);
protected:
	void Cow();
#if UCFG_COM
	void SetVariant(const VARIANT& v);
#endif

private:
//	EXPLICIT_OPERATOR_BOOL() const;												// don't public it. Ambiguous type conversions
friend class String;
};

inline void swap(Blob& x, Blob& y) noexcept {
	x.swap(y);
}

inline Blob operator+(const ConstBuf& mb1, const ConstBuf& mb2) {
	Blob r(0, mb1.Size+mb2.Size);
	memcpy(r.data(), mb1.P, mb1.Size);
	memcpy(r.data()+mb1.Size, mb2.P, mb2.Size);
	return r;
}

EXT_API std::ostream& __stdcall operator<<(std::ostream& os, const ConstBuf& cbuf);

inline std::ostream& __stdcall operator<<(std::ostream& os, const Blob& blob) {
	return os << ConstBuf(blob);
}

typedef const Blob& RCBlob;

template <class T>
class StaticList : noncopyable {
public:
	static T *Root;

	T *Next;

	StaticList()
		:	Next(0)
	{}
protected:
	explicit StaticList(bool)
		:	Next(Root)
	{
		Root = static_cast<T*>(this);
	}
};

class ErrorCategoryBase : public std::error_category, public StaticList<ErrorCategoryBase> {
	typedef StaticList<ErrorCategoryBase> base;
public:
	const char *Name;
	int Facility;

	ErrorCategoryBase(const char *name, int facility = FACILITY_UNKNOWN);
	
	const char *name() const noexcept override { return Name; }

	static ErrorCategoryBase* AFXAPI GetRoot();
	static const error_category* AFXAPI Find(int fac);
};





} // Ext::

namespace EXT_HASH_VALUE_NS {
	inline size_t hash_value(const Ext::Blob& blob) {
		return Ext::hash_value(blob.constData(), blob.Size);
	}
}

EXT_DEF_HASH(Ext::Blob)

