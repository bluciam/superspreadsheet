/* List_uuid_entities.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef LISTUUIDENTITIES_H
#define LISTUUIDENTITIES_H

#include <gtkmm.h>
#include <tl/translator.hpp>
#include <tl/utility.hpp>
#include <tl/parser_util.hpp>


class List_uuid_entities 
{
public:
  virtual ~List_uuid_entities();
  List_uuid_entities();

  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    ModelColumns()
    {
      add(uuid);
      add(content);
      add(del_ent);
    }
//  Gtk::TreeModelColumn<unsigned long> uuid;
//  Gtk::TreeModelColumn<TransLucid::uuid> uuid;
  Gtk::TreeModelColumn<std::string> uuid;
  Gtk::TreeModelColumn<Glib::ustring> content;
  Gtk::TreeModelColumn<bool> del_ent;
  };

  ModelColumns theColumns;

  Gtk::TreeView ent_TreeView;
  Glib::RefPtr<Gtk::ListStore> ent_refTreeModel;

  // Method for the spreadsheet
  void add_entity(std::string uuid, std::string content);
};




#endif // LISTUUIDENTITIES_H

