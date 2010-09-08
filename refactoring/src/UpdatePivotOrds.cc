/* UpdatePivotOrds.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "UpdatePivotOrds.h"

void
UpdatePivotOrds::operator()
  (const std::basic_string<TransLucid::Parser::char_type>& s, 
   const std::u32string& v
  )
{
  std::cout << "Dimension is " << v << std::endl;
// #if 0
  std::string v_8 = std::string (v.begin(), v.end());
  if (!((v_8 == "time") || (v_8 == "all") || ( v_8 == "id")))
  {
    if (!(pivot.isMember(v)))
    {
      (pivot.ords)[v_8] = 0;
    }
    else
    {
    }
  }
// #endif
}
