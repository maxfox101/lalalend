#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream& out, StrokeLineCap value) {
    switch (value) {
        case StrokeLineCap::BUTT: out << "butt"; break;
        case StrokeLineCap::ROUND: out << "round"; break;
        case StrokeLineCap::SQUARE: out << "square"; break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, StrokeLineJoin value) {
    switch (value) {
        case StrokeLineJoin::ARCS: out << "arcs"; break;
        case StrokeLineJoin::BEVEL: out << "bevel"; break;
        case StrokeLineJoin::MITER: out << "miter"; break;
        case StrokeLineJoin::MITER_CLIP: out << "miter-clip"; break;
        case StrokeLineJoin::ROUND: out << "round"; break;
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const Rgb& rgb) {
    out << "rgb(" << static_cast<int>(rgb.red) 
        << "," << static_cast<int>(rgb.green) 
        << "," << static_cast<int>(rgb.blue) << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Rgba& rgba) {
    out << "rgba(" << static_cast<int>(rgba.red) 
        << "," << static_cast<int>(rgba.green) 
        << "," << static_cast<int>(rgba.blue) 
        << "," << rgba.opacity << ")";
    return out;
}

std::ostream& operator<<(std::ostream& out, const Color& color) {
    if (color.IsNone()) {
        out << "none";
    } else if (const auto* str = std::get_if<std::string>(&color.color_)) {
        out << *str;
    } else if (const auto* rgb = std::get_if<Rgb>(&color.color_)) {
        out << *rgb;
    } else if (const auto* rgba = std::get_if<Rgba>(&color.color_)) {
        out << *rgba;
    }
    return out;
}

void Object::Render(const RenderContext& context) const {
    context.RenderIndent();
    RenderObject(context);
    context.out << std::endl;
}

Circle& Circle::SetCenter(Point center) {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius) {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\"" << center_.x << "\" cy=\"" << center_.y << "\"";
    out << " r=\"" << radius_ << "\"";
    PathProps<Circle>::RenderAttrs(out);
    out << "/>";
}

void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n";
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    RenderContext ctx{out, 2, 2};
    for (const auto& obj : objects_) {
        obj->Render(ctx);
    }
    out << "</svg>";
}

}  // namespace svg
