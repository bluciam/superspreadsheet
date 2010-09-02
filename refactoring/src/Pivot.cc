/* Pivot.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "Pivot.h"

Pivot::Pivot()
{
  // TODO: ckeck if there are any dimensions in the header
  // typedef qi::symbols< char_type, u32string > symbols_t
  v_dim.clear();
  h_dim.clear();
  ords["n"] = 6;
  h_radius = 6;
  v_radius = 6;
}


Pivot::~Pivot()
{
}

bool
Pivot::isOneCell()
{
  return ( v_dim.empty() && h_dim.empty() ) ; 
}

bool
Pivot::isOneRow()
{
  return ( v_dim.empty() ) ; 
}

bool
Pivot::isOneCol()
{
  return ( h_dim.empty() ) ; 
}


