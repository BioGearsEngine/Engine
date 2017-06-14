#ifndef DECIMAL_HXX
#define DECIMAL_HXX

namespace xsd
{
  namespace cxx
  {
    namespace tree
    {
      template <>
      struct fundamental_p<long double>
      {
        static const bool r = true;
      };
    }
  }
}

#endif // DECIMAL_HXX
