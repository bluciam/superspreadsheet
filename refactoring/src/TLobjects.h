/* TLobjects.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef TLOBJECTS_H
#define TLOBJECTS_H

#include <gtkmm.h> 
#include <map>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>

namespace TL = TransLucid;

class TLobjects
{
public:
  // Constructors and Destructor
  TLobjects();
  TLobjects(std::u32string header_str, std::u32string eqns_str);
  virtual ~TLobjects();

  // Members objects
  TL::Translator traductor;
  TL::Parser::Header * header;
  Glib::ustring expression;

  // Member methods
  Glib::ustring calculate_expr (std::u32string expr); 
  int get_time ();

protected:
};

#endif // TLOBJECTS_H
