#pragma once
#include <string>
// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr);

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str);

// Convert an wide Unicode string to ANSI string
std::string unicode2ansi(const std::wstring &wstr);

// Convert an ANSI string to a wide Unicode String
std::wstring ansi2unicode(const std::string &str);