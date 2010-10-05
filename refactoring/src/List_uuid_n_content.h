/* List_uuid_n_content.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef LISTUUIDNCONTENT_H
#define LISTUUIDNCONTENT_H

#include <gtkmm.h>
#include <map>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>


class List_uuid_n_content 
{
public:
  virtual ~List_uuid_n_content();
  List_uuid_n_content();

  std::map<TransLucid::uuid,Glib::ustring> entities;
  // Method for the spreadsheet
  void add_entity(TransLucid::uuid uuid, std::string content);
  bool del_entity(TransLucid::uuid uuid);
};

#endif // LISTUUIDNCONTENT_H

