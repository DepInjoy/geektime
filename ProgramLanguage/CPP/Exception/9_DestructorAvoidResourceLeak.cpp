/**
 * @brief   条款9：利用destructors避免泄漏资
 */
#include <iostream>
#include <memory>
#include <exception>
#include <fstream>

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

class WINDOW_HANDLE {
public:
    void displayInfo() {
        std::cout << "Display Info" << std::endl;
    }
};

WINDOW_HANDLE createWindow() {
    WINDOW_HANDLE wh;
    std::cout << "createWindow" << std::endl;
    return wh;
}

void destroyWindow(const WINDOW_HANDLE& handle) {
    std::cout << "destroyWindow" << std::endl;
}

class WindowHandle {
public:
    explicit WindowHandle(WINDOW_HANDLE handle) : w(handle) {}
    ~WindowHandle() { destroyWindow(w); }
    // 隐式转换,将WindowHandle转换为WINDOW_HANDLE
    operator WINDOW_HANDLE() const { return w; }

private:
    WINDOW_HANDLE w;
    WindowHandle(const WindowHandle&);
    WindowHandle& operator=(const WindowHandle&);
};

// 展示隐式转换带来的方便之处
// 由于WindowHandle支持隐式转换为WINDOW_HANDLE, 函数入参可以为WindowHandle
void ConvenientWindowHandlerDisplay(WINDOW_HANDLE winHandle) {
    winHandle.displayInfo();
}
void CloseFile(std::FILE* fd) {
    std::cout << "close file" << std::endl;
    std::fclose(fd);
}

int main(int argc, char* argv[]) {
    try {
        {
            std::cout << "1) 采用unique_ptr包装裸指针,避免资源泄漏,同时支持多态" << std::endl;
            // ALA* puppy(new Puppy);
            std::unique_ptr<ALA> puppy(new Puppy);
            puppy->processAdoption();
            // throw std::runtime_error("just for test");
        }

        {
            std::cout << "\n2) 将WINDOW_HANDLE封装在WindowHandle对象内来避免资源泄漏" << std::endl;
            WindowHandle wh2(createWindow());
            static_cast<WINDOW_HANDLE>(wh2).displayInfo();
            ConvenientWindowHandlerDisplay(wh2);
        }

        // follow see: https://en.cppreference.com/w/cpp/memory/unique_ptr
        {
            std::cout << "\n3) Custom deleter demo" << std::endl;
            std::ofstream("demo.txt") << 'x';  // prepare the file to read
            using UniqueFilePtrType = std::unique_ptr<std::FILE, decltype(&CloseFile)>;
            UniqueFilePtrType ufd(std::fopen("demo.txt", "r"), &CloseFile);
            if (ufd) {
                std::cout << char(std::fgetc(ufd.get())) << std::endl;;
            }
        }

        {
            std::cout << "\n4) Custom lambda-expression deleter and exception safety demo" << std::endl;
            std::unique_ptr<Puppy, void(*)(Puppy*)> p(new Puppy, [](Puppy* ptr) {
                std::cout << "destroying from a custom deleter...\n";
                delete ptr;
            });
            // throw std::runtime_error("just for test");
        }
    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
    return 0;
}

