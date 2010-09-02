/* Pivot.h
 * This file is part of the SuperSpreadSheet project.
 */

#include <gtkmm.h> 
#include <map>

class Pivot
{
public:
  // Constructor and Destructor
  Pivot();
  virtual ~Pivot();

  // Members objects
  std::map<Glib::ustring,int> ords; // List of dimensions and ordinates
  Glib::ustring v_dim ;             // vertical dim to display
  Glib::ustring h_dim ;             // horizontal dim to display
  int           h_radius ;          // horizontal radius of display
  int           v_radius ;          // vertical radius of display

  // Member methods
  bool isOneCell();
  bool isOneRow();
  bool isOneCol();

protected:
};
