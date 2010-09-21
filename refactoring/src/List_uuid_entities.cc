/* List_uuid_entities.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "List_uuid_entities.h"

List_uuid_entities::List_uuid_entities()
{
  //Create the Tree model:
  ent_refTreeModel = Gtk::ListStore::create(theColumns);
  ent_TreeView.set_model(ent_refTreeModel);

  //Fill the TreeView's model
  Gtk::TreeModel::Row row = *(ent_refTreeModel->append());
  row[theColumns.uuid] = 1111111111;
  row[theColumns.content] = "x = 5";

  row = *(ent_refTreeModel->append());
  row[theColumns.uuid] = 2222222222;
  row[theColumns.content] = "a = 565";

  //Add the TreeView's view columns:
  ent_TreeView.append_column("uuid", theColumns.uuid);
  ent_TreeView.append_column_editable("content", theColumns.content);

  //Make all the columns reorderable:
  for(int i = 0; i < 2; i++)
  {
    Gtk::TreeView::Column* pColumn = ent_TreeView.get_column(i);
    pColumn->set_reorderable();
  }
}


List_uuid_entities::~List_uuid_entities()
{
}
