#ifndef DOUBLE_SERIALIZATION_HXX
#define DOUBLE_SERIALIZATION_HXX

#include <limits>
#include <locale>
#include <sstream>
#include <iomanip>

#include <xsd/cxx/tree/bits/literals.hxx>

namespace XERCES_CPP_NAMESPACE
{
  inline void
  operator<< (xercesc::DOMElement& e, double d)
  {
    if (d == std::numeric_limits<double>::infinity ())
      e << "INF";
    else if (d == -std::numeric_limits<double>::infinity ())
      e << "-INF";
    else if (!(d == d))
      e << "NaN";
    else
    {
      std::basic_ostringstream<char> os;
      os.imbue (std::locale::classic ());

	  os << std::setprecision(8) << std::scientific << d;

      e << os.str ();
    }
  }

  inline void
  operator<< (xercesc::DOMAttr& a, double d)
  {
    if (d == std::numeric_limits<double>::infinity ())
      a << "INF";
    else if (d == -std::numeric_limits<double>::infinity ())
      a << "-INF";
    else if (!(d == d))
      a << "NaN";
    else
    {
      std::basic_ostringstream<char> os;
      os.imbue (std::locale::classic ());

      os << std::setprecision(8) << std::scientific << d;

      a << os.str ();
    }
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
      operator<< (list_stream<C>& ls, double d)
      {
        if (d == std::numeric_limits<double>::infinity ())
          ls.os_ << bits::positive_inf<C> ();
        else if (d == -std::numeric_limits<double>::infinity ())
          ls.os_ << bits::negative_inf<C> ();
        else if (!(d == d))
          ls.os_ << bits::nan<C> ();
        else
        {
          ls.os_.imbue (std::locale::classic ());
          ls.os_ << std::setprecision(8) << std::scientific << d;
        }
      }
    }
  }
}

#endif // DOUBLE_SERIALIZATION_HXX
