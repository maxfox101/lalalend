#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

namespace svg {

struct Rgb {
    Rgb() = default;
    Rgb(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) {}
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

struct Rgba {
    Rgba() = default;
    Rgba(uint8_t r, uint8_t g, uint8_t b, double a) : red(r), green(g), blue(b), opacity(a) {}
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

class Color {
public:
    Color() = default;
    Color(std::string color) : color_(std::move(color)) {}
    Color(const char* color) : color_(std::string(color)) {}
    Color(Rgb rgb) : color_(rgb) {}
    Color(Rgba rgba) : color_(rgba) {}

    bool IsNone() const { 
        return std::holds_alternative<std::monostate>(color_); 
    }

private:
    std::variant<std::monostate, std::string, Rgb, Rgba> color_;
    friend std::ostream& operator<<(std::ostream& out, const Color& color);
};

inline const Color NoneColor{};

std::ostream& operator<<(std::ostream& out, const Rgb& rgb);
std::ostream& operator<<(std::ostream& out, const Rgba& rgba);
std::ostream& operator<<(std::ostream& out, const Color& color);

struct Point {
    Point() = default;
    Point(double x, double y) : x(x), y(y) {}
    double x = 0;
    double y = 0;
};

struct RenderContext {
    RenderContext(std::ostream& out) : out(out) {}
    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out), indent_step(indent_step), indent(indent) {}
    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }
    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

class Object {
public:
    void Render(const RenderContext& context) const;
    virtual ~Object() = default;
private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE,
};

std::ostream& operator<<(std::ostream& out, StrokeLineCap value);

enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

std::ostream& operator<<(std::ostream& out, StrokeLineJoin value);

template <typename Owner>
class PathProps {
public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return AsOwner();
    }
    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return AsOwner();
    }
    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_line_cap_ = line_cap;
        return AsOwner();
    }
    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_line_join_ = line_join;
        return AsOwner();
    }

protected:
    ~PathProps() = default;

    void RenderAttrs(std::ostream& out) const {
        using namespace std::literals;
        if (!fill_color_.IsNone()) {
            out << " fill=\"" << fill_color_ << "\""sv;
        }
        if (!stroke_color_.IsNone()) {
            out << " stroke=\"" << stroke_color_ << "\""sv;
        }
        if (stroke_width_) {
            out << " stroke-width=\"" << *stroke_width_ << "\""sv;
        }
        if (stroke_line_cap_) {
            out << " stroke-linecap=\"" << *stroke_line_cap_ << "\""sv;
        }
        if (stroke_line_join_) {
            out << " stroke-linejoin=\"" << *stroke_line_join_ << "\""sv;
        }
    }

private:
    Owner& AsOwner() {
        return static_cast<Owner&>(*this);
    }

    Color fill_color_;
    Color stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_line_cap_;
    std::optional<StrokeLineJoin> stroke_line_join_;
};

class Circle : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;
    Point center_;
    double radius_ = 1.0;
};

class ObjectContainer {
public:
    template <typename ObjectType>
    void Add(ObjectType object) {
        AddPtr(std::make_unique<ObjectType>(std::move(object)));
    }
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
protected:
    ~ObjectContainer() = default;
};

class Document : public ObjectContainer {
public:
    void AddPtr(std::unique_ptr<Object>&& obj) override {
        objects_.push_back(std::move(obj));
    }
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg
