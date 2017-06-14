#ifndef DOUBLE_PARSING_HXX
#define DOUBLE_PARSING_HXX

#include <limits>
#include <locale>

#include <xsd/cxx/ro-string.hxx>
#include <xsd/cxx/zc-istream.hxx>

#include <xsd/cxx/xml/string.hxx> // xml::transcode

#include <xsd/cxx/tree/text.hxx>  // text_content
#include <xsd/cxx/tree/bits/literals.hxx>

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <typename C>
      struct traits<double, C>
      {
        typedef double type;

        static type
        create (const xercesc::DOMElement& e, flags f, tree::type* c);

        static type
        create (const xercesc::DOMAttr& a, flags f, tree::type* c);

        static type
        create (const std::basic_string<C>& s,
                const xercesc::DOMElement*,
                flags,
                tree::type*);
      };

      template <typename C>
      double traits<double, C>::
      create (const xercesc::DOMElement& e, flags f, tree::type* c)
      {
        return create (text_content<C> (e), 0, f, c);
      }

      template <typename C>
      double traits<double, C>::
      create (const xercesc::DOMAttr& a, flags f, tree::type* c)
      {
        return create (xml::transcode<C> (a.getValue ()), 0, f, c);
      }

      template <typename C>
      double traits<double, C>::
      create (const std::basic_string<C>& s,
              const xercesc::DOMElement*,
              flags,
              tree::type*)
      {
        // This type cannot have whitespaces in its values. As result we
        // don't need to waste time collapsing whitespaces. All we need to
        // do is trim the string representation which can be done without
        // copying.
        //
        ro_string<C> tmp (s);
        trim (tmp);

        if (tmp == bits::positive_inf<C> ())
          return std::numeric_limits<double>::infinity ();

        if (tmp == bits::negative_inf<C> ())
          return -std::numeric_limits<double>::infinity ();

        if (tmp == bits::nan<C> ())
          return std::numeric_limits<double>::quiet_NaN ();

        zc_istream<C> is (tmp);
        is.imbue (std::locale::classic ());

        type t;
        is >> t;

        return t;
      }
    }
  }
}

#endif // DOUBLE_PARSING_HXX
