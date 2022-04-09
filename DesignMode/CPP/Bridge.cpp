/**
 * @brief   桥接模式，属于结构模型式
 *          将抽象和实现分离，使得它们可以独立变化
 *          采用组合关系替代继承关系降低抽象和实现两维度的耦合程度
 * 
 *                                      桥
 *              操作系统(ImageOS) <>------------ 图片格式(ImageFormat)
 *                 ^                                ^
 *               /   \                            /   \
 *              /     \                          /     \
 *          Windows   Mac                       PNG     JPG 
 */

#include <iostream>

class ImageOS {
public:
    virtual void draw(const char* data) = 0;
    virtual ~ImageOS() {};
};

class ImageOSWin : public ImageOS {
public:
    virtual void draw(const char* data) {
        std::cout << "Windows System draw image data" << std::endl;
    }
};

class ImageOSLinux : public ImageOS {
public:
    virtual void draw(const char* data) {
        std::cout << "Linux System draw image data" << std::endl;
    }
};

class ImageOSMac : public ImageOS {
public:
    virtual void draw(const char* data) {
        std::cout << "Mac System draw image data" << std::endl;
    }
};

class ImageFormat {
public:
    ImageFormat(ImageOS* imageOS) : imageos_(imageOS) {};
    virtual void parseFile(const char* fileName) = 0;
    virtual ~ImageFormat() {};

protected:
    ImageOS*        imageos_;
};

class ImageJPG : public ImageFormat {
public:
    ImageJPG(ImageOS* imageOS) : ImageFormat(imageOS) {};
    virtual void parseFile(const char* fileName) {
        std::cout << "Parse JPG file" << fileName << std::endl;
        // 模拟数据
        char* imageData = new char[128];
        imageos_->draw(imageData);
        delete imageData;
    }
};

class ImagePNG : public ImageFormat {
public:
    ImagePNG(ImageOS* imageOS) : ImageFormat(imageOS) {};
    virtual void parseFile(const char* fileName) {
        std::cout << "Parse PNG file" << fileName << std::endl;
        // 模拟数据
        char* imageData = new char[128];
        imageos_->draw(imageData);
        delete imageData;
    }
};

int main(int argc, char* argv[]) {
    // 对Windows操作系统的PNG格式的图片处理
    ImageOS* winos = new ImageOSWin();
    ImageFormat* png = new ImagePNG(winos);
    png->parseFile("1.png");
    delete winos;
    delete png;

    std::cout << std::endl << std::endl;

    // 假设是Linux操作系统的JPG格式的图片处理
    ImageOS * linuxos = new ImageOSLinux();
    ImageFormat* jpg = new ImageJPG(linuxos);
    jpg->parseFile("2.jpg");
    delete linuxos;
    delete jpg;

    return 0;
}

