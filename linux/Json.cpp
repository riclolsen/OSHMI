//#include "StdAfx.h"
#include "Json.h"
#include <float.h>
//#include "igmem.h"
#include <memory.h>

JSONVALUE::JSONVALUE() 
{
	vt = JVT_NULL;
	jt = JT_NULL;
	val.numVal.llVal = 0;//i64;
}

JSONVALUE::JSONVALUE(const JSONTYPE nType)
{
	jt = nType;
	switch (jt) {
		case JT_STRING:
			vt = JVT_BSTR;
			val.strVal = new string;
			*val.strVal = "";
		//	val.strVal = new wstring;
		//	*val.strVal = "";
			break;
		case JT_NUMBER:
			vt = JVT_I4;
			val.numVal.llVal = 0;//i64;
			break;
		case JT_BOOL:
			vt = JVT_BOOL;
			val.boolVal = false;
			break;
		case JT_OBJECT:
			vt = JVT_DISPATCH;
			val.objVal = new JSONOBJECT;
			break;
		case JT_ARRAY:
			vt = JVT_ARRAY;
			val.arVal = new JSONARRAY;
			break;
		case JT_NULL:
			vt = JVT_NULL;
			val.numVal.llVal = 0;//i64;
			break;
		case JT_UNDEFINED:
		default:
			vt = JVT_EMPTY;
			val.numVal.llVal = 0;//i64;
			break;
	};
}

JSONVALUE::JSONVALUE(const JSONVALUE& src) 
{
	vt = src.vt;
	jt = src.jt;
	switch (jt) {
		case JT_STRING:
			val.strVal = new string;
			*val.strVal = src.val.strVal ? *src.val.strVal : "";
		//	val.strVal = new wstring;
		//	*val.strVal = src.val.strVal ? *src.val.strVal : "";
			break;
		case JT_NUMBER:
			memcpy(&val.numVal, &src.val.numVal, sizeof(val.numVal));
			break;
		case JT_BOOL:
			val.boolVal = src.val.boolVal;
			break;
		case JT_OBJECT:
			val.objVal = new JSONOBJECT;
			if (src.val.objVal && (src.val.objVal->size() > 0))
				val.objVal->insert(src.val.objVal->begin(), src.val.objVal->end());
			break;
		case JT_ARRAY:
			val.arVal = new JSONARRAY;
			if (src.val.arVal && (src.val.arVal->size() > 0))
				val.arVal->insert(val.arVal->end(), src.val.arVal->begin(), src.val.arVal->end());
			break;
		default:
			val.numVal.llVal = 0;//i64;
			break;
	};
}
/*
JSONVALUE::JSONVALUE(const wstring& src) 
{
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new wstring;
	*val.strVal = src;
}
*/
JSONVALUE::JSONVALUE(const string& src)
{
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new string;
    *val.strVal = src;
//	val.strVal = new wstring;
//	ConvertAW(*val.strVal, src.c_str());
}
/*
JSONVALUE::JSONVALUE(const char* src) 
{
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new wstring;
	if (src)
		*val.strVal = src;
}
*/
JSONVALUE::JSONVALUE(const char* src)
{
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new string;
	if (src)
        *val.strVal = src;
//	val.strVal = new wstring;
//	if (src)
//		ConvertAW(*val.strVal, src);
}
/*
JSONVALUE::JSONVALUE(const DECIMAL& src) 
{
	vt = JVT_DECIMAL;
	jt = JT_NUMBER;
	VarR8FromDec(&src, &val.numVal.dblVal);
}
*/
JSONVALUE::JSONVALUE(const double src)
{
	vt = JVT_R8;
	jt = JT_NUMBER;
	val.numVal.dblVal = src;
}

JSONVALUE::JSONVALUE(const float src)
{
	vt = JVT_R4;
	jt = JT_NUMBER;
	val.numVal.dblVal = src;
}

JSONVALUE::JSONVALUE(const int64_t src) 
{
	vt = JVT_I8;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
}

JSONVALUE::JSONVALUE(const long src) 
{
	vt = JVT_I4;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
}

JSONVALUE::JSONVALUE(const int src) 
{
	vt = JVT_I4;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
}

JSONVALUE::JSONVALUE(const short src) 
{
	vt = JVT_I2;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
}

JSONVALUE::JSONVALUE(const char src) 
{
	vt = JVT_I1;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
}

JSONVALUE::JSONVALUE(const uint64_t src) 
{
	vt = JVT_UI8;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
}

JSONVALUE::JSONVALUE(const unsigned long src) 
{
	vt = JVT_UI4;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
}

JSONVALUE::JSONVALUE(const unsigned int src) 
{
	vt = JVT_UI4;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
}

JSONVALUE::JSONVALUE(const unsigned short src) 
{
	vt = JVT_UI2;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
}

JSONVALUE::JSONVALUE(const unsigned char src) 
{
	vt = JVT_UI1;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
}

JSONVALUE::JSONVALUE(const bool src) 
{
	vt = JVT_BOOL;
	jt = JT_BOOL;
	val.boolVal = src;
}

JSONVALUE::JSONVALUE(const JSONOBJECT& src) 
{
	vt = JVT_DISPATCH;
	jt = JT_OBJECT;
	val.objVal = new JSONOBJECT;
	if (src.size() > 0)
		val.objVal->insert(src.begin(), src.end());
}

JSONVALUE::JSONVALUE(const JSONARRAY& src) 
{
	vt = JVT_ARRAY;
	jt = JT_ARRAY;
	val.arVal = new JSONARRAY;
	if (src.size() > 0)
		val.arVal->insert(val.arVal->end(), src.begin(), src.end());
}
/*
JSONVALUE::JSONVALUE(const _variant_t& src) 
{
	vt = src.vt;
	switch (src.vt) {
		// special
		/*case JVT_DATE:
			// save as number
			jt = JT_NUMBER;
			val.numVal.dblVal = src.date;
			break; *//*
		case JVT_NULL:
		case JVT_EMPTY:
			vt = JVT_NULL;
			jt = JT_NULL;
			val.numVal.llVal = 0;//i64;
			break;

		// normal
		case JVT_BSTR:
			jt = JT_STRING;
			val.strVal = new string;
            ConvertAW(*val.strVal, src.bstrVal);
		//	val.strVal = new wstring;
		//	*val.strVal = (LPCTSTR)src.bstrVal;
			break;
		case JVT_DECIMAL:
			jt = JT_NUMBER;
			VarR8FromDec(&src.decVal, &val.numVal.dblVal);
			break;
		case JVT_R4:
			jt = JT_NUMBER;
			val.numVal.dblVal = src.fltVal;
			break;
		case JVT_R8:
			jt = JT_NUMBER;
			val.numVal.dblVal = src.dblVal;
			break;
		case JVT_I1:
			jt = JT_NUMBER;
			val.numVal.llVal = src.cVal;
			break;
		case JVT_I2:
			jt = JT_NUMBER;
			val.numVal.llVal = src.iVal;
			break;
		case JVT_I4:
			jt = JT_NUMBER;
			val.numVal.llVal = src.lVal;
			break;
		case JVT_I8:
			jt = JT_NUMBER;
			val.numVal.llVal = src.llVal;
			break;
		case JVT_UI1:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.bVal;
			break;
		case JVT_UI2:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.uiVal;
			break;
		case JVT_UI4:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.ulVal;
			break;
		case JVT_UI8:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.ullVal;
			break;
		case JVT_BOOL:
			jt = JT_BOOL;
			val.boolVal = (VARIANT_FALSE != src.boolVal);
			break;
		default:
			vt = JVT_NULL;
			jt = JT_NULL;
			val.numVal.llVal = 0;//i64;
			break;
	}
}
*/
// destructor
JSONVALUE::~JSONVALUE() 
{
	Clear();
}

void JSONVALUE::Clear() 
{
	switch (jt) {
		case JT_STRING:
			if (NULL != val.strVal) {
				delete val.strVal;
				val.strVal = NULL;
			}
			break;
		case JT_OBJECT:
			if (NULL != val.objVal) {
				if (val.objVal->size() > 0)
					val.objVal->clear();
				delete val.objVal;
				val.objVal = NULL;
			}
			break;
		case JT_ARRAY:
			if (NULL != val.arVal) {
				if (val.arVal->size() > 0)
					val.arVal->clear();
				delete val.arVal;
				val.arVal = NULL;
			}
			break;
		case JT_NUMBER:
			val.numVal.llVal = 0;//i64;
			break;
		case JT_BOOL:
			val.boolVal = false;
			break;
		default:
			break;
	};
	vt = JVT_NULL;
	jt = JT_NULL;
}

// assignment
JSONVALUE& JSONVALUE::operator =(const JSONVALUE& src) 
{
	Clear();
	vt = src.vt;
	jt = src.jt;
	switch (jt) {
		case JT_STRING:
			val.strVal = new string;
			*val.strVal = src.val.strVal ? *src.val.strVal : "";
		//	val.strVal = new wstring;
		//	*val.strVal = src.val.strVal ? *src.val.strVal : "";
			break;
		case JT_NUMBER:
			memcpy(&val.numVal, &src.val.numVal, sizeof(val.numVal));
			break;
		case JT_BOOL:
			val.boolVal = src.val.boolVal;
			break;
		case JT_OBJECT:
			val.objVal = new JSONOBJECT;
			if (src.val.objVal && (src.val.objVal->size() > 0))
				val.objVal->insert(src.val.objVal->begin(), src.val.objVal->end());
			break;
		case JT_ARRAY:
			val.arVal = new JSONARRAY;
			if (src.val.arVal && (src.val.arVal->size() > 0))
				val.arVal->insert(val.arVal->end(), src.val.arVal->begin(), src.val.arVal->end());
			break;
		default:
			break;
	};
	return *this;
}
/*
JSONVALUE& JSONVALUE::operator =(const wstring& src) 
{
	Clear();
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new wstring;
	*val.strVal = src;
	return *this;
}
*/
JSONVALUE& JSONVALUE::operator =(const string& src) 
{
	Clear();
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new string;
    *val.strVal = src;
//	val.strVal = new wstring;
//	ConvertAW(*val.strVal, src.c_str());
	return *this;
}
/*
JSONVALUE& JSONVALUE::operator =(const char* src) 
{
	Clear();
	vt = JVT_BSTR;
	jt = JT_STRING;
	val.strVal = new wstring;
	if (src)
		*val.strVal = src;
	return *this;
}
*/
JSONVALUE& JSONVALUE::operator =(const char* src) 
{
	Clear();
	vt = JVT_BSTR;
	jt = JT_STRING;
   	val.strVal = new string;
	if (src)
           *val.strVal = src;
//	val.strVal = new wstring;
//	if (src)
//		ConvertAW(*val.strVal, src);
	return *this;
}
/*
JSONVALUE& JSONVALUE::operator =(const DECIMAL& src) 
{
	Clear();
	vt = JVT_DECIMAL;
	jt = JT_NUMBER;
	VarR8FromDec(&src, &val.numVal.dblVal);
	return *this;
}
*/
JSONVALUE& JSONVALUE::operator =(const double src) 
{
	Clear();
	vt = JVT_R8;
	jt = JT_NUMBER;
	val.numVal.dblVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const float src) 
{
	Clear();
	vt = JVT_R4;
	jt = JT_NUMBER;
	val.numVal.dblVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const int64_t src) 
{
	Clear();
	vt = JVT_I8;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const long src) 
{
	Clear();
	vt = JVT_I4;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const int src) 
{
	Clear();
	vt = JVT_I4;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const short src) 
{
	Clear();
	vt = JVT_I2;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const char src) 
{
	Clear();
	vt = JVT_I1;
	jt = JT_NUMBER;
	val.numVal.llVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const uint64_t src) 
{
	Clear();
	vt = JVT_UI8;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const unsigned long src) 
{
	Clear();
	vt = JVT_UI4;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const unsigned int src) 
{
	Clear();
	vt = JVT_UI4;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const unsigned short src) 
{
	Clear();
	vt = JVT_UI2;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const unsigned char src) 
{
	Clear();
	vt = JVT_UI1;
	jt = JT_NUMBER;
	val.numVal.ullVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const bool src) 
{
	Clear();
	vt = JVT_BOOL;
	jt = JT_BOOL;
	val.boolVal = src;
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const JSONOBJECT& src) 
{
	Clear();
	vt = JVT_DISPATCH;
	jt = JT_OBJECT;
	val.objVal = new JSONOBJECT;
	if (src.size() > 0)
		val.objVal->insert(src.begin(), src.end());
	return *this;
}

JSONVALUE& JSONVALUE::operator =(const JSONARRAY& src) 
{
	Clear();
	vt = JVT_ARRAY;
	jt = JT_ARRAY;
	val.arVal = new JSONARRAY;
	if (src.size() > 0)
		val.arVal->insert(val.arVal->end(), src.begin(), src.end());
	return *this;
}
/*
JSONVALUE& JSONVALUE::operator =(const _variant_t& src) 
{
	Clear();
	vt = src.vt;
	switch (src.vt) {
		// special
		/*case JVT_DATE:
			// save as number
			jt = JT_NUMBER;
			val.numVal.dblVal = src.date;
			break;*//*
		case JVT_NULL:
		case JVT_EMPTY:
			val.numVal.llVal = 0;
			vt = JVT_NULL;
			jt = JT_NULL;
			break;

		// normal
		case JVT_BSTR:
			jt = JT_STRING;
			val.strVal = new string;
            ConvertWA(val.strVal, src.bstrVal);
		//	val.strVal = new wstring;
		//	*val.strVal = (LPCTSTR)src.bstrVal;
			break;
		case JVT_DECIMAL:
			jt = JT_NUMBER;
			VarR8FromDec(&src.decVal, &val.numVal.dblVal);
			break;
		case JVT_R4:
			jt = JT_NUMBER;
			val.numVal.dblVal = src.fltVal;
			break;
		case JVT_R8:
			jt = JT_NUMBER;
			val.numVal.dblVal = src.dblVal;
			break;
		case JVT_I1:
			jt = JT_NUMBER;
			val.numVal.llVal = src.cVal;
			break;
		case JVT_I2:
			jt = JT_NUMBER;
			val.numVal.llVal = src.iVal;
			break;
		case JVT_I4:
			jt = JT_NUMBER;
			val.numVal.llVal = src.lVal;
			break;
		case JVT_I8:
			jt = JT_NUMBER;
			val.numVal.llVal = src.llVal;
			break;
		case JVT_UI1:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.bVal;
			break;
		case JVT_UI2:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.uiVal;
			break;
		case JVT_UI4:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.ulVal;
			break;
		case JVT_UI8:
			jt = JT_NUMBER;
			val.numVal.ullVal = src.ullVal;
			break;
		case JVT_BOOL:
			jt = JT_BOOL;
			val.boolVal = (VARIANT_FALSE != src.boolVal);
			break;
		default:
			val.numVal.llVal = 0;
			vt = JVT_NULL;
			jt = JT_NULL;
			break;
	}
	return *this;
}
*/
/*
JSONVALUE& JSONVALUE::operator [](const wstring &src)
{
	// if data type is not object, convert
	if (JT_OBJECT != jt) {
		Clear();
		SetObject(this);
	}

	JSONOBJECT::iterator it = val.objVal->find(src);
	if (val.objVal->end() == it) {
		(*val.objVal)[src] = JSONVALUE(JT_UNDEFINED);
		return (*val.objVal)[src];
	} else {
		return it->second;
	}
}
*/
JSONVALUE& JSONVALUE::operator [](const char *src)
{
	// if data type is not object, convert
	if (JT_OBJECT != jt) {
		Clear();
		SetObject(this);
	}

	string s(src);
	JSONOBJECT::iterator it = val.objVal->find(s);
	if (val.objVal->end() == it) {
		(*val.objVal)[s] = JSONVALUE(JT_UNDEFINED);
		return (*val.objVal)[s];
	} else {
		return it->second;
	}
}

void JSONVALUE::Push(const JSONVALUE &src)
{
	// if data type is not array, convert
	if (JT_ARRAY != jt) {
		Clear();
		SetArray(this);
	}

	val.arVal->push_back(src);
}

size_t JSONVALUE::Size()
{
	switch (jt) {
		case JT_OBJECT:
			return val.objVal ? val.objVal->size() : 0;
		case JT_ARRAY:
			return val.arVal ? val.arVal->size() : 0;
		default:
			return 0;
	}
}

bool JSONVALUE::IsUndefined()
{
	return (JT_UNDEFINED == jt);
}

JSONVALUE& JSONVALUE::At(const size_t pos)
{
	// return undefined if not array or position invalid
	if ((JT_ARRAY != jt) || (NULL == val.arVal) || (val.arVal->size() <= pos))
	{
        JSONVALUE j(JT_UNDEFINED);
        return (JSONVALUE&)j;
	}
    return (JSONVALUE&)val.arVal->at(pos);
}

// conversion
/*
const char* JSONVALUE::ToString(string& sz, const DWORD nFlag, const JSONFORMAT* pFormat)
{
	wstring szw;
	ToString(szw, nFlag, pFormat);
	return ConvertWA(sz, szw.c_str());
}

bool JSONVALUE::ToVariant(_variant_t& v, const DWORD nFlag)
{
	UNREFERENCED_PARAMETER(nFlag);

	bool bOk = true;
	switch (vt) {
		case JVT_NULL:		v = _variant_t();						break;
		case JVT_I1:			v = (char)val.numVal.llVal;				break;
		case JVT_I2:			v = (short)val.numVal.llVal;			break;
		case JVT_I4:			v = (long)val.numVal.llVal;				break;
		case JVT_I8:			v = val.numVal.llVal;					break;
		case JVT_UI1:		v = (unsigned char)val.numVal.ullVal;	break;
		case JVT_UI2:		v = (unsigned short)val.numVal.ullVal;	break;
		case JVT_UI4:		v = (unsigned long)val.numVal.ullVal;	break;
		case JVT_UI8:		v = val.numVal.ullVal;					break;
		case JVT_DECIMAL:
			{
				DECIMAL decVal = {0};
				VarDecFromR8(val.numVal.dblVal, &decVal);
				v = decVal;
			}
			break;
		case JVT_R4:			v = (float)val.numVal.dblVal;			break;
		case JVT_R8:			v = val.numVal.dblVal;					break;
		case JVT_BSTR:		v = val.strVal->c_str();				break;
		case JVT_DATE:		v = (DATE)val.numVal.dblVal, JVT_DATE;	break;
		case JVT_BOOL:		v = val.boolVal;						break;
		default:			bOk = false;							break;
	}
	return bOk;
}

bool JSONVALUE::Parse(const string& sz, const DWORD nFlag, JSONERROR *pError)
{
	wstring szw;
	if (NULL == ConvertAW(szw, sz.c_str()))
		return false;
	return Parse(szw, nFlag, pError);
}
*/
//const char* JSONVALUE::ToString(wstring& sz, const DWORD nFlag, const JSONFORMAT* pFormat) 
const char* JSONVALUE::ToString(string& sz, const DWORD nFlag, const JSONFORMAT* pFormat) 
{
	// NOTE: when formatting value, remove JSON_FLAG_ALLOWUNQUOTENAME flag

	sz = "";
//	sz = "";
	bool bPrettyPrint = HasFlag(JSON_FLAG_PRETTYPRINTTAB, nFlag) ||
		HasFlag(JSON_FLAG_PRETTYPRINTSPACE, nFlag);

	// format
	JSONFORMAT format;
	if (NULL != pFormat) {
		memcpy(&format, pFormat, sizeof(JSONFORMAT));
	}
	string szNumberFormat;
	if (NULL == format.szNumberFormat)
		szNumberFormat = (format.nDecimalPoint >= 0) ? "%.*f" : "%f";
//		szNumberFormat = (format.nDecimalPoint >= 0) ? "%.*f" : "%f";
	else
		szNumberFormat = format.szNumberFormat;
	if (NULL != pFormat) {
		// verify number format
		string::size_type pos = szNumberFormat.find(".*");
//		wstring::size_type pos = szNumberFormat.find(".*");
		if (format.nDecimalPoint < 0) {
			// if auto decimal point, replace '.*' to '.' in format
			if (string::npos != pos) {
				string temp = szNumberFormat.substr(0, pos +1) + szNumberFormat.substr(pos +2);
				szNumberFormat = temp;
			}
		} else {
			// if specific decimal point, ensure there is '.*'
			if (string::npos == pos) {
				pos = szNumberFormat.find(".");
				if (string::npos != pos) {
					string temp = szNumberFormat.substr(0, pos +1) + "*";
					temp += szNumberFormat.substr(pos +1);
					szNumberFormat = temp;
				} else {
					// ms-help://MS.VSCC.v90/MS.MSDNQTR.v90.en/dv_vccrt/html/dc59ea4e-d23a-4f1f-9881-2c919ebefb82.htm
					pos = szNumberFormat.find_first_of("aAdiuoxXeEfgGsS");
					if (string::npos != pos) {
						string temp = szNumberFormat.substr(0, pos) + ".*";
						temp += szNumberFormat.substr(pos);
						szNumberFormat = temp;
					}
				}
			}
		}
	}

	// stringify
	switch (jt) {
		case JT_NULL:
			sz = JSON_LITERAL_NULL;
			break;
		case JT_STRING:
			FormatString(sz, val.strVal ? *val.strVal : "", nFlag, false);
			break;
		case JT_NUMBER:
			switch (vt) {
				case JVT_I1:
				case JVT_I2:
				case JVT_I4:
				case JVT_I8:
					{
						// _I64_MIN = -9,223,372,036,854,775,808 = max 20 char
						char* sVal = new char[21];
						memset(sVal, 0, 21);
						#if defined(_MSC_VER) && (_MSC_VER >= 1400)
						_i64toa_s(val.numVal.llVal, sVal, 21, 10);
						#else
						_i64toa(val.numVal.llVal, sVal, 10);
						#endif
						sz = sVal;
						delete[] sVal;
					}
					break;
				case JVT_UI1:
				case JVT_UI2:
				case JVT_UI4:
				case JVT_UI8:
					{
						// _UI64_MAX = 18,446,744,073,709,551,615 = max 20 char
						char* sVal = new char[21];
						memset(sVal, 0, 21);
						#if defined(_MSC_VER) && (_MSC_VER >= 1400)
						_ui64toa_s(val.numVal.ullVal, sVal, 21, 10);
						#else
						_ui64toa(val.numVal.ullVal, sVal, 10);
						#endif
						sz = sVal;
						delete[] sVal;
					}
					break;
				case JVT_R4:
				case JVT_R8:
				case JVT_DECIMAL:
					{
						// DBL_MAX = 1.7976931348623158e+308 = max 308 char before decimal point
						int MaxChar = 308 +1 +1;
						if (format.nDecimalPoint > 0)
							MaxChar += format.nDecimalPoint;
						char* sVal = new char[MaxChar];
						memset(sVal, 0, MaxChar);
						#if defined(_MSC_VER) && (_MSC_VER >= 1400)
						if (format.nDecimalPoint >= 0)
							sprintf_s(sVal, MaxChar, szNumberFormat.c_str(), format.nDecimalPoint, val.numVal.dblVal);
						else
							sprintf_s(sVal, MaxChar, szNumberFormat.c_str(), val.numVal.dblVal);
						#else
						if (format.nDecimalPoint >= 0)
							sprintf(sVal, szNumberFormat.c_str(), format.nDecimalPoint, val.numVal.dblVal);
						else
							sprintf(sVal, szNumberFormat.c_str(), val.numVal.dblVal);
						#endif
						sz = sVal;
						delete[] sVal;
					}
					break;
				/*case JVT_DATE:
					if (HasFlag(JSON_FLAG_LOCALEDATE, nFlag)) {
						BSTR bstr = NULL;
						VarBstrFromDate(val.numVal.dblVal, LOCALE_SYSTEM_DEFAULT,
							LOCALE_NOUSEROVERRIDE, &bstr);
						FormatString(sz, (const char*)bstr, nFlag, false);
						SysFreeString(bstr);
					} else {
						// default JSON_FLAG_ISODATE
						SYSTEMTIME* sdt = new SYSTEMTIME;
						memset(sdt, 0, sizeof(SYSTEMTIME));
						VariantTimeToSystemTime(val.numVal.dblVal, sdt);
						// yyyy-mm-dd hh:mm:ss = max 19 char
						char* sVal = new char[20];
						memset(sVal, 0, 20);
						#if defined(_MSC_VER) && (_MSC_VER >= 1400)
						swprintf_s(sVal, 20, "%04d-%02d-%02d %02d:%02d:%02d",
							sdt->wYear, sdt->wMonth, sdt->wDay,
							sdt->wHour, sdt->wMinute, sdt->wSecond);
						#else
						swprintf(sVal, 20, "%04d-%02d-%02d %02d:%02d:%02d",
							sdt->wYear, sdt->wMonth, sdt->wDay,
							sdt->wHour, sdt->wMinute, sdt->wSecond);
						#endif
						FormatString(sz, sVal, nFlag, false);
						delete[] sVal;
						delete sdt;
					}
					break;*/
				default:
					break;
			}
			break;
		case JT_BOOL:
			sz = val.boolVal ? JSON_LITERAL_TRUE : JSON_LITERAL_FALSE;
			break;
		case JT_OBJECT:
			sz = JSON_OBJECT_START;
			if (val.objVal && (val.objVal->size() > 0)) {
				string szIndent = "", cIndent = "";
				JSONFORMAT formatNew;
				memcpy(&formatNew, &format, sizeof(JSONFORMAT));
				formatNew.szNumberFormat = szNumberFormat.c_str();
				if (bPrettyPrint)
					bPrettyPrint = (val.objVal->size() > 1);
				if (bPrettyPrint) {
					// newline and increase indent
					if (HasFlag(JSON_FLAG_PRETTYPRINTSPACE, nFlag)) {
						if (format.nIndentSpace > 0) {
							for (DWORD i = 0; i < format.nIndentSpace; i++)
								cIndent += JSON_SPACE;
						}
					} else {
						cIndent = JSON_INDENTTAB;
					}
					sz += JSON_NEWLINE;
					for (DWORD i = 0; i < format.nIndentLevel; i++)
						szIndent += cIndent;
					sz += szIndent + cIndent;
					formatNew.nIndentLevel++;
				}
				string szString;
				bool bFirstItem = true;
				for (JSONOBJECT::const_iterator it = val.objVal->begin(); it != val.objVal->end(); ++it) {
					// if value is not undefined
					if (false == ((JSONVALUE&)it->second).IsUndefined()) {
						if (false == bFirstItem) {
							sz += JSON_ARRAY_SEPARATOR;
							if (true == bPrettyPrint) {
								sz += JSON_NEWLINE;
								sz += szIndent + cIndent;
							}
						}
						FormatString(szString, it->first, nFlag, true);
						sz += szString;
						sz += JSON_VALUE_SEPARATOR;
						if (bPrettyPrint)
							sz += JSON_SPACE;
						((JSONVALUE&)it->second).ToString(szString, nFlag, &formatNew);
						sz += szString;
						bFirstItem = false;
					}
				}
				if (bPrettyPrint) {
					// newline and decrease indent
					sz += JSON_NEWLINE;
					sz += szIndent;
				}
			}
			sz += JSON_OBJECT_END;
			break;
		case JT_ARRAY:
			sz = JSON_ARRAY_START;
			if (val.arVal && (val.arVal->size() > 0)) {
				string szIndent = "", cIndent = "";
				JSONFORMAT formatNew;
				memcpy(&formatNew, &format, sizeof(JSONFORMAT));
				formatNew.szNumberFormat = szNumberFormat.c_str();
				if (bPrettyPrint)
					bPrettyPrint = (val.arVal->size() > 1);
				if (bPrettyPrint) {
					// newline and increase indent
					if (HasFlag(JSON_FLAG_PRETTYPRINTSPACE, nFlag)) {
						if (format.nIndentSpace > 0) {
							for (DWORD i = 0; i < format.nIndentSpace; i++)
								cIndent += JSON_SPACE;
						}
					} else {
						cIndent = JSON_INDENTTAB;
					}
					sz += JSON_NEWLINE;
					for (DWORD i = 0; i < format.nIndentLevel; i++)
						szIndent += cIndent;
					sz += szIndent + cIndent;
					formatNew.nIndentLevel++;
				}
				string szString;
				bool bFIrstItem = true;
				for (JSONARRAY::iterator it = val.arVal->begin(); it != val.arVal->end(); ++it) {
					if (false == bFIrstItem) {
						sz += JSON_ARRAY_SEPARATOR;
						if (true == bPrettyPrint) {
							sz += JSON_NEWLINE;
							sz += szIndent + cIndent;
						}
					}
					it->ToString(szString, nFlag, &formatNew);
					sz += szString;
					bFIrstItem = false;
				}
				if (bPrettyPrint) {
					// newline and decrease indent
					sz += JSON_NEWLINE;
					sz += szIndent;
				}
			}
			sz += JSON_ARRAY_END;
			break;
		default:
			break;
	};

	return sz.c_str();
}

bool JSONVALUE::Parse(const string& sz, const DWORD nFlag, JSONERROR *pError)
{
	Clear();
	m_stage.clear();
	SetError(pError, sz, 0, NULL);

	if (sz.empty()) {
		SetError(pError, sz, 0, "Empty string.");
		return false;
	}

	bool bAllowUnquoteName = HasFlag(JSON_FLAG_ALLOWUNQUOTENAME, nFlag),
		bAllowWhitespaceControlChar = HasFlag(JSON_FLAG_ALLOWWSCNTRLCHAR, nFlag),
		bAllowSingleQuote = HasFlag(JSON_FLAG_ALLOWSINGLEQUOTE, nFlag);

	string sName,				// object name
		sVal;					// array value
	char c = 0,				// current character
		lastc = 0,				// last character
		namequote = 0;
	JSONVALUE *pCur = this;		// current json value
	bool bError = false,		// error?
		bStart = false;			// first json construct found
	string::size_type pos = 0;	// current position
	int nIndex = 0;				// current scope index

	// check every character
	for (string::const_iterator it = sz.begin(); (it != sz.end()) && !bError; ++it) {

		// get current character
		pos = it - sz.begin();
		c = *it;
		if (iswcntrl(c)) {
			if (bAllowWhitespaceControlChar && iswspace(c)) {
				// continue checking if allow white-space control character
				// and this character is white-space character
				bError = false;
			} else {
			bError = true;
				SetError(pError, sz, pos, bAllowWhitespaceControlChar ?
					"Invalid control character." : "Invalid whitespace character.");
			break;
		}
		}

		// get current scope
		JSONSCOPES cur;
		nIndex = GetCurrentScope(&cur);
		if (bStart && (0 == nIndex) && !iswspace(c)) {
			// if parsing has already started, and a scope has already closed, and we found a non-whitespace
			// the json text is likely to start another scope, which is invalid.
			bError = true;
			SetError(pError, sz, pos, "Invalid character after a matching close.");
			break;
		}

		pCur = cur.pValue;
		switch (cur.nScope) {

			// start -----------------------------------------------------------
			case JS_NONE:
				switch (c) {
					case char('/'):
						// / signal starts of comment
						SetNextScope(JS_COMMENT, NULL);
						break;
					case char('{'):
						// { signals start of object
						SetObject(this);
						SetNextScope(JS_OBJECT, this);
						bStart = true;
						break;
					case char('['):
						// [ signals start of array
						SetArray(this);
						SetNextScope(JS_ARRAY, this);
						bStart = true;
						break;
					default:
						// ignore if whitespace, otherwise error
						if (!iswspace(c)) {
							bError = true;
							SetError(pError, sz, pos, "Invalid first character, expected array, object or comment.");
						}
						break;
				}
				break;
			// comment ---------------------------------------------------------
			case JS_COMMENT:
				switch (cur.nSubScope) {
					case JS_NONE:
						// if possible new comment
						switch (c) {
							case char('*'):
								// /* signals start of comment block
								SetCurrentSubScope(JS_COMMENTBLOCK);
								break;
							case char('/'):
								// // signals start of comment line
								SetCurrentSubScope(JS_COMMENTLINE);
								break;
							default:
								// any other than /* and // is error
								bError = true;
								SetError(pError, sz, pos, "Invalid comment block.");
								break;
						}
						break;
					case JS_COMMENTBLOCK:
						// if /* comment block
						switch (c) {
							case char('/'):
								// if */ found, comment block ends
								if (char('*') == lastc)
									RevertScope(1);
								break;
							default:
								// continue scanning
								break;
						}
						break;
					case JS_COMMENTLINE:
						// if // comment line
						switch (c) {
							case char('\r'):
							case char('\n'):
								// if \r or \n found, comment line ends
								RevertScope(1);
								break;
							default:
								// continue scanning
								break;
						}
						break;
					default:
						// unreacheable place
						break;
				}
				break;
			// object ----------------------------------------------------------
			case JS_OBJECT:
				switch (cur.nSubScope) {
					case JS_NONE:
					case JS_NAME:
						// start of object
						switch (c) {
							case char('/'):
								// comment starts
								SetNextScope(JS_COMMENT, NULL);
								break;
							case char('"'):
								// in either strict or loose mode, name can start with "
								SetCurrentSubScope(JS_NAMESTRICT);
								namequote = c;
								break;
							case char(','):
								// if value separator
								if (pCur->val.objVal->size() <= 0) {
									// only valid if already contain at least 1 item
									bError = true;
									SetError(pError, sz, pos, "Invalid separator at start of object.");
								} else if (JS_NAME == cur.nSubScope) {
									// , followed by , empty value
									bError = true;
									SetError(pError, sz, pos, "Invalid extra separator, expected object name.");
								} else {
									// if more than 1 item, comma (,) signals start of another name
									SetCurrentSubScope(JS_NAME);
									sName.clear();
								}
								break;
							case char('}'):
								if (JS_NAME == cur.nSubScope) {
									// , followed by }, extra comma
									bError = true;
									SetError(pError, sz, pos, "Invalid end of object, expected another object name after separator.");
								} else {
									// end of object block, this is an empty object
									RevertScope(1);
									SetCurrentSubScope(JS_NONE);
								}
								break;
							case char('\''):
								if (bAllowSingleQuote) {
									// if allow single quote, start name
									SetCurrentSubScope(JS_NAMESTRICT);
									namequote = c;
								} else {
									// invalid quote
									bError = true;
									SetError(pError, sz, pos, "Invalid single quote.");
								}
								break;
							default:
								if (!iswspace(c)) {
									if (bAllowUnquoteName) {
										// if allow unquote name, name can start with non-whitespace
										SetCurrentSubScope(JS_NAMELOOSE);
										namequote = 0;
										sName.push_back(c);
									} else {
										// if strict json, no other format allowed
										bError = true;
										SetError(pError, sz, pos, "Invalid unquoted object name.");
									}
								}
								break;
						}
						break;
					case JS_NAMESTRICT:
						switch (c) {
							case char('"'):
							case char('\''):
								if (c == namequote) {
									// possible end of strict name
									string temp = sName;
									if (JSON_VALID == UnescapeString(temp)) {
										sName = temp;
										SetCurrentSubScope(JS_NAME_END);
										namequote = 0;
									} else
										sName.push_back(c);
								} else
									sName.push_back(c);
								break;
							default:
								sName.push_back(c);
								break;
						}
						break;
					case JS_NAMELOOSE:
						switch (c) {
							case char('*'):
							case char('/'):
								if (char('/') == lastc) {
									// /* or // signals start of comment, thus ends the name
									(*pCur->val.objVal)[sName.substr(0, sName.length() -1)] = JSONVALUE();
								SetNextScope(JS_COMMENT, NULL);
								} else
									sName.push_back(c);
								break;
							case char(':'):
								// : signals start of value, thus ends the name
								(*pCur->val.objVal)[sName] = JSONVALUE();
								SetCurrentSubScope(JS_VALUE);
								break;
							default:
								if (iswspace(c)) {
									// any whitespace signals end of loose name
									(*pCur->val.objVal)[sName] = JSONVALUE();
									SetCurrentSubScope(JS_NAME_END);
								} else
									sName.push_back(c);
								break;
						}
						break;
					case JS_NAME_END:
						switch (c) {
							case char('/'):
									SetNextScope(JS_COMMENT, NULL);
								break;
							case char(':'):
								(*pCur->val.objVal)[sName] = JSONVALUE();
								SetCurrentSubScope(JS_VALUE);
								break;
							default:
								if (!iswspace(c)) {
									// if not whitespace, error
									bError = true;
									SetError(pError, sz, pos, "Invalid end of object name, expected colon not found.");
								}
								break;
						}
						break;
					case JS_VALUE:
						switch (c) {
							case char('/'):
								SetNextScope(JS_COMMENT, NULL);
								break;
							case char('{'):
								// object value
								pCur = &(*cur.pValue->val.objVal)[sName];
								sName.clear();
								SetObject(pCur);
								SetNextScope(JS_OBJECT, pCur);
								break;
							case char('['):
								// array value
								pCur = &(*cur.pValue->val.objVal)[sName];
								sName.clear();
								SetArray(pCur);
								SetNextScope(JS_ARRAY, pCur);
								break;
							default:
								if (!iswspace(c)) {
									pCur = &(*cur.pValue->val.objVal)[sName];
									SetString(pCur);
									pCur->val.strVal->push_back(c);
									SetCurrentSubScope(JS_VALUEOTHER);
								}
								break;
						}
						break;
					case JS_VALUEOTHER:
						// for string, number, boolean and null, comma (,) signals end
						{
							pCur = &(*cur.pValue->val.objVal)[sName];
							pCur->val.strVal->push_back(c);
							string strVal = *pCur->val.strVal;
							TrimString(strVal, 0);
							if (strVal.size() > 0) {

								// if this is possible a string, we always check value
								// otherwise, we check value only if comma signal
								bool bCheckValue = false,
									bNonStringEnd = false;
								if (char('"') == strVal.at(0)) {
									bCheckValue = true;
								} else if ((char('\'') == strVal.at(0)) && bAllowSingleQuote) {
									bCheckValue = true;
								} else {
									bNonStringEnd = (char(',') == c) || (char('}') == c);
									if (bNonStringEnd) {
										bCheckValue = true;
										// remove the trailing , or }
										string temp = strVal.substr(0, strVal.length() -1);
										strVal = temp;
									}
								}

								// check value
								if (true == bCheckValue) {
									JSONVALUE jval;
									int isstr = JsonFromString(&jval, strVal, nFlag);
									switch (isstr) {
										case JSON_VALID:
											*pCur = jval;
											sName.clear();
											if (char(',') == c) {
												SetCurrentSubScope(JS_NAME);
											} else {
												SetCurrentSubScope(JS_NONE);
												if (char('}') == c) {
													RevertScope(1);
													SetCurrentSubScope(JS_NONE);
												}
											}
											break;
										case JSON_INVALID:
											bError = true;
											SetError(pError, sz, pos, "Invalid string format.");
											break;
										default:
											if (bNonStringEnd) {
												bError = true;
												SetError(pError, sz, pos, "Invalid end of string.");
											}
											break;
									}
								}
							}
						}
						break;
					default:
						break;
				}
				break;

			// array -----------------------------------------------------------
			case JS_ARRAY:
				switch (cur.nSubScope) {
					case JS_NONE:	// start of array
					case JS_VALUE:	// start of value
				switch (c) {
							case char('/'):
								// comment starts
								SetNextScope(JS_COMMENT, NULL);
						break;
							case char('{'):
						// object value
								cur.pValue->val.arVal->push_back(JSONVALUE());
								pCur = &*cur.pValue->val.arVal->rbegin();
								SetObject(pCur);
								SetNextScope(JS_OBJECT, pCur);
						break;
							case char('['):
						// array value
								cur.pValue->val.arVal->push_back(JSONVALUE());
								pCur = &*cur.pValue->val.arVal->rbegin();
								SetArray(pCur);
								SetNextScope(JS_ARRAY, pCur);
								break;
							case char(','):
								// if value separator
								if (pCur->val.arVal->size() <= 0) {
									// only valid if already contain at least 1 item
									bError = true;
									SetError(pError, sz, pos, "Invalid separator at start of array.");
								} else if (JS_VALUE == cur.nSubScope) {
									// , followsed by , empty value
									bError = true;
									SetError(pError, sz, pos, "Invalid extra separator, expected value.");
								} else {
									// if more than 1 item, comma (,) signals start of another value
									SetCurrentSubScope(JS_VALUE);
								}
								break;
							case char(']'):
								if (JS_VALUE == cur.nSubScope) {
									// , followed by ], extra comma
									bError = true;
									SetError(pError, sz, pos, "Invalid end of array, expected another value after separator.");
								} else {
									// [ followed by ], end of array block, this is an empty array
									RevertScope(1);
									SetCurrentSubScope(JS_NONE);
								}
						break;
					default:
								if (!iswspace(c)) {
									SetCurrentSubScope(JS_VALUEOTHER);
									sVal = c;
								}
								break;
						}
						break;
					case JS_VALUEOTHER:
						// for string, number, boolean and null, comma (,) signals end
						{
							sVal.push_back(c);
							string strVal = sVal;
							TrimString(strVal, 0);
							if (strVal.size() > 0) {

								// if this is possible a string, we always check value
								// otherwise, we check value only if comma signal
								bool bCheckValue = false,
									bNonStringEnd = false;
								if (char('"') == strVal.at(0)) {
									bCheckValue = true;
								} else {
									bNonStringEnd = (char(',') == c) || (char(']') == c);
									if (bNonStringEnd) {
										bCheckValue = true;
										// remove the trailing , or ]
										string temp = strVal.substr(0, strVal.length() -1);
										strVal = temp;
									}
								}

								// check value
								if (true == bCheckValue) {
									JSONVALUE jval;
									int isstr = JsonFromString(&jval, strVal, nFlag);
									switch (isstr) {
										case JSON_VALID:
											pCur->val.arVal->push_back(jval);
											sVal.clear();
											SetCurrentSubScope(JS_NONE);
											if (char(']') == c) {
												RevertScope(1);
												SetCurrentSubScope(JS_NONE);
											}
											break;
										case JSON_INVALID:
											bError = true;
											SetError(pError, sz, pos, "Invalid string format.");
											break;
										default:
											if (bNonStringEnd) {
												bError = true;
												SetError(pError, sz, pos, "Invalid end of string.");
						}
						break;
				}
								}
							}
						}
				break;
					default:
						break;
				}
				break;
			default:
				// unreacheable place
				break;
		}

		// remember last char
		lastc = c;
	}

	// check whether the json string is closed correctly
	if (!bError && m_stage.size() > 0) {
		Clear();
		m_stage.clear();
		bError = true;
		SetError(pError, sz, pos +1, "Invalid format, matching closing character not found.");
	}

	return !bError;
}

void JSONVALUE::SetString(JSONVALUE* p)
{
	if (NULL == p)
		return;
	p->jt = JT_STRING;
	p->vt = JVT_BSTR;
	p->val.strVal = new string;
}

void JSONVALUE::SetObject(JSONVALUE* p)
{
	if (NULL == p)
		return;
	p->jt = JT_OBJECT;
	p->vt = JVT_DISPATCH;
	p->val.objVal = new JSONOBJECT;
}

void JSONVALUE::SetArray(JSONVALUE* p)
{
	if (NULL == p)
		return;
	p->jt = JT_ARRAY;
	p->vt = JVT_ARRAY;
	p->val.arVal = new JSONARRAY;
}


int JSONVALUE::JsonFromString(JSONVALUE* dst, const string& sz, const DWORD nFlag)
{
	if (NULL == dst)
		return JSON_CHECKAGAIN;

	int bOk = JSON_CHECKAGAIN;

	string strVal = TrimString((string&)sz, 0);
	if (true == strVal.empty()) {
		// if empty, error
		bOk = JSON_CHECKAGAIN;
	} else if (0 == strVal.compare(JSON_LITERAL_TRUE)) {
		// true
		*dst = true;
		bOk = JSON_VALID;
	} else if (0 == strVal.compare(JSON_LITERAL_FALSE)) {
		// false
		*dst = false;
		bOk = JSON_VALID;
	} else if (0 == strVal.compare(JSON_LITERAL_NULL)) {
		// null
		dst->Clear();
		bOk = JSON_VALID;
	} else {
		// string or number
		bool bAllowSingleQuote = HasFlag(JSON_FLAG_ALLOWSINGLEQUOTE, nFlag);

		string::size_type strl = strVal.length();
		char cStart = strVal.at(0),
			cEnd = 0;
		if (strl > 1)
			cEnd = strVal.at(strl -1);
		bool bString = ((char('"') == cStart) && (char('"') == cEnd)) ||
			((char('\'') == cStart) && (char('\'') == cEnd) && bAllowSingleQuote);
		if (bString) {
			// if starts and ends with ", possible string
			string content = strVal.substr(1, strl -2);		// check content
			bOk = UnescapeString(content);
			if (JSON_VALID == bOk)
				*dst = content;
		} else if ((char('"') != cStart) && (char('"') != cEnd)) {
			// if not starts and ends with ", this is number
			char* wEnd = NULL;
			if (string::npos == strVal.find_first_of(".eE")) {
				// no decimal point, integer
				bool bAllowHexNumber = HasFlag(JSON_FLAG_ALLOWHEXNUMBER, nFlag),
					bAllowOctNumber = HasFlag(JSON_FLAG_ALLOWOCTNUMBER, nFlag),
					bCheck = false;
				if (1 == strl) {
					// string length = 1, process
					bCheck = true;
				} else if (char('0') != strVal.at(0)) {
					// string length at least 2, and not starts with 0, process
					bCheck = true;
				} else if (!bAllowHexNumber && !bAllowOctNumber) {
					// string length at least 2, starts with 0, but oct and hex not allowed, invalid
					bOk = JSON_INVALID;
				} else {
					// string length at least 2, starts with 0, either oct or hex allowed
					char c2 = strVal.at(1);
					switch (c2) {
						case char('x'):
						case char('X'):
							// if starts with 0x or 0X, and hex allowed, process, otherwise invalid
							if (bAllowHexNumber)
								bCheck = true;
							else
								bOk = JSON_INVALID;
							break;
						default:
							// if starts with 0n, and oct allowed, process, otherwise invalid
							if (bAllowOctNumber && iswdigit(c2))
								bCheck = true;
							else
								bOk = JSON_INVALID;
							break;
					}
				} 
				if (bCheck) {
					// check for signed whole number
					int64_t llVal = strtol(strVal.c_str(), &wEnd, 10);
					if ((NULL == wEnd) || (wEnd - strVal.c_str() == strl)) {
						// scale down to lowest data type
						if ((llVal >= _I8_MIN) && (llVal <= _I8_MAX))
							*dst = (char)llVal;		// 8 bit signed
						else if ((llVal >= _I16_MIN) && (llVal <= _I16_MAX))
							*dst = (short)llVal;	// 16 bit signed
						else if ((llVal >= _I32_MIN) && (llVal <= _I32_MAX))
							*dst = (long)llVal;		// 32 bit signed
						else
							*dst = llVal;			// 64 bit signed
						bOk = JSON_VALID;
					} else {
						// check for unsigned whole number
						wEnd = NULL;
						uint64_t ullVal = strtoul(strVal.c_str(), &wEnd, 10);
						if ((NULL == wEnd) || (wEnd - strVal.c_str() == strl)) {
							// scale down to lowest data type
							if (ullVal <= _UI8_MAX)
								*dst = (unsigned char)ullVal;	// 8 bit unsigned
							else if (ullVal <= _UI16_MAX)
								*dst = (unsigned short)ullVal;	// 16 bit unsigned
							else if (ullVal <= _UI32_MAX)
								*dst = (unsigned long)ullVal;	// 32 bit unsigned
							else
								*dst = ullVal;					// 64 bit unsigned
							bOk = JSON_VALID;
						}
					}
				}
			} else {
				// with decimal point, check for double
				double dblVal = strtod(strVal.c_str(), &wEnd);
				if ((NULL == wEnd) || (wEnd - strVal.c_str() == strl)) {
					// scale down to lowest data type
					if ((dblVal >= FLT_MIN) && (dblVal <= FLT_MAX))
						*dst = (float)dblVal;
					else
						*dst = dblVal;
					bOk = JSON_VALID;
				}
			}
		}
	}

	return bOk;
}

// helper
bool JSONVALUE::HasFlag(const DWORD nFlag, const DWORD nSrc) 
{
	return ((0 == nFlag) ? true : (nFlag == (nFlag & nSrc)));
}

bool JSONVALUE::IsHexChar(const char c)
{
	switch (c) {
		case char('0'):
		case char('1'):
		case char('2'):
		case char('3'):
		case char('4'):
		case char('5'):
		case char('6'):
		case char('7'):
		case char('8'):
		case char('9'):
		case char('a'):
		case char('b'):
		case char('c'):
		case char('d'):
		case char('e'):
		case char('f'):
		case char('A'):
		case char('B'):
		case char('C'):
		case char('D'):
		case char('E'):
		case char('F'):
			return true;
		default:
			return false;
	}
}

const char* JSONVALUE::FormatString(string &sz, const string &szSrc, const DWORD nFlag, const bool bName) 
{
	bool bAllowUnquoteName = bName && HasFlag(JSON_FLAG_ALLOWUNQUOTENAME, nFlag),
		bAllowSingleQuote = HasFlag(JSON_FLAG_ALLOWSINGLEQUOTE, nFlag),
		bAllowWhitespaceControlChar = HasFlag(JSON_FLAG_ALLOWWSCNTRLCHAR, nFlag),
		bHasSingleQuote = (string::npos != szSrc.find(char('\''))),
		bHasDoubleQuote = (string::npos != szSrc.find(char('"'))),
		bHasWhiteSpace = (string::npos != szSrc.find_first_of("\b\t\n\v\f\r "));	// 0x08-0x0d, 0x20

	char cQuote = char('"');			// default to strict mode output
	if (szSrc.empty()) {
		// empty string always need to be surrounded with quote
		if (bAllowSingleQuote)
			cQuote = char('\'');
	} else if (bAllowUnquoteName) {
		cQuote = 0;							// allow unquote name, default to no quote
		if (bHasSingleQuote)				// but if there is single quote in string, surround with double quote
			cQuote = char('"');
		else if (bHasDoubleQuote || bHasWhiteSpace ||
			(0 == szSrc.compare(JSON_LITERAL_TRUE)) || (0 == szSrc.compare(JSON_LITERAL_FALSE)) || 
			(0 == szSrc.compare(JSON_LITERAL_NULL)))
			// but if there is double quote or whitespace or is json literal, surround with single or double quote
			cQuote = bAllowSingleQuote ? char('\'') : char('"');
	} else if (bAllowSingleQuote) {
		cQuote = char('\'');				// allow single quote, default to single quote
		if (bHasSingleQuote)				// but if there is single quote in string, surround with double quote
			cQuote = char('"');
	}

	sz = "";
	if (0 != cQuote) sz += cQuote;

	if (szSrc.length() > 0) {
		string::size_type pos = 0,
			escpos = string::npos;
		string escchar = bAllowWhitespaceControlChar ? "\"\\" : "\"\\\b\f\n\r\t";
		do {
			escpos = szSrc.find_first_of(escchar.c_str(), pos);
			if (string::npos == escpos) {
				// copy balance to destination
				sz += szSrc.substr(pos);
				break;
			} else {
				sz += szSrc.substr(pos, escpos - pos);
				sz += EscapeChar(szSrc.at(escpos), cQuote);
				pos = escpos +1;
			}
		} while (string::npos != escpos);
	}

	if (0 != cQuote) sz += cQuote;
	return (char*)sz.c_str();
}

const char* JSONVALUE::EscapeChar(const char c, const char cQuote)
{
	switch (c) {
		case char('"'):	return (char('\'') == cQuote) ? "\"" : "\\\"";
		case char('\\'):	return "\\\\";
		case char('\b'):	return "\\b";
		case char('\f'):	return "\\f";
		case char('\n'):	return "\\n";
		case char('\r'):	return "\\r";
		case char('\t'):	return "\\t";
		// default return the character
	//	case char('/'):	return "/";
		default:			return (char*)&c;
	}
}

char JSONVALUE::UnescapeChar(const string& sz)
{
	if (0 == sz.compare("\\\""))		return char('"');
	else if (0 == sz.compare("\\"))	return char('\\');
	else if (0 == sz.compare("/"))		return char('/');
	else if (0 == sz.compare("\\b"))	return char('\b');
	else if (0 == sz.compare("\\f"))	return char('\f');
	else if (0 == sz.compare("\\n"))	return char('\n');
	else if (0 == sz.compare("\\r"))	return char('\r');
	else if (0 == sz.compare("\\t"))	return char('\t');
	// default return 0
	else return 0;
}

int JSONVALUE::UnescapeString(string& sz)
{
		// empty string
	if (sz.size() <= 0)
		return JSON_VALID;

	// find invalid escape sequence -> '\' not followed by "\/bfnrtu
	string temp = sz;
	string::size_type escpos = 0,
		escfound = string::npos;
	char escchar = 0;
	int bOk = JSON_VALID;
	bool bExit = false;
	do {
		escfound = temp.find(char('\\'), escpos);
		if (string::npos == escfound) {
			// no escape sequence, done
			bExit = true;
			sz = temp;
			bOk = JSON_VALID;
			break;
		} else if (escfound +1 < temp.size()) {
			// check next character
			switch (temp.at(escfound +1)) {
				case char('"'):
				case char('\\'):
				case char('b'):
				case char('f'):
				case char('n'):
				case char('r'):
				case char('t'):
					// valid escape sequence, continue checking
					escchar = UnescapeChar(temp.substr(escfound, 2));
					if (0 != escchar) {
						string temp2 = temp.substr(0, escfound);
						temp2 += escchar;
						temp2 += temp.substr(escfound +2);
						temp = temp2;
						escpos = escfound +1;
					} else {
						escpos = escfound +2;
					}
					break;
				case char('/'):
					// special handle, if \/, just store as it is
					escpos = escfound +2;
					break;
				case char('u'):
					// the following 4 characters must be hex: 0-9, a-f, A-F
					if (escfound +5 < temp.size()) {
						for (string::size_type i = escfound +2; i < escfound +6; i++) {
							if (false == IsHexChar(temp.at(i))) {
								// invalid string, \u not followed by 4 characters
								bExit = true;
								bOk = JSON_INVALID;
								break;
							}
						}
						if (JSON_VALID == bOk)
							escpos = escfound +6;	// continue checking
					} else {
						bExit = true;
						bOk = JSON_CHECKAGAIN;	// check again, not enough character after \u
						break;
					}
					break;
				default:
					// invalid string, \ not followed by valid character
					bExit = true;
					bOk = JSON_INVALID;
					break;
			}
		} else {
			// invalid string, \ (escape sequence) is the last character
			bExit = true;
			bOk = JSON_CHECKAGAIN;	// check again, because now maybe "\" (invalid), next maybe "\\" (valid)
			break;
		}
	} while (!bExit);

	return bOk;
}

const char* JSONVALUE::TrimString(string& sz, const int nLR)
{
	// trim left
	if ((-1 == nLR) || (0 == nLR)) {
		string::iterator it;
		for (it = sz.begin(); it != sz.end(); it++) {
			if (!iswspace(*it))
				break;
			}
		sz.erase(sz.begin(), it);
		}

	// trim right
	if ((1 == nLR) || (0 == nLR)) {
		string::difference_type dt;
		string::reverse_iterator it;

		for (it = sz.rbegin(); it != sz.rend(); it++) {
			if (!iswspace(*it))
				break;
		}
		dt = sz.rend() - it;
		sz.erase(sz.begin() + dt, sz.end());
	}

	return sz.c_str();
}
/*
const char* JSONVALUE::ConvertAW(string& dest, const char* src, const UINT srccp, const bool bRemoveBom)
{
	dest = "";
	bool bOk = false;

	if (NULL != src) {
		int nsrclen = (int)strlen(src);
		if (nsrclen > 0) {
			DWORD dwFlags = (CP_UTF8 == srccp) ? 0 : MB_PRECOMPOSED;
			char* srcNoBom = src;
			if (true == bRemoveBom) {
				// UTF-8
				if ((nsrclen > 2) && (0xef == (BYTE)src[0]) && (0xbb == (BYTE)src[1]) && (0xbf == (BYTE)src[2]))
					srcNoBom = src +3;
				nsrclen = (int)strlen(srcNoBom);
			}

			// get output buffer size
			int ndestlen = MultiByteToWideChar(srccp, dwFlags, srcNoBom, nsrclen, NULL, 0);
			if (ndestlen > 0) {
				ndestlen++;
				char* p = new char[ndestlen];
				memset(p, 0, ndestlen * sizeof(char));
				// convert
				MultiByteToWideChar(srccp, dwFlags, srcNoBom, nsrclen, p, ndestlen);
				dest = p;
				delete[] p;
				bOk = true;
			}
		}
}

	return bOk ? dest.c_str() : NULL;
}

const char* JSONVALUE::ConvertWA(string& dest, const char* src, const UINT destcp, const bool bRemoveBom)
{
	dest = "";
	bool bOk = false;

	if (NULL != src) {
		int nsrclen = (int)strlen(src);
		if (nsrclen > 0) {
			DWORD dwFlags = ((destcp < 50000) && (42 != destcp)) ? (WC_COMPOSITECHECK | WC_SEPCHARS) : 0;
			const char* srcNoBom = src;
			if (true == bRemoveBom) {
				if (0xfeff == (WORD)src[0]) srcNoBom = src +1;		// UNICODE
				else if (0xfffe == (WORD)src[0]) srcNoBom = src +1;	// UNICODE big-endian
				nsrclen = (int)strlen(srcNoBom);
			}

			// get output buffer size
			int ndestlen = WideCharToMultiByte(destcp, dwFlags, srcNoBom, nsrclen, NULL, 0, NULL, NULL);
			if (ndestlen > 0) {
				ndestlen++;
				LPSTR p = new char[ndestlen];
				memset(p, 0, ndestlen);
				// convert
				WideCharToMultiByte(destcp, dwFlags, srcNoBom, nsrclen, p, ndestlen, NULL, NULL);
				dest = p;
				delete[] p;
				bOk = true;
			}
		}
	}

	return bOk ? dest.c_str() : NULL;
}
*/
void JSONVALUE::SetError(JSONERROR *pError, const string& sz, const string::size_type pos, const char* pszDesc)
{
	if (NULL == pError)
		return;

	pError->szDesc = pszDesc ? pszDesc : "";
	pError->nPosition = pos;

	// calculate line and column from position
	DWORD nLine = 1,
		nColumn = 0;
	string sNewline = JSON_NEWLINE;
	string::size_type nlpos = 0,
		nllen = sNewline.length();
	do {
		nlpos = sz.find(sNewline, nlpos);
		if (string::npos == nlpos) {
			nColumn = pos - nColumn +1;
			break;
		} else if (nlpos < pos) {
			nLine++;			// if newline position is earlier than pos, increase line count
			nlpos += nllen;		// continue searching after newline characters
			nColumn = nlpos;	// next line start position
		} else {
			// line number confirmed, calculate column number
			nColumn = pos - nColumn +1;
			break;
		}
	} while (nlpos < pos);

	pError->nLine = nLine;
	pError->nColumn = nColumn;
}

// stage
int JSONVALUE::GetCurrentScope(JSONSCOPES *pScope)
{
	if (NULL == pScope)
		return -1;

	if (m_stage.size() <= 0) {
		pScope->nScope = JS_NONE;
		pScope->nSubScope = JS_NONE;
		pScope->pValue = NULL;
		return 0;
	}

	JSONSTAGE::reverse_iterator rit = m_stage.rbegin();
	pScope->nScope = rit->nScope;
	pScope->nSubScope = rit->nSubScope;
	pScope->pValue = rit->pValue;
	return m_stage.size();
}

void JSONVALUE::SetNextScope(const JSONSCOPE nScope, const JSONVALUE *pValue)
{
	JSONSCOPES s;
	s.nScope = nScope;
	s.nSubScope = JS_NONE;
	s.pValue = (JSONVALUE*)pValue;
	m_stage.push_back(s);
}

bool JSONVALUE::SetCurrentSubScope(const JSONSCOPE nSubScope)
{
	if (m_stage.size() <= 0)
		return false;
	JSONSTAGE::reverse_iterator rit = m_stage.rbegin();
	rit->nSubScope = nSubScope;
	return true;
}

DWORD JSONVALUE::RevertScope(const DWORD nLevel)
{
	if (m_stage.size() <= 0)
		return (DWORD)-1;
	
	DWORD nLevelReverted = 0;
	while (m_stage.size() > 0) {
		m_stage.pop_back();
		nLevelReverted++;
		if (nLevelReverted >= nLevel)
			break;
	}

	return nLevelReverted;
}

//bool JSONVALUE::ToIsoTime(tstring& dest, const DATE src, const DWORD dwOption)
//{
//	bool bHasMs = HASFLAG(IGRC_ISOTIME_MILLISECOND, dwOption);
//
//	dest = _T("");
//	SYSTEMTIME sdt = {0};
//	bool bOk = bHasMs ? VariantTimeToSystemTimeWithMs(src, &sdt) :
//		(TRUE == VariantTimeToSystemTime(src, &sdt));
//	if (false == bOk)
//		return false;
//
//	tstring time;
//	if (HASFLAG(IGRC_ISOTIME_DATE, dwOption)) {
//		FormatT(time, _T("%04d-%02d-%02d "), sdt.wYear, sdt.wMonth, sdt.wDay);
//		dest += time;
//	}
//	if (HASFLAG(IGRC_ISOTIME_ZEROTIME, dwOption)) {
//		dest += _T("00:00:00");
//		if (bHasMs)
//			dest += _T(".000");
//	} else if (HASFLAG(IGRC_ISOTIME_TIME, dwOption)) {
//		if (bHasMs)
//			FormatT(time, _T("%02d:%02d:%02d.%03d"), sdt.wHour, sdt.wMinute, sdt.wSecond, sdt.wMilliseconds);
//		else
//			FormatT(time, _T("%02d:%02d:%02d"), sdt.wHour, sdt.wMinute, sdt.wSecond);
//		dest += time;
//	}
//
//	TrimT(dest);
//	return true;
//}
//
//bool JSONVALUE::FromIsoTime(DATE& dest, LPCTSTR src, const DWORD dwOption)
//{
//	bool bHasMs = HASFLAG(IGRC_ISOTIME_MILLISECOND, dwOption);
//
//	if (NULL == src)
//		return false;
//	int nLen = _tcsclen(src);
//	if (nLen < 19)
//		return false;
//
//	SYSTEMTIME sdt = {0};
//	int nCount = 0;
//	if ((19 == nLen) || ((23 == nLen) && !bHasMs)) {
//		nCount =
//#if defined(_MSC_VER) && (_MSC_VER >= 1400)
//			_stscanf_s
//#else
//			_stscanf
//#endif
//			(src, _T("%u-%u-%u %u:%u:%u"),
//			&sdt.wYear, &sdt.wMonth, &sdt.wDay,
//			&sdt.wHour, &sdt.wMinute, &sdt.wSecond);
//		if (6 != nCount)
//			return false;
//	} else if (23 == nLen) {
//		// NOTE: use a variable bigger than WORD to avoid runtime check failure #2
//		DWORD dwMs = 0;
//		nCount =
//#if defined(_MSC_VER) && (_MSC_VER >= 1400)
//			_stscanf_s
//#else
//			_stscanf
//#endif
//			(src, _T("%u-%u-%u %u:%u:%u.%u"),
//			&sdt.wYear, &sdt.wMonth, &sdt.wDay,
//			&sdt.wHour, &sdt.wMinute, &sdt.wSecond, &dwMs);
//		if (7 != nCount)
//			return false;
//		else
//			sdt.wMilliseconds = (WORD)dwMs;
//	} else
//		return false;
//
//	return SystemTimeToVariantTimeWithMs(&sdt, &dest);
//}
