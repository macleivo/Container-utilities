#include <catch2/catch_test_macros.hpp>

#include <any>
#include <iostream>
#include <memory>
#include <vector>

struct Shape;
struct Circle;
struct Rectangle;

struct ShapeVisitor {
    virtual ~ShapeVisitor() = default;
    virtual std::any visit(Circle& shape) = 0;
    virtual std::any visit(Rectangle& shape) = 0;
};

struct Draw : ShapeVisitor {
    std::any visit(Circle& circle) override {
        std::cout << "draw circle"
                  << "\n";
        return {};
    }
    std::any visit(Rectangle& rectangle) override {
        std::cout << "draw rectangle"
                  << "\n";
        return {};
    }
};

struct GetName : ShapeVisitor {
    std::any visit(Circle& circle) override {
        return std::string{"i am a circle"};
    }
    std::any visit(Rectangle& rectangle) override {
        return std::string{"i am a rectangle"};
    }
};

struct Shape {
    virtual ~Shape() = default;

    template <typename ReturnT>
    ReturnT accept(ShapeVisitor& visitor) {
        if constexpr (std::is_same_v<void, ReturnT>)
            doAccept(visitor);
        else
            return std::any_cast<ReturnT>(doAccept(visitor));
    }

private:
    virtual std::any doAccept(ShapeVisitor& visitor) = 0;
};

struct Circle : Shape {
private:
    std::any doAccept(ShapeVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

struct Rectangle : Shape {
private:
    std::any doAccept(ShapeVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

TEST_CASE("test_visitor)", "[visitor]") {
    auto v = std::vector<std::unique_ptr<Shape>>{};
    v.emplace_back(std::make_unique<Circle>());
    v.emplace_back(std::make_unique<Rectangle>());
    for (auto&& uptr : v) {
        auto draw = Draw{};
        uptr->accept<void>(draw);
    }

    for (auto&& uptr : v) {
        auto getName = GetName{};
        auto result = uptr->accept<std::string>(getName);
        std::cout << result << "\n";
    }
}
