#include <catch2/catch_test_macros.hpp>

#include <any>
#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>

struct Shape;
struct Circle;
struct Rectangle;

// Visitors -----------------------------------------------------------------------------------------------------------
struct ShapeVisitor {
    virtual ~ShapeVisitor() = default;
    virtual std::any visit(Circle& circle) const = 0;
    virtual std::any visit(Rectangle& rectangle) const = 0;
};

struct Draw : ShapeVisitor {
    using ReturnT = void;
    std::any visit(Circle& circle) const override {
        std::cout << "draw circle"
                  << "\n";
        return {};
    }
    std::any visit(Rectangle& rectangle) const override {
        std::cout << "draw rectangle"
                  << "\n";
        return {};
    }
};

struct GetName : ShapeVisitor {
    using ReturnT = std::string;
    std::any visit(Circle& circle) const override {
        return ReturnT{"i am a circle"};
    }
    std::any visit(Rectangle& rectangle) const override {
        return ReturnT{"i am a rectangle"};
    }
};

// Actual classes -----------------------------------------------------------------------------------------------------
struct Shape {
    virtual ~Shape() = default;

    template <typename VisitorT>
    decltype(auto) accept(VisitorT&& visitor) {
        using T = std::remove_cv_t<std::remove_reference_t<VisitorT>>;
        static_assert(std::is_base_of_v<ShapeVisitor, T>, "VisitorT does not inherit from ShapeVisitor!");
        using ReturnT = typename T::ReturnT;
        if constexpr (std::is_same_v<void, ReturnT>)
            doAccept(visitor);
        else
            return std::any_cast<ReturnT>(doAccept(visitor));
    }

private:
    virtual std::any doAccept(const ShapeVisitor& visitor) = 0;
};

class Circle : public Shape {
    std::any doAccept(const ShapeVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

class Rectangle : public Shape {
    std::any doAccept(const ShapeVisitor& visitor) override {
        return visitor.visit(*this);
    }
};

TEST_CASE("test_visitor)", "[visitor]") {
    auto v = std::vector<std::unique_ptr<Shape>>{};
    v.emplace_back(std::make_unique<Circle>());
    v.emplace_back(std::make_unique<Rectangle>());
    for (auto&& uptr : v) {
        uptr->accept(Draw{});
    }

    for (auto&& uptr : v) {
        auto result = uptr->accept(GetName{});
        std::cout << result << "\n";
    }
}
