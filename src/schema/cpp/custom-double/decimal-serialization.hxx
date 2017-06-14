#ifndef DECIMAL_SERIALIZATION_HXX
#define DECIMAL_SERIALIZATION_HXX

#include <limits>
#include <locale>
#include <sstream>
#include <cfloat>

namespace XERCES_CPP_NAMESPACE
{
  inline void
  operator<< (xercesc::DOMElement& e, long double d)
  {
    std::basic_ostringstream<char> os;
    os.imbue (std::locale::classic ());
    os.precision(DECIMAL_DIG);// (std::numeric_limits<long double>::digits10);

    os << std::fixed << d;
    std::string s (os.str ());

    // Remove the trailing zeros and the decimal point if necessary.
    //
    std::string::size_type size (s.size ()), n (size);

    for (; n > 0 && s[n - 1] == '0'; --n);

    if (n > 0 && s[n - 1] == '.')
      --n;

    if (n != size)
      s.resize (n);

    e << s;
  }

  inline void
  operator<< (xercesc::DOMAttr& a, long double d)
  {
    std::basic_ostringstream<char> os;
    os.imbue (std::locale::classic ());
    os.precision(DECIMAL_DIG);// (std::numeric_limits<long double>::digits10);

    os << std::fixed << d;
    std::string s (os.str ());

    // Remove the trailing zeros and the decimal point if necessary.
    //
    std::string::size_type size (s.size ()), n (size);

    for (; n > 0 && s[n - 1] == '0'; --n);

    if (n > 0 && s[n - 1] == '.')
      --n;

    if (n != size)
      s.resize (n);

    a << s;
  }
}

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <typename C>
      inline void
      operator<< (list_stream<C>& ls, long double d)
      {
        std::basic_ostringstream<C> os;
        os.imbue (std::locale::classic ());
        os.precision(DECIMAL_DIG);// (std::numeric_limits<long double>::digits10);

        os << std::fixed << d;
        std::basic_string<C> s (os.str ());

        // Remove the trailing zeros and the decimal point if necessary.
        //
        typename std::basic_string<C>::size_type size (s.size ()), n (size);

        for (; n > 0 && s[n - 1] == '0'; --n);

        if (n > 0 && s[n - 1] == '.')
          --n;

        if (n != size)
          s.resize (n);

        ls.os_ << s;
      }
    }
  }
}

#endif // DECIMAL_SERIALIZATION_HXX
