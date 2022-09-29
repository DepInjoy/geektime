/**
 * @brief   条款10：在constructors内阻止资源泄漏（resource leak）
 */
#include <iostream>
#include <memory>
#include <exception>

// #define VERSION1_
// #define VERSION2_
#define VERSION3_

class Image {
public:
    explicit Image(const std::string& imageFileName) {
        std::cout << "Construct Image" << std::endl;
    }
    ~Image() { std::cout << "Destruct ~Image" << std::endl; }
};

class AudioClip {
public:
    explicit AudioClip(const std::string& audioFileName) {
        std::cout << "Construct AudioClip" << std::endl;
    }
    ~AudioClip() { std::cout << "Destruct ~Image" << std::endl; }
};


#ifdef VERSION1_
class BookEntry {
public:
    BookEntry(const std::string& name = "", 
        const std::string& address = "",
        const std::string& imageFileName = "",
        const std::string& audioFileName = "") {
        try {
            imagePtr_ = new Image(imageFileName);
            throw std::runtime_error("just for test");
            audioClipPtr_ = new AudioClip(audioFileName);
        } catch (std::exception& e) {
            delete imagePtr_;
            imagePtr_ = nullptr;
            delete audioClipPtr_;
            audioClipPtr_ = nullptr;
            throw;
        }
    }

    ~BookEntry() {
        delete imagePtr_;
        delete audioClipPtr_;
    }
private:
    std::string                     name_;
    std::string                     address_;
    Image*                          imagePtr_{nullptr};      // 个人相片
    AudioClip*                      audioClipPtr_{nullptr};  // 一段个人声音
};
#endif

#ifdef VERSION2_
class BookEntry {
public:
    BookEntry(const std::string& name = "", 
        const std::string& address = "",
        const std::string& imageFileName = "",
        const std::string& audioFileName = "") :
        imagePtr_(InitImage(imageFileName)),
        audioClipPtr_(InitAudioClicp(audioFileName)) {
    }

    ~BookEntry() {
        delete imagePtr_;
        delete audioClipPtr_;
    }
private:
    Image* InitImage(const std::string& imageFileName) {
        return new Image(imageFileName);
    }

    AudioClip* InitAudioClicp(const std::string& audioFileName) {
        try {
            throw std::runtime_error("just for test");
            return new AudioClip(audioFileName);
        } catch(std::exception& e) {
            delete imagePtr_;
            throw;
        }
    }

    std::string                     name_;
    std::string                     address_;
    Image* const                    imagePtr_{nullptr};      // 个人相片
    AudioClip* const                audioClipPtr_{nullptr};  // 一段个人声音
};
#endif

#define VERSION3_
#ifdef VERSION3_
class BookEntry {
public:
#ifdef UNIQUE_PTR_WRAP_POINTER_
    BookEntry(const std::string& name = "", 
        const std::string& address = "",
        const std::string& imageFileName = "",
        const std::string& audioFileName = "") :
        imagePtr_(new Image(imageFileName)),
        audioClipPtr_(new AudioClip(audioFileName)) {
        throw std::runtime_error("just for test");
    }
#else
    BookEntry(const std::string& name = "", 
        const std::string& address = "",
        const std::string& imageFileName = "",
        const std::string& audioFileName = "") {
        imagePtr_.reset(new Image(imageFileName));
        throw std::runtime_error("just for test");
        audioClipPtr_.reset(new AudioClip(audioFileName));
    }
#endif
private:
    std::string                     name_;
    std::string                     address_;
    std::unique_ptr<Image>          imagePtr_;      // 个人相片
    std::unique_ptr<AudioClip>      audioClipPtr_;  // 一段个人声音
};
#endif


int main(int argc, char* argv[]) {
    try {
        std::unique_ptr<BookEntry> bookEntryPtr(new BookEntry());
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}