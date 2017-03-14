#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
#include <boost/python.hpp>

#include <boost/foreach.hpp>
#include <boost/range/combine.hpp>

#include "config.hpp"

//io testing boost
#include <ostream>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>

//including src files
#include "ExampleClass.hpp"

namespace io = boost::iostreams;
namespace py = boost::python;

int main (int argc, char *argv[])
{
  fprintf(stdout, "Testing build & make.\n");
  #ifdef FIND_EXACT
  fprintf(stdout, "Options working\n");
  #endif
 
  io::stream_buffer<io::file_sink> buf("build/out.txt");
  std::ostream out(&buf);
  out << "testing boost io...\n";
  
  Rectangle rect;
  rect.set_values (3,4);
  cout << "area: " << rect.area();

  return 0;
}
