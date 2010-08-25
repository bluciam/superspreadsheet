#include "Pivot.h"

Pivot::Pivot()
{
  ords["x"] = 5;
  ords["y"] = 7;
  ords["w"] = 11;
  ords["z"] = 42;
  ords["t"] = 8;
  ords["DIM_TIME"] = 0;
  v_dim = "x";
  h_dim = "y";
  h_radius = 6;
  v_radius = 7;

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


