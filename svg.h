#define _USE_MATH_DEFINES
#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
namespace svg {

class Document;
class Object;

class ObjectContainer{
public:
    //
    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::move((std::make_unique<Obj>(std::move(obj)))));
    }

    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
    virtual ~ObjectContainer() = default;

};

class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};


/*
 * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
 * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
 */
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

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

/*
 * Абстрактный базовый класс Object служит для унифицированного хранения
 * конкретных тегов SVG-документа
 * Реализует паттерн "Шаблонный метод" для вывода содержимого тега
 */
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};



/*
 * Класс Circle моделирует элемент <circle> для отображения круга
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
 */
class Circle final : public Object {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

/*
 * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
 */
class Polyline final : public Object{
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);
    void RenderObject(const RenderContext& context) const override;

    /*
     * Прочие методы и данные, необходимые для реализации элемента <polyline>
     */
private:
    std::vector<Point> vector_points_;
};

/*
 * Класс Text моделирует элемент <text> для отображения текста
 * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
 */
class Text: public Object {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos){
        pos_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset){
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size){
        size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family){
        font_family_ = font_family;
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight){
        font_weight_ = font_weight;
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data){
        data_ = data;
        return *this;
    }
    void RenderObject(const RenderContext& context) const override;

    // Прочие данные и методы, необходимые для реализации элемента <text>
private:
    std::string format_data(std::string_view ) const ;
    std::string data_="";
    std::string font_weight_="";
    std::string font_family_="";
    uint32_t size_ = 1;
    Point offset_={0.0, 0.0};
    Point pos_={0.0, 0.0};

};

class Document :public ObjectContainer{
public:
    /*
     Метод Add добавляет в svg-документ любой объект-наследник svg::Object.
     Пример использования:
     Document doc;
     doc.Add(Circle().SetCenter({20, 30}).SetRadius(15));
    */
    //
    /*
    template <typename Obj>
    Document& Add(Obj obj) {
        objects_.emplace_back(std::make_unique<Obj>(std::move(obj)));
        return *this;
    }
*/

    // Добавляет в svg-документ объект-наследник svg::Object

    void AddPtr(std::unique_ptr<Object>&& obj) override{
        objects_.push_back(std::move(obj));
    }

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

    // Прочие методы и данные, необходимые для реализации класса Document
private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg



namespace shapes {

class Triangle : public svg::Drawable {
public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1)
        , p2_(p2)
        , p3_(p3) {
    }

    // Реализует метод Draw интерфейса svg::Drawable
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }

private:
    svg::Point p1_, p2_, p3_;
};
//(Point{50.0, 20.0}, 10.0, 4.0, 5))
class Star : public svg::Drawable  {
public:
    Star(svg::Point  center, double outer_rad, double inner_rad, int num_rays):center_(center),
        outer_rad_(outer_rad),inner_rad_(inner_rad),num_rays_(num_rays) {
    }

    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar());
    }

private:
    svg::Polyline CreateStar() const{
        svg::Polyline polyline;
        for (int i = 0; i <= num_rays_; ++i) {
            double angle = 2 * M_PI * (i % num_rays_) / num_rays_;
            polyline.AddPoint({center_.x + outer_rad_ * sin(angle), center_.y - outer_rad_ * cos(angle)});
            if (i == num_rays_) {
                break;
            }
            angle += M_PI / num_rays_;
            polyline.AddPoint({center_.x + inner_rad_ * sin(angle), center_.y - inner_rad_ * cos(angle)});
        }
        return polyline;
    }

    svg::Point center_;
    double outer_rad_;
    double inner_rad_;
    int num_rays_;

};
class Snowman : public svg::Drawable {
public:
    Snowman(svg::Point  center, double head_radius):center_(center),head_radius_(head_radius){
        svg::Circle circle;
        snowman_.push_back(circle.SetCenter({center_.x,center_.y+head_radius_ * 5} ).SetRadius(head_radius_ * 2));
        snowman_.push_back(circle.SetCenter({center_.x,center_.y+head_radius_ * 2} ).SetRadius(head_radius_ * 1.5));
        snowman_.push_back(circle.SetCenter(center_).SetRadius(head_radius_));
    }

    void Draw(svg::ObjectContainer& container) const override {
        for(const auto& ball:snowman_){
            container.Add(ball);
        }

    }

private:
    std::vector<svg::Circle> snowman_;

    svg::Point  center_;
    double head_radius_;


};

} // namespace shapes
