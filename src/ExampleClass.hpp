#ifndef EXAMPLECLASS_HPP
#define EXAMPLECLASS_HPP

class Rectangle {
      int width, height;

    public:
      void set_values (int,int);
      int area() { return width*height; }
};

#endif

