#ifndef WINDOW_H
#define WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>

class Xwindow {
    Display* d;
    Window w;
    int s;
    GC gc;
    unsigned long colours[10];
    int wd, ht;

public:
    enum { White=0, Black, Red, Green, Blue, Cyan, Yellow, Magenta, Orange, Brown };

    Xwindow(int width=880, int height=1000);
    ~Xwindow();
    Xwindow(const Xwindow&) = delete;
    Xwindow& operator=(const Xwindow&) = delete;

    int getWidth()  const { return wd; }
    int getHeight() const { return ht; }

    void fillRectangle(int x, int y, int width, int height, int colour=Black);
    void drawString(int x, int y, const std::string& msg, int colour=Black);
};

#endif
