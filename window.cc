#include "window.h"
#include <stdexcept>
#include <unistd.h>

Xwindow::Xwindow(int width, int height) : wd{width}, ht{height} {
    d = XOpenDisplay(nullptr);
    if (!d) throw std::runtime_error("Cannot open display");

    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapRaised(d, w);

    gc = XCreateGC(d, w, 0, nullptr);

    // Allocate colours
    static const char* colorNames[10] = {
        "white", "black", "red", "green", "blue",
        "cyan", "yellow", "magenta", "orange", "brown"
    };
    Colormap cmap = DefaultColormap(d, s);
    for (int i = 0; i < 10; ++i) {
        XColor xc;
        if (XAllocNamedColor(d, cmap, colorNames[i], &xc, &xc)) {
            colours[i] = xc.pixel;
        } else {
            colours[i] = (i == 0) ? WhitePixel(d, s) : BlackPixel(d, s);
        }
    }

    XSetForeground(d, gc, colours[Black]);

    // Make window non-resizable
    XSizeHints hints;
    hints.flags = PMinSize | PMaxSize;
    hints.min_width  = hints.max_width  = width;
    hints.min_height = hints.max_height = height;
    XSetNormalHints(d, w, &hints);

    XSynchronize(d, True);
    usleep(1000);
}

Xwindow::~Xwindow() {
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XFillRectangle(d, w, gc, x, y, width, height);
    XSetForeground(d, gc, colours[Black]);
    XFlush(d);
}

void Xwindow::drawString(int x, int y, const std::string& msg, int colour) {
    XSetForeground(d, gc, colours[colour]);
    XDrawString(d, w, gc, x, y, msg.c_str(), msg.length());
    XSetForeground(d, gc, colours[Black]);
    XFlush(d);
}
