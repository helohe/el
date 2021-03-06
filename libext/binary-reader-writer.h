#pragma once

#include EXT_HEADER(set)

#if UCFG_OLE
	typedef union tagCY CY;
#endif

namespace Ext {

using std::true_type;
using std::false_type;
using std::is_arithmetic;

class BinaryReader;
class BinaryWriter;

class AFX_CLASS CPersistent {
public:
	virtual ~CPersistent() {
	}

	virtual void Write(BinaryWriter& wr) const {
		Throw(E_NOTIMPL);
	}

	virtual void Read(const BinaryReader& rd) {
		Throw(E_NOTIMPL);
	}
};

class BinaryReader : noncopyable {
	typedef BinaryReader class_type;
public:
	const Stream& BaseStream;
	Ext::Encoding& Encoding;

	explicit BinaryReader(const Stream& stm, Ext::Encoding& enc = Encoding::UTF8)
		:	BaseStream(stm)
		,	Encoding(enc)
	{}

	const BinaryReader& EXT_FASTCALL Read(void *buf, size_t count) const { BaseStream.ReadBuffer(buf, count); return *this; }
	
	const BinaryReader& operator>>(char& v) const {
		Read(&v, 1);
		return *this;
	}

	const BinaryReader& operator>>(signed char& v) const {
		Read(&v, 1);
		return *this;
	}

	const BinaryReader& operator>>(unsigned char& v) const {
		Read(&v, 1);
		return *this;
	}

	byte ReadByte() const {
		byte r;
		*this >> r;
		return r;
	}

	bool ReadBoolean() const {
		return ReadByte();
	}

	template <typename T> const BinaryReader& ReadType(T& v, false_type) const {
		Read(v);
		return *this;
	}

	template <typename T> const BinaryReader& ReadType(T& v, true_type) const {
		Read(&v, sizeof v);
		v = letoh(v);
		return *this;
	} 

	int16_t ReadInt16() const {
		int16_t r;
		ReadType(r, std::true_type());
		return r;
	}

	uint16_t ReadUInt16() const {
		uint16_t r;
		ReadType(r, true_type());
		return r;
	}

	int32_t ReadInt32() const {
		int32_t r;
		ReadType(r, true_type());
		return r;
	}

	uint32_t ReadUInt32() const {
		uint32_t r;
		ReadType(r, true_type());
		return r;
	}

	int64_t ReadInt64() const {
		int64_t r;
		ReadType(r, true_type());
		return r;
	}

	uint64_t ReadUInt64() const {
		uint64_t r;
		ReadType(r, true_type());
		return r;
	}

	float ReadSingle() const {
		float r;
		ReadType(r, true_type());
		return r;
	}

	double ReadDouble() const {
		double r;
		ReadType(r, true_type());
		return r;
	}

	uint64_t Read7BitEncoded() const;
	
	size_t ReadSize() const {
		uint64_t v = Read7BitEncoded();
		if (v > (std::numeric_limits<size_t>::max)())
			Throw(HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW));
		return (size_t)v;
	}

	String ReadString() const;

	template <typename K, typename T, typename P, typename A>
	void Read(std::map<K, T, P, A>& m) const {
		m.clear();
		size_t count = ReadSize();
		for (size_t i=0; i<count; ++i) {
			pair<K, T> pp;
			*this >> pp;
			m.insert(pp);
		}
	}

	template <typename K, typename T, typename H, typename E>
	void Read(std::unordered_map<K, T, H, E>& m) const {
		m.clear();
		size_t count = ReadSize();
		for (size_t i=0; i<count; ++i) {
			pair<K, T> pp;
			*this >> pp;
			m.insert(pp);
		}
	}

	template <typename T, typename P, typename A>
	void Read(std::set<T, P, A>& s) const {
		s.clear();
		size_t count = ReadSize();
		for (size_t i=0; i<count; ++i) {
			T el;
			*this >> el;
			s.insert(el);
		}
	}

	template <typename T, typename H, typename E>
	void Read(std::unordered_set<T, H, E>& s) const {
		s.clear();
		size_t count = ReadSize();
		for (size_t i=0; i<count; ++i) {
			T el;
			*this >> el;
			s.insert(el);
		}
	}

	template <typename T> void Read(T& pers) const {
		pers.Read(*this);
	}

	void Read(CPersistent& pers) const {
		pers.Read(*this);
	}

	template <class T> const BinaryReader& ReadStruct(T& t) const {
		return Read(&t, sizeof(T));
	}

	const BinaryReader& operator>>(Blob& blob) const;

	Blob ReadBytes(size_t size) const {
		Blob r(0, size);
		Read(r.data(), size);
		return r;
	}

	Blob ReadToEnd() const;

#if UCFG_OLE
	const BinaryReader& operator>>(CY& v) const;
	const BinaryReader& operator>>(COleVariant& v) const; 
#endif

#if UCFG_COM
	COleVariant ReadVariantOfType(VARTYPE vt) const;
#endif

#ifdef _NATIVE_WCHAR_T_DEFINED
	const BinaryReader& operator>>(wchar_t& v) const { return Read(&v, sizeof v); }
#endif

#ifdef GUID_DEFINED
	void Read(GUID& v) const { Read(&v, sizeof v); }
#endif

	template <typename T> void Read(std::vector<T>& ar) const {
		size_t count = ReadSize();
		ar.resize(count);
		for (size_t i=0; i<count; ++i)
			*this >> ar[i];
	}
protected:
	

/*!!!	Stream& operator>>(bool& v) { v = false; return Read(&v, 1); } 
	Stream& operator>>(char& v) { return Read(&v, sizeof v); }
	Stream& operator>>(byte& v) { return Read(&v, sizeof v); }
	Stream& operator>>(short& v) { return Read(&v, sizeof v); }
	Stream& operator>>(uint16_t& v) { return Read(&v, sizeof v); }
	Stream& operator>>(int& v) { return Read(&v, sizeof v); }
	Stream& operator>>(LONG& v) { return Read(&v, sizeof v); }
	Stream& operator>>(uint32_t& v) { return Read(&v, sizeof v); }
	Stream& operator>>(unsigned long v) { return Read(&v, sizeof v); }

	//!!!  Stream& operator>>(size_t& v) { return Read(&v, sizeof v); }
	Stream& operator>>(int64_t& v) { return Read(&v, sizeof v); }
	Stream& operator>>(u_int64_t& v) { return Read(&v, sizeof v); }
	Stream& operator>>(float& v) { return Read(&v, sizeof v); }
	Stream& operator>>(double& v) { return Read(&v, sizeof v); }
	Stream& operator>>(GUID& v) { return Read(&v, sizeof v); }
	*/

};

class BinaryWriter : noncopyable {
	typedef BinaryWriter class_type;
public:
	Stream& BaseStream;
	Ext::Encoding& Encoding;

	explicit BinaryWriter(Stream& stm, Ext::Encoding& enc = Encoding::UTF8)
		:	BaseStream(stm)
		,	Encoding(enc)
	{}

	BinaryWriter& Ref() { return *this; }


	BinaryWriter& EXT_FASTCALL Write(const void *buf, size_t count) { BaseStream.WriteBuffer(buf, count); return *this; }
	
	template <typename T> void Write(const T& pers) {
		pers.Write(*this);
	}

	void Write(const CPersistent& pers) {
		pers.Write(*this);
	}

#if UCFG_OLE
	void Write(const CY& v);
	void Write(const VARIANT& v);

	void Write(const COleVariant& v) { Write((const VARIANT&)v); }
#endif

	template <typename T> void Write(const std::vector<T>& ar) {
		size_t count = ar.size();
		WriteSize(count);
		for (size_t i=0; i<count; ++i)
			*this << ar[i];
	}

	template <typename K, typename T, typename P, typename A> void Write(const std::map<K, T, P, A>& m) {
		WriteSize(m.size());
		for (typename std::map<K, T, P, A>::const_iterator it=m.begin(), e=m.end(); it!=e; ++it)
			*this << *it;
	}

	template <typename K, typename T, typename P, typename A> void Write(const std::unordered_map<K, T, P, A>& m) {
		WriteSize(m.size());
		for (typename std::map<K, T, P, A>::const_iterator it=m.begin(), e=m.end(); it!=e; ++it)
			*this << *it;
	}

	template <typename T> BinaryWriter& WriteType(const T& v, false_type) {
		Write(v);
		return *this;
	}

	template <typename T> BinaryWriter& WriteType(const T& v, true_type) {
		T vle = htole(v);
		return Write(&vle, sizeof vle);
	}

	void Write(byte v) {
		Write(&v, 1);
	}

	void Write7BitEncoded(uint64_t v);
	void WriteSize(size_t size) { Write7BitEncoded(size); }

	template <class T> BinaryWriter& WriteStruct(const T& t) {
		return Write(&t, sizeof(T));
	}

	BinaryWriter& operator<<(const ConstBuf& mb);

	BinaryWriter& operator<<(const Blob& blob) {
		return *this << ConstBuf(blob);
	}

	void WriteString(RCString v);
	
#if UCFG_COM
	void WriteVariantOfType(const VARIANT& v, VARTYPE vt);
#endif

#ifdef GUID_DEFINED
	void Write(const GUID& v) { Write(&v, sizeof v); }
#endif

protected:
	

/*!!!


__forceinline Stream& operator<<(const char& v)     { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const byte& v)     { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const short& v)    { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const uint16_t& v)     { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const int& v)      { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const LONG& v)     { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const uint32_t& v)    { return Write(&v, sizeof v); }
//!!!	__forceinline Stream& operator<<(size_t v)			{ return Write(&v, sizeof v); }


__forceinline Stream& operator<<(const int64_t& v) { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const u_int64_t& v) { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const float& v)    { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const double& v)   { return Write(&v, sizeof v); }
__forceinline Stream& operator<<(const GUID& v)     { return Write(&v, sizeof v); }

*/	
};



class BitReader {
public:
	const Stream& BaseStream;

	BitReader(const Stream& stm)
		:	BaseStream(stm)
		,	m_n(0)
	{
	}

	bool ReadBit() {
		if (m_n == 0)
			ReadByte();
		return (m_b >> --m_n) & 1;
	}
private:
	byte m_b;
	int m_n;

	void ReadByte() {
		BaseStream.ReadBuffer(&m_b, 1);
		m_n = 8;
	}
};

inline BinaryWriter& operator<<(BinaryWriter& wr, bool v) {			//!!!V need to test
	 return wr.Write(&v, 1);
}

inline BinaryWriter& operator<<(BinaryWriter& wr, char v) {
	return wr.Write(&v, 1);
}

inline BinaryWriter& operator<<(BinaryWriter& wr, signed char v) {
	return wr.Write(&v, 1);
}

inline BinaryWriter& operator<<(BinaryWriter& wr, unsigned char v) {
	return wr.Write(&v, 1);
}

#ifdef _NATIVE_WCHAR_T_DEFINED
inline BinaryWriter& operator<<(BinaryWriter& wr, const wchar_t& v)  {				//!!! sizeof(wchar_t) is 2 or 4
	return wr.Write(&v, sizeof v);
}
#endif

template <typename T> BinaryWriter& operator<<(BinaryWriter& wr, const T& v) {
	return wr.WriteType(v,  typename is_arithmetic<T>::type() );
}

template<> inline BinaryWriter& operator<< <bool>(BinaryWriter& wr, const bool& v) { return wr.Write(&v, 1); } 


template <typename T, typename U>
BinaryWriter& operator<<(BinaryWriter& wr, const std::pair<T, U>& pp) {
	return wr << pp.first << pp.second;
}

template <typename T, typename U>
const BinaryReader& operator>>(const BinaryReader& rd, std::pair<T, U>& pp) {
	return rd >> pp.first >> pp.second;
}

template <typename T> const BinaryReader& operator>>(const BinaryReader& rd, T& v) {
	return rd.ReadType(v,  typename is_arithmetic<T>::type() );
} 

template<> inline const BinaryReader& operator>><bool>(const BinaryReader& rd, bool& v) { v = false; return rd.Read(&v, 1); } 


inline BinaryWriter& operator<<(BinaryWriter& wr, RCString s) {
	wr.WriteString(s);
	return wr;
}

inline const BinaryReader& operator>>(const BinaryReader& rd, String& s) {
	s = rd.ReadString();
	return rd;
}

} // Ext::


