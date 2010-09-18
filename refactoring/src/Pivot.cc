/* Pivot.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "Pivot.h"
#include <cstdio>
#include <iostream>


Pivot::Pivot()
{
  v_dim.clear();
  h_dim.clear();
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

// Method to verfiy if the dimension is already in pivot.ord or empty
bool
Pivot::isMember(std::u32string dimInHeader)
{
  if (ords.empty()) return false;
  for ( std::map<Glib::ustring,int>::iterator mit=(ords).begin();
        mit != (ords).end();
        ++mit )
  {
    Glib::ustring dimInOrds = ((*mit).first);
    std::u32string dimInOrds32 = std::u32string (dimInOrds.begin(),
                                                 dimInOrds.end());
    if (dimInHeader == dimInOrds32) return true;
  }
  return false;
}

