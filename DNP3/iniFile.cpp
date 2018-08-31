// IniFile.cpp:  Implementation of the CIniFile class.
// Written by:   Adam Clauss
// Email: cabadam@houston.rr.com
// You may use this class/code as you wish in your programs.  Feel free to distribute it, and
// email suggested changes to me.
//
// Rewritten by: Shane Hill
// Date:         21/08/2001
// Email:        Shane.Hill@dsto.defence.gov.au
// Reason:       Remove dependancy on MFC. Code should compile on any
//               platform.
//////////////////////////////////////////////////////////////////////

// C++ Includes
#include <iostream>
#include <fstream>
#include <strstream>

using namespace std;

// C Includes
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

// Local Includes
#include "iniFile.h"

#if defined(WIN32)
#define iniEOL endl
#else
#define iniEOL '\r' << endl
#endif

CIniFile::CIniFile(string const iniPath)
{
  Path(iniPath);
  caseInsensitive = true;
}

bool CIniFile::ReadFile()
{
  // Normally you would use ifstream, but the SGI CC compiler has
  // a few bugs with ifstream. So ... fstream used.
  fstream f;
  string line;
  string keyname, valuename, value;
  string::size_type pLeft, pRight;

  f.open(path.c_str(), ios::in);
  if (f.fail())
    return false;

  while (getline(f, line))
  {

    if (line.length() == 0)
      continue;

    // To be compatible with Win32, check for existence of '\r'.
    // Win32 files have the '\r' and Unix files don't at the end of a line.
    // Note that the '\r' will be written to INI files from
    // Unix so that the created INI file can be read under Win32
    // without change.
    if (line[line.length() - 1] == '\r')
      line = line.substr(0, line.length() - 1);

    if (line.length())
    {
      // Check that the user hasn't openned a binary file by checking the first
      // character of each line!
      //if ( !isprint( line[0])) {
      //printf( "Failing on char %d\n", line[0]);
      //f.close();
      //return false;
      //}
      if ((pLeft = line.find_first_of(";#[=")) != string::npos)
      {
        switch (line[pLeft])
        {
        case '[':
          if ((pRight = line.find_last_of("]")) != string::npos &&
              pRight > pLeft)
          {
            keyname = line.substr(pLeft + 1, pRight - pLeft - 1);
            AddKeyName(keyname);
          }
          break;

        case '=':
          valuename = line.substr(0, pLeft);
          value = line.substr(pLeft + 1);
          SetValue(keyname, valuename, value);
          break;

        case ';':
        case '#':
          if (!names.size())
            HeaderComment(line.substr(pLeft + 1));
          else
            KeyComment(keyname, line.substr(pLeft + 1));
          break;
        }
      }
    }
  }

  f.close();
  if (names.size())
    return true;
  return false;
}

bool CIniFile::WriteFile()
{
  unsigned commentID, keyID, valueID;
  // Normally you would use ofstream, but the SGI CC compiler has
  // a few bugs with ofstream. So ... fstream used.
  fstream f;

  f.open(path.c_str(), ios::out);
  if (f.fail())
    return false;

  // Write header comments.
  for (commentID = 0; commentID < comments.size(); ++commentID)
    f << ';' << comments[commentID] << iniEOL;
  if (comments.size())
    f << iniEOL;

  // Write keys and values.
  for (keyID = 0; keyID < keys.size(); ++keyID)
  {
    f << '[' << names[keyID] << ']' << iniEOL;
    // Comments.
    for (commentID = 0; commentID < keys[keyID].comments.size(); ++commentID)
      f << ';' << keys[keyID].comments[commentID] << iniEOL;
    // Values.
    for (valueID = 0; valueID < keys[keyID].names.size(); ++valueID)
      f << keys[keyID].names[valueID] << '=' << keys[keyID].values[valueID] << iniEOL;
    f << iniEOL;
  }
  f.close();

  return true;
}

long CIniFile::FindKey(string const keyname) const
{
  for (unsigned keyID = 0; keyID < names.size(); ++keyID)
    if (CheckCase(names[keyID]) == CheckCase(keyname))
      return long(keyID);
  return noID;
}

long CIniFile::FindValue(unsigned const keyID, string const valuename) const
{
  if (!keys.size() || keyID >= keys.size())
    return noID;

  for (unsigned valueID = 0; valueID < keys[keyID].names.size(); ++valueID)
    if (CheckCase(keys[keyID].names[valueID]) == CheckCase(valuename))
      return long(valueID);
  return noID;
}

unsigned CIniFile::AddKeyName(string const keyname)
{
  names.resize(names.size() + 1, keyname);
  keys.resize(keys.size() + 1);
  return names.size() - 1;
}

string CIniFile::KeyName(unsigned const keyID) const
{
  if (keyID < names.size())
    return names[keyID];
  else
    return "";
}

unsigned CIniFile::NumValues(unsigned const keyID)
{
  if (keyID < keys.size())
    return keys[keyID].names.size();
  return 0;
}

unsigned CIniFile::NumValues(string const keyname)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return 0;
  return keys[keyID].names.size();
}

string CIniFile::ValueName(unsigned const keyID, unsigned const valueID) const
{
  if (keyID < keys.size() && valueID < keys[keyID].names.size())
    return keys[keyID].names[valueID];
  return "";
}

string CIniFile::ValueName(string const keyname, unsigned const valueID) const
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return "";
  return ValueName(keyID, valueID);
}

bool CIniFile::SetValue(unsigned const keyID, unsigned const valueID, string const value)
{
  if (keyID < keys.size() && valueID < keys[keyID].names.size())
    keys[keyID].values[valueID] = value;

  return false;
}

bool CIniFile::SetValue(string const keyname, string const valuename, string const value, bool const create)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
  {
    if (create)
      keyID = long(AddKeyName(keyname));
    else
      return false;
  }

  long valueID = FindValue(unsigned(keyID), valuename);
  if (valueID == noID)
  {
    if (!create)
      return false;
    keys[keyID].names.resize(keys[keyID].names.size() + 1, valuename);
    keys[keyID].values.resize(keys[keyID].values.size() + 1, value);
  }
  else
    keys[keyID].values[valueID] = value;

  return true;
}

bool CIniFile::SetValueI(string const keyname, string const valuename, int const value, bool const create)
{
  char svalue[MAX_VALUEDATA];

  sprintf(svalue, "%d", value);
  return SetValue(keyname, valuename, svalue);
}

bool CIniFile::SetValueF(string const keyname, string const valuename, double const value, bool const create)
{
  char svalue[MAX_VALUEDATA];

  sprintf(svalue, "%f", value);
  return SetValue(keyname, valuename, svalue);
}

bool CIniFile::SetValueV(string const keyname, string const valuename, char *format, ...)
{
  va_list args;
  char value[MAX_VALUEDATA];

  va_start(args, format);
  vsprintf(value, format, args);
  va_end(args);
  return SetValue(keyname, valuename, value);
}

string CIniFile::GetValue(unsigned const keyID, unsigned const valueID, string const defValue) const
{
  if (keyID < keys.size() && valueID < keys[keyID].names.size())
    return keys[keyID].values[valueID];
  return defValue;
}

string CIniFile::GetValue(string const keyname, string const valuename, string const defValue) const
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return defValue;

  long valueID = FindValue(unsigned(keyID), valuename);
  if (valueID == noID)
    return defValue;

  return keys[keyID].values[valueID];
}

int CIniFile::GetValueI(string const keyname, string const valuename, int const defValue) const
{
  char svalue[MAX_VALUEDATA];

  sprintf(svalue, "%d", defValue);
  return atoi(GetValue(keyname, valuename, svalue).c_str());
}

double CIniFile::GetValueF(string const keyname, string const valuename, double const defValue) const
{
  char svalue[MAX_VALUEDATA];

  sprintf(svalue, "%f", defValue);
  return atof(GetValue(keyname, valuename, svalue).c_str());
}

// 16 variables may be a bit of over kill, but hey, it's only code.
unsigned CIniFile::GetValueV(string const keyname, string const valuename, char *format,
                             void *v1, void *v2, void *v3, void *v4,
                             void *v5, void *v6, void *v7, void *v8,
                             void *v9, void *v10, void *v11, void *v12,
                             void *v13, void *v14, void *v15, void *v16)
{
  string value;
  // va_list  args;
  unsigned nVals;

  value = GetValue(keyname, valuename);
  if (!value.length())
    return false;
  // Why is there not vsscanf() function. Linux man pages say that there is
  // but no compiler I've seen has it defined. Bummer!
  //
  // va_start( args, format);
  // nVals = vsscanf( value.c_str(), format, args);
  // va_end( args);

  nVals = sscanf(value.c_str(), format,
                 v1, v2, v3, v4, v5, v6, v7, v8,
                 v9, v10, v11, v12, v13, v14, v15, v16);

  return nVals;
}

bool CIniFile::DeleteValue(string const keyname, string const valuename)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return false;

  long valueID = FindValue(unsigned(keyID), valuename);
  if (valueID == noID)
    return false;

  // This looks strange, but is neccessary.
  vector<string>::iterator npos = keys[keyID].names.begin() + valueID;
  vector<string>::iterator vpos = keys[keyID].values.begin() + valueID;
  keys[keyID].names.erase(npos, npos + 1);
  keys[keyID].values.erase(vpos, vpos + 1);

  return true;
}

bool CIniFile::DeleteKey(string const keyname)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return false;

  // Now hopefully this destroys the vector lists within keys.
  // Looking at <vector> source, this should be the case using the destructor.
  // If not, I may have to do it explicitly. Memory leak check should tell.
  // memleak_test.cpp shows that the following not required.
  //keys[keyID].names.clear();
  //keys[keyID].values.clear();

  vector<string>::iterator npos = names.begin() + keyID;
  vector<key>::iterator kpos = keys.begin() + keyID;
  names.erase(npos, npos + 1);
  keys.erase(kpos, kpos + 1);

  return true;
}

void CIniFile::Erase()
{
  // This loop not needed. The vector<> destructor seems to do
  // all the work itself. memleak_test.cpp shows this.
  //for ( unsigned i = 0; i < keys.size(); ++i) {
  //  keys[i].names.clear();
  //  keys[i].values.clear();
  //}
  names.clear();
  keys.clear();
  comments.clear();
}

void CIniFile::HeaderComment(string const comment)
{
  comments.resize(comments.size() + 1, comment);
}

string CIniFile::HeaderComment(unsigned const commentID) const
{
  if (commentID < comments.size())
    return comments[commentID];
  return "";
}

bool CIniFile::DeleteHeaderComment(unsigned commentID)
{
  if (commentID < comments.size())
  {
    vector<string>::iterator cpos = comments.begin() + commentID;
    comments.erase(cpos, cpos + 1);
    return true;
  }
  return false;
}

unsigned CIniFile::NumKeyComments(unsigned const keyID) const
{
  if (keyID < keys.size())
    return keys[keyID].comments.size();
  return 0;
}

unsigned CIniFile::NumKeyComments(string const keyname) const
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return 0;
  return keys[keyID].comments.size();
}

bool CIniFile::KeyComment(unsigned const keyID, string const comment)
{
  if (keyID < keys.size())
  {
    keys[keyID].comments.resize(keys[keyID].comments.size() + 1, comment);
    return true;
  }
  return false;
}

bool CIniFile::KeyComment(string const keyname, string const comment)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return false;
  return KeyComment(unsigned(keyID), comment);
}

string CIniFile::KeyComment(unsigned const keyID, unsigned const commentID) const
{
  if (keyID < keys.size() && commentID < keys[keyID].comments.size())
    return keys[keyID].comments[commentID];
  return "";
}

string CIniFile::KeyComment(string const keyname, unsigned const commentID) const
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return "";
  return KeyComment(unsigned(keyID), commentID);
}

bool CIniFile::DeleteKeyComment(unsigned const keyID, unsigned const commentID)
{
  if (keyID < keys.size() && commentID < keys[keyID].comments.size())
  {
    vector<string>::iterator cpos = keys[keyID].comments.begin() + commentID;
    keys[keyID].comments.erase(cpos, cpos + 1);
    return true;
  }
  return false;
}

bool CIniFile::DeleteKeyComment(string const keyname, unsigned const commentID)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return false;
  return DeleteKeyComment(unsigned(keyID), commentID);
}

bool CIniFile::DeleteKeyComments(unsigned const keyID)
{
  if (keyID < keys.size())
  {
    keys[keyID].comments.clear();
    return true;
  }
  return false;
}

bool CIniFile::DeleteKeyComments(string const keyname)
{
  long keyID = FindKey(keyname);
  if (keyID == noID)
    return false;
  return DeleteKeyComments(unsigned(keyID));
}

string CIniFile::CheckCase(string s) const
{
  if (caseInsensitive)
    for (string::size_type i = 0; i < s.length(); ++i)
      s[i] = tolower(s[i]);
  return s;
}
