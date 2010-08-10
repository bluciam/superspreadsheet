#include "labeledoptionmenu.h"

LabeledOptionMenu::LabeledOptionMenu(const Glib::ustring& menu_title,
        Gtk::Menu& menu, bool homogeneous, int spacing) :
    Gtk::HBox(homogeneous, spacing),
    m_label(menu_title),
    m_pMenu(&menu)
{
  pack_start(m_label, Gtk::PACK_SHRINK);

  #ifndef GTKMM_DISABLE_DEPRECATED
  m_OptionMenu.set_menu(*m_pMenu);
  pack_start(m_OptionMenu);
  #endif //GTKMM_DISABLE_DEPRECATED
}

