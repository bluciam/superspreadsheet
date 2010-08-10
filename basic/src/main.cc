int main(int argc, char *argv[])
{

  for (int i = 0 ; i != row_range  ; ++i)
  {
     for (int j = 0 ; j != col_range ; ++j)
     {
       std::string s;
       std::stringstream out;
       out << (i+h_min) << " " << (j+v_min);
       s = out.str();

       Glib::ustring cell = s; 
       (*label).set_label(cell); 
       (*frame).add(*label);
       (*table).attach(*frame, i+1, i+2, j+1, j+2, Gtk::FILL, Gtk::FILL);
       std::cout << "expression = " << expression << std::endl;
       std::stringstream newout;
       newout << expression;
       newout << " @ [";
       newout << *h_dim << ":" << (i+h_min) << ", ";
       newout << *v_dim << ":" << (j+v_min);
       for (std::map<Glib::ustring,int>::iterator mit = tuples.begin();
            mit != tuples.end(); ++mit)
       {
         if (mit->first != *h_dim && mit->first != *v_dim)
         {
	   newout << ", " << mit->first << ":" << mit->second;
         }
       }
       newout << "]";
       std::cout << newout.str() << std::endl;
       std::u32string tuple32;
       for (std::string::iterator it = newout.str().begin();
            it != newout.str().end(); ++it)
       {
         int i = *it;
         char32_t c = i;
         tuple32.push_back(c);
       }
       tuple32.push_back(0);
       TL::HD* cellContext = traductor.translate_expr(tuple32);
       //std::cout << newout.str() << std::endl;
     }
  }
  return 0;
}

