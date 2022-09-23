#include <iostream>
#include <memory>
#include <exception>

class ALA {
public:
    // 基类的析构函数为虚函数确保派生类析构会调用基类析构
    virtual ~ALA() {
        std::cout << "~ALA" << std::endl;
    }
    virtual void processAdoption() = 0;
};

class Puppy : public ALA {
public:
    ~Puppy() {
        std::cout << "~Puppy" << std::endl;
    }
    void processAdoption() override {
        std::cout << "Puppy::processAdoption" << std::endl;
    }
};

class Kitte : public ALA {
public:
    ~Kitte() {
        std::cout << "~Kitte" << std::endl;
    }

    void processAdoption() override {
        std::cout << "Kitte::processAdoption" << std::endl;
    }
};


class WINDOW_HANDLE {
};

static WINDOW_HANDLE g_windowHandle;
static WINDOW_HANDLE& createWindow() {
    std::cout << "createWindow" << std::endl;
    return g_windowHandle;
}

static void destroyWindow(const WINDOW_HANDLE& handle) {
    std::cout << "destroyWindow" << std::endl;
}

class WindowHandle {
public:
    explicit WindowHandle(WINDOW_HANDLE handle) : w(handle) {}
    ~WindowHandle() { destroyWindow(w); }
    operator WINDOW_HANDLE() { return w; }

private:
    WINDOW_HANDLE w;
    WindowHandle(const WindowHandle&);
    WindowHandle& operator=(const WindowHandle&);
};

int main(int argc, char* argv[]) {
    try {
        // 采用unique_ptr避免资源泄漏
        std::unique_ptr<ALA> puppy(new Puppy);
        puppy->processAdoption();
        throw std::exception();
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
