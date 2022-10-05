/**
 * @brief   条款12: 了解“抛出一个 exception”与“传递一个参数”或“调用一个虚函数”之间的差异
 */

#include <iostream>
#include <exception>

class Widget {
public:
    Widget() = default;
    Widget(const Widget& widget) {
        std::cout << "Widget : Copy Constructtor" << std::endl;
    }

    void ShowWidgetInfo() {
        std::cout << this << std::endl;
    }
};

class SpecialWidget: public Widget {
public:
    SpecialWidget() = default;
    SpecialWidget(const SpecialWidget& widget) {
        std::cout << "SpecialWidget : Copy Constructtor" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    /**
     * 一个对象被抛出作为 exception时，总是会发生复制(copy)
     * 因此, 抛出异常通常比传递参数要慢
     */
    try {
        Widget localWidget;
        localWidget.ShowWidgetInfo();
        throw localWidget;  // localWidget作为异常被抛出会发生复制
    }  // 将副本抛出
    catch (const Widget& e) {  // 捕获localWidget的副本，可以看到两个指针不同
        const_cast<Widget&>(e).ShowWidgetInfo();
    }

    /**
     * 当对象当作异常被复制，复制行为是由对象的复制构造( copy constructor)执行
     * copy constructor 相应于该对象的“静态类型”而非“动态类型
     */
    try {
        SpecialWidget specialWidget;
        Widget& rw = specialWidget;
        throw rw;   // rw的静态类型为Widget，执行Widget的复制构造函数创建副本并抛出Widget类型的异常
    } catch (const SpecialWidget& e) {
        std::cout << "SpecialWidget Exception" << std::endl;
    } catch (const Widget& e) {  // 可以捕获Widget和SpecialWidget异常
        std::cout << "Widget Exception" << std::endl;
    }

    return 0;
}