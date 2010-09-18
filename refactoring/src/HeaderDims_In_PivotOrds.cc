/* HeaderDims_In_PivotOrds.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "HeaderDims_In_PivotOrds.h"

void
HeaderDims_In_PivotOrds::operator()
  (const std::basic_string<TransLucid::Parser::char_type>& s, 
   const std::u32string& v
  )
{
//  std::cout << "Dimension is " << v << std::endl;
  std::string v_8 = std::string (v.begin(), v.end());
  //TODO; will not need to check for system dimensions when Jarryd changes code
  if (!((v_8 == "time") || (v_8 == "all") || ( v_8 == "id")))
  {
    if (!(pivot.isMember(v)))
    {
      (pivot.ords)[v_8] = 0;
       std::cout << "Adding dimension \"" << v_8 << "\" in utf-8 or \"" 
                 << v << "\" in uft-32" << std::endl;
    }
    else
    {
    }
  }
}
