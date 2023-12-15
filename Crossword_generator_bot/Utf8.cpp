#ifndef LINUX
    #include <windows.h>
    #include "RT_Macros.h"
#else
    #include <boost/locale.hpp>
#endif // !LINUX
#include "Utf8.h"

#ifndef LINUX
using namespace std;

wstring FromOtherCP( boost::string_ref s, unsigned CP_FROM )
{
    if (!s.empty())
    {
        // MultiByte_To_Wide - количество не увеличивается
        // значит s.size() - это максимум символов результата
        wstring w(s.size(), wchar_t(0));
        int wcsz = MultiByteToWideChar(CP_FROM, 0, &s.front(), size32(s), &w.front(), size32(w));
        // уточненный размер
        w.resize(wcsz);
        return w;
    }
    else
        return wstring();
}

string ToOtherCP( boost::wstring_ref w, unsigned CP_TO )
{
    if ( !w.empty() )
    {
        // Wide_To_MultiByte_ - количество Увеличивается но не более чем в 4 раза
        // разместим максимум символов результата
        string s( w.size()*4, char(0) );
        int mbsz = WideCharToMultiByte( CP_TO, 0, &w[0], size32(w), &s.front(), size32(s), NULL, NULL );
        s.resize( mbsz );
        return s;
    }
    else
        return string();
}

#else

std::wstring FromUtf8(boost::string_ref s)
{
    return boost::locale::conv::utf_to_utf<wchar_t,char>(s.to_string());
}

std::string ToUtf8(boost::wstring_ref w)
{
    return boost::locale::conv::utf_to_utf<char,wchar_t>(w.to_string());
}

std::wstring From1251(boost::string_ref s)
{
    return boost::locale::conv::to_utf<wchar_t>(s.to_string(), "windows-1251");
}

std::string To1251(boost::wstring_ref w)
{
    return boost::locale::conv::from_utf(w.to_string(), "windows-1251");
}

std::wstring From866(boost::string_ref s)
{
    return boost::locale::conv::to_utf<wchar_t>(s.to_string(), "cp866");
}

std::string To866(boost::wstring_ref w)
{
    return boost::locale::conv::from_utf(w.to_string(), "cp866");
}

std::wstring FromOtherCP(boost::string_ref s, unsigned CP_FROM)
{
    switch(CP_FROM)
    {
    case CODEPAGE_UTF8:
        return FromUtf8(s);
    case 1251:
        return From1251(s);
    case 866:
        return From866(s);
    default:
        break;
    }
    return std::wstring();
}

std::string ToOtherCP(boost::wstring_ref w, unsigned CP_TO)
{
	switch (CP_TO)
	{
	case CODEPAGE_UTF8:
		return ToUtf8(w);
	case 1251:
		return To1251(w);
	case 866:
		return To866(w);
	default:
		break;
	}
	return std::string();
}


#endif // !LINUX
