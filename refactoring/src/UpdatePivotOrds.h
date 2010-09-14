/* UpdatePivotOrds.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef UPDATEPIVOTORDS_H
#define UPDATEPIVOTORDS_H

#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>
#include <tl/parser_fwd.hpp>
#include "Pivot.h"

class UpdatePivotOrds
{
public:
  void
  operator()(const std::basic_string<TransLucid::Parser::char_type>& s,
                  const std::u32string& v);

  UpdatePivotOrds(Pivot& pivot_in)
    : pivot (pivot_in)
  {
  }

private:
  Pivot& pivot;
};

#endif // UPDATEPIVOTORDS_H
