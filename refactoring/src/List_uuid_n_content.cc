/* List_uuid_n_content.cc
 * This file is part of the SuperSpreadSheet project.
 */

#include "List_uuid_n_content.h"

List_uuid_n_content::List_uuid_n_content()
{
}


List_uuid_n_content::~List_uuid_n_content()
{
}

void
List_uuid_n_content::add_entity(TransLucid::uuid uuid, std::string content)
{
  std::cout << "Adding equation"  << std::endl;
  entities[uuid] = content;
}

bool
List_uuid_n_content::del_entity(TransLucid::uuid uuid)
{
  std::cout << "Deleting equation"  << std::endl;
}
