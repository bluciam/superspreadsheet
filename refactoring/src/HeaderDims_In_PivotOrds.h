/* HeaderDims_In_PivotOrds.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef HEADERDIMSINPIVOTORDS_H
#define HEADERDIMSINPIVOTORDS_H

#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>
#include <tl/parser_fwd.hpp>
#include "Pivot.h"

class HeaderDims_In_PivotOrds
{
public:
  void
  operator()(const std::basic_string<TransLucid::Parser::char_type>& s,
                  const std::u32string& v);

  HeaderDims_In_PivotOrds(Pivot& pivot_in)
    : pivot (pivot_in)
  {
  }

private:
  Pivot& pivot;
};

#endif // HEADERDIMSINPIVOTORDS_H
