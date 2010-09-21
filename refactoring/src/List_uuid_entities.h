/* List_uuid_entities.h
 * This file is part of the SuperSpreadSheet project.
 */

#ifndef LISTUUIDENTITIES_H
#define LISTUUIDENTITIES_H

#include <gtkmm.h>

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
    }
  Gtk::TreeModelColumn<unsigned long> uuid;
  Gtk::TreeModelColumn<Glib::ustring> content;
  };

  ModelColumns theColumns;

  Gtk::TreeView ent_TreeView;
  Glib::RefPtr<Gtk::ListStore> ent_refTreeModel;
};




#endif // LISTUUIDENTITIES_H

