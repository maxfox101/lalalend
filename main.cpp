#include "svg.h"
#include <iostream>

using namespace std;
using namespace svg;

int main() {
    // Тест вывода цветов
    Color none_color;
    cout << none_color << endl; // none
    
    Color purple{"purple"};
    cout << purple << endl; // purple
    
    Color rgb = Rgb{100, 200, 255};
    cout << rgb << endl; // rgb(100,200,255)

    Color rgba = Rgba{100, 200, 255, 0.5};
    cout << rgba << endl; // rgba(100,200,255,0.5)

    // Тест SVG документа
    Circle c;
    c.SetRadius(3.5).SetCenter({1.0, 2.0});
    c.SetFillColor(rgba);
    c.SetStrokeColor(purple);
    
    Document doc;
    doc.Add(move(c));
    doc.Render(cout);

    return 0;
}
