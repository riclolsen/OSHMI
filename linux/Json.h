// TODO:
// 3. feature: support _variaint_t array and object

#pragma once

#include <string>
#include <map>
#include <vector>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>
#include <wctype.h>
#define _i64toa(x,y,z) sprintf(y,"%ll",x);
#define _ui64toa(x,y,z) sprintf(y,"%llu",x);


// forward declaration of json value
class JSONVALUE;

//typedef unsigned long DWORD; by BGT

//typedef std::wstring wstring, *pwstring;
typedef std::string string, *pstring;

// json object is a collection of name-value pair
typedef std::map<string, JSONVALUE> JSONOBJECT, *LPJSONOBJECT;

// json array is a collection of json value
typedef std::vector<JSONVALUE> JSONARRAY, *LPJSONARRAY;


#define DWORD       unsigned long
/*#define VARTYPE     unsigned short
#define VT_NULL    
*/

enum JVARTYPE{ 
JVT_EMPTY = 0,
JVT_NULL = 1,
JVT_I2 = 2,
JVT_I4 = 3,
JVT_R4 = 4,
JVT_R8 = 5,
JVT_CY = 6,
JVT_DATE = 7,
JVT_BSTR = 8,
JVT_DISPATCH = 9,
JVT_ERROR = 10,
JVT_BOOL = 11,
JVT_VARIANT = 12,
JVT_UNKNOWN = 13,
JVT_DECIMAL = 14,
JVT_I1 = 16,
JVT_UI1 = 17,
JVT_UI2 = 18,
JVT_UI4 = 19,
JVT_I8 = 20,
JVT_UI8 = 21,
JVT_INT = 22,
JVT_UINT = 23,
JVT_VOID = 24,
JVT_HRESULT = 25,
JVT_PTR = 26,
JVT_SAFEARRAY = 27,
JVT_CARRAY = 28,
JVT_USERDEFINED = 29,
JVT_LPSTR = 30,
JVT_LPWSTR = 31,
JVT_RECORD = 36,
JVT_INT_PTR = 37,
JVT_UINT_PTR = 38,
JVT_FILETIME = 64,
JVT_BLOB = 65,
JVT_STREAM = 66,
JVT_STORAGE = 67,
JVT_STREAMED_OBJECT = 68,
JVT_STORED_OBJECT = 69,
JVT_BLOB_OBJECT = 70,
JVT_CF = 71,
JVT_CLSID = 72,
JVT_VERSIONED_STREAM = 73,
JVT_BSTR_BLOB = 0xfff,
JVT_VECTOR = 0x1000,
JVT_ARRAY = 0x2000,
JVT_BYREF = 0x4000,
JVT_RESERVED = 0x8000,
JVT_ILLEGAL = 0xffff,
JVT_ILLEGALMASKED = 0xfff,
JVT_TYPEMASK = 0xfff
} ;


#define _I8_MIN     (-127 - 1) /* minimum signed 8 bit value */
#define _I8_MAX     127 /* maximum signed 8 bit value */
#define _UI8_MAX    0xffu /* maximum unsigned 8 bit value */

#define _I16_MIN    (-32767 - 1) /* minimum signed 16 bit value */
#define _I16_MAX    32767 /* maximum signed 16 bit value */
#define _UI16_MAX   0xffff /* maximum unsigned 16 bit value */

#define _I32_MIN    (-2147483647 - 1) /* minimum signed 32 bit value */
#define _I32_MAX    2147483647 /* maximum signed 32 bit value */
#define _UI32_MAX   0xffffffff /* maximum unsigned 32 bit value */


// json type can be
enum JSONTYPE {
	JT_UNDEFINED = -1,	// undefined
	JT_NULL = 0,		// null
	JT_STRING,			// string
	JT_NUMBER,			// number
	JT_BOOL,			// true/false
	JT_OBJECT,			// json object
	JT_ARRAY			// json array
};

// defaults
#define JSON_FLAG_STRICT			0x00000000	// default
#define JSON_FLAG_ISODATE			0x00000000	// default

// stringify flags
#define JSON_FLAG_ALLOWUNQUOTENAME	0x00000001
#define JSON_FLAG_LOCALEDATE		0x00000002
#define JSON_FLAG_PRETTYPRINTTAB	0x00000004
#define JSON_FLAG_PRETTYPRINTSPACE	0x00000008

// parse flags
#define JSON_FLAG_ALLOWHEXNUMBER	0x00000010	// allow 0x and 0X
#define JSON_FLAG_ALLOWOCTNUMBER	0x00000020	// allow 0n
#define JSON_FLAG_ALLOWWSCNTRLCHAR	0x00000040	// allow white space control character: 0x09-0x0d
#define JSON_FLAG_ALLOWSINGLEQUOTE	0x00000080	// allow single quote for string

#define JSON_FLAG_LOOSE				(JSON_FLAG_ALLOWUNQUOTENAME | JSON_FLAG_ALLOWWSCNTRLCHAR | JSON_FLAG_ALLOWSINGLEQUOTE)

// json string return
#define JSON_VALID				1
#define JSON_INVALID			(-1)
#define JSON_CHECKAGAIN			0

#define JSON_OBJECT_START		"{"
#define JSON_OBJECT_END			"}"
#define JSON_ARRAY_START		"["
#define JSON_ARRAY_END			"]"
#define JSON_ARRAY_SEPARATOR	","
#define JSON_VALUE_SEPARATOR	":"
#define JSON_QUOTE				"\""
#define JSON_SINGLEQUOTE		"'"
#define JSON_LITERAL_TRUE		"true"
#define JSON_LITERAL_FALSE		"false"
#define JSON_LITERAL_NULL		"null"
#define JSON_NEWLINE			"\r\n"
#define JSON_SPACE				" "
#define JSON_INDENTTAB			"\t"

// json parser error
typedef struct JSONERROR {
	DWORD nLine;
	DWORD nColumn;
	DWORD nPosition;
	string szDesc;
} JSONERROR, *LPJSONERROR;

// json stringify format
typedef struct JSONFORMAT {
	DWORD nIndentLevel;				// formatting
	DWORD nIndentSpace;				// number of space character to use as indent if JSON_FLAG_PRETTYPRINTSPACE
	int nDecimalPoint;				// number of decimal point for floating point
	const char* szNumberFormat;	// floting point formatting
	DWORD nDateFormat;				// date formatting

	JSONFORMAT() {
		nIndentLevel = 0;
		nIndentSpace = 2;
		nDecimalPoint = 6;
		szNumberFormat = "%.*f";
		nDateFormat = 11;			// date = 1, time = 2, zerotime = 4, millisecond = 8
	};
} JSONFORMAT, *LPJSONFORMAT;

enum JSONSCOPE {
	JS_NONE,
	JS_COMMENT,
		JS_COMMENTBLOCK,
		JS_COMMENTLINE,
	JS_OBJECT,
		JS_NAME,
		JS_NAMESTRICT,
		JS_NAMELOOSE,
		JS_NAME_END,
	JS_ARRAY,
	JS_VALUE,
		JS_VALUEOTHER
};

typedef struct JSONSCOPES {
	JSONSCOPE nScope;
	JSONSCOPE nSubScope;
	JSONVALUE* pValue;
} JSONSCOPES, *LPJSONSCOPES;

typedef std::vector<JSONSCOPES> JSONSTAGE, *LPJSONSTAGE;

// json value
class JSONVALUE
{
public:
	// member
#pragma region member
	JVARTYPE vt;
	JSONTYPE jt;
	union {
		string*	strVal;
		union {
			int64_t				llVal;	// signed whole number
			uint64_t	ullVal;	// unsigned whole number
			double				dblVal;	// floating point number
		} numVal;
		bool		boolVal;
		JSONOBJECT*	objVal;
		JSONARRAY*	arVal;
	} val;
#pragma endregion

	// constructor
#pragma region constructor
	JSONVALUE();
	JSONVALUE(const JSONTYPE nType);
	JSONVALUE(const JSONVALUE& src);
	//JSONVALUE(const wstring& src);
	JSONVALUE(const string& src);
	//JSONVALUE(const wchar_t* src);
	JSONVALUE(const char* src);
	//JSONVALUE(const DECIMAL& src);
	JSONVALUE(const double src);
	JSONVALUE(const float src);
	JSONVALUE(const int64_t src);
	JSONVALUE(const long src);
	JSONVALUE(const int src);
	JSONVALUE(const short src);
	JSONVALUE(const char src);
	JSONVALUE(const uint64_t src);
	JSONVALUE(const unsigned long src);
	JSONVALUE(const unsigned short src);
	JSONVALUE(const unsigned int src);
	JSONVALUE(const unsigned char src);
	JSONVALUE(const bool src);
	JSONVALUE(const JSONOBJECT& src);
	JSONVALUE(const JSONARRAY& src);
	//JSONVALUE(const _variant_t& src);
#pragma endregion

	// destructor
#pragma region destructor
	~JSONVALUE();
	void Clear();
#pragma endregion

	// assignment
#pragma region assignment
	JSONVALUE& operator =(const JSONVALUE& src);
	//JSONVALUE& operator =(const wstring& src);
	JSONVALUE& operator =(const string& src);
	//JSONVALUE& operator =(const wchar_t* src);
	JSONVALUE& operator =(const char* src);
	//JSONVALUE& operator =(const DECIMAL& src);
	JSONVALUE& operator =(const double src);
	JSONVALUE& operator =(const float src);
	JSONVALUE& operator =(const int64_t src);
	JSONVALUE& operator =(const long src);
	JSONVALUE& operator =(const int src);
	JSONVALUE& operator =(const short src);
	JSONVALUE& operator =(const char src);
	JSONVALUE& operator =(const uint64_t src);
	JSONVALUE& operator =(const unsigned long src);
	JSONVALUE& operator =(const unsigned int src);
	JSONVALUE& operator =(const unsigned short src);
	JSONVALUE& operator =(const unsigned char src);
	JSONVALUE& operator =(const bool src);
	JSONVALUE& operator =(const JSONOBJECT& src);
	JSONVALUE& operator =(const JSONARRAY& src);
	//JSONVALUE& operator =(const _variant_t& src);

	// object assignment
	JSONVALUE& operator [](const string& src);
	JSONVALUE& operator [](const char* src);

	// array
	void Push(const JSONVALUE& src);
#pragma endregion

	// general
	size_t Size();
	JSONVALUE& At(const size_t pos);
	bool IsUndefined();

	// conversion
#pragma region conversion
	// conversion, wide string version
	const char* ToString(string& sz, const DWORD nFlag = 0, const JSONFORMAT* pFormat = NULL);
	bool Parse(const string& sz, const DWORD nFlag = 0, JSONERROR *pError = NULL);

	// conversion, ansi string version
	//const char* ToString(string& sz, const DWORD nFlag = 0, const JSONFORMAT* pFormat = NULL);
	//bool Parse(const string& sz, const DWORD nFlag = 0, JSONERROR *pError = NULL);

	// conversion, variant
	//bool ToVariant(_variant_t& v, const DWORD nFlag = 0);
#pragma endregion

protected:
	// string
	const char* FormatString(string &sz, const string &szSrc, const DWORD nFlag, const bool bName);
	const char* TrimString(string& sz, const int nLR);
	bool IsHexChar(const char c);
	const char* EscapeChar(const char c, const char cQuote);
	char UnescapeChar(const string& sz);
	int UnescapeString(string& sz);
	//const char* ConvertAW(string& dest, const char* src, const UINT srccp = CP_UTF8, const bool bRemoveBom = true);
	//const char* ConvertWA(string& dest, const char* src, const UINT destcp = CP_UTF8, const bool bRemoveBom = true);
	int JsonFromString(JSONVALUE* dst, const string& sz, const DWORD nFlag);
	
	bool HasFlag(const DWORD nFlag, const DWORD nSrc);
	void SetError(JSONERROR *pError, const string& sz, const string::size_type pos, const char* pszDesc);

	// value
	void SetString(JSONVALUE* p);
	void SetObject(JSONVALUE* p);
	void SetArray(JSONVALUE* p);

	// stage
	JSONSTAGE m_stage;
	int GetCurrentScope(JSONSCOPES *pScope);
	void SetNextScope(const JSONSCOPE nScope, const JSONVALUE *pValue);
	bool SetCurrentSubScope(const JSONSCOPE nSubScope);
	DWORD RevertScope(const DWORD nLevel);
};
