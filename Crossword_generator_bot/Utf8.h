#pragma once

#include <memory>
#include <string>
#include <boost/utility/string_ref.hpp>

const unsigned CODEPAGE_UTF8 = 65001;       // UTF-8 translation

std::wstring FromOtherCP(boost::string_ref s, unsigned CP_FROM);
std::string ToOtherCP(boost::wstring_ref w, unsigned CP_TO);

#ifndef LINUX
//////////////////////////////////////////////////////////////////////////
/// WINDOWS version

inline std::wstring FromUtf8(boost::string_ref s)
{
	return FromOtherCP(s, CODEPAGE_UTF8);
}
inline std::string ToUtf8(boost::wstring_ref w)
{
	return ToOtherCP(w, CODEPAGE_UTF8);
}
inline std::wstring From1251(boost::string_ref s)
{
	return FromOtherCP(s, 1251);
}
inline std::string To1251(boost::wstring_ref w)
{
	return ToOtherCP(w, 1251);
}
inline std::wstring From866(boost::string_ref s)
{
	return FromOtherCP(s, 866);
}
inline std::string To866(boost::wstring_ref w)
{
	return ToOtherCP(w, 866);
}

#else
//////////////////////////////////////////////////////////////////////////
/// LINUX version
std::wstring FromUtf8(boost::string_ref s);
std::string ToUtf8(boost::wstring_ref w);
std::wstring From1251(boost::string_ref s);
std::string To1251(boost::wstring_ref w);
std::wstring From866(boost::string_ref s);
std::string To866(boost::wstring_ref w);

#endif // !LINUX
