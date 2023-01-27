#ifndef ENGINE_FILE
#define ENGINE_FILE

#include <cstddef>
#include <string>
#include <memory>

namespace wuh {

struct File {
    File(const char* path);
    File(const char* path, std::unique_ptr<uint8_t> data, std::size_t size);
    File(File&& other);
    ~File() = default;
    static std::string cwd();
    static bool exists(const char* path);
    static void remove(const char* path);
    static File asset(const std::string& path);
    static void save(const char* path, void* data, std::size_t size);

    void save() const;
    void remove() const;
    [[nodiscard]] uint8_t* data() { return data_.get(); }
    [[nodiscard]] std::size_t size() const { return size_; }
    [[nodiscard]] const std::string& path() const { return path_; }

protected:
    void load();
    std::unique_ptr<uint8_t> data_;
    std::size_t size_;
    std::string path_;
};

// class Loadable {
// protected:
//     virtual std::string file_path() const = 0;
//     virtual std::size_t file_size() const = 0;
//     virtual void file_on_load(void* data) = 0;
//     virtual void* file_data() = 0;
// public:
//     void init_from_file() {

//         std::string path = this->file_path();
// #if defined(ANDROID)
//         std::string path = "data/data/com.pachuch.linhop/files/" + this->file_path();
// #endif

//         FILE* f = fopen(path.c_str(), "r"); if(!f) {
//             throw std::runtime_error("IO: Opening file \"" + path + "\" for read failed");
//         }

//         fseek(f, 0, SEEK_END);
//         std::size_t size = ftell(f);
//         fseek(f, 0, SEEK_SET);

//         auto data = std::unique_ptr<uint8_t>(new uint8_t[size]);

//         if (data == nullptr) {
//             fclose(f);
//             throw std::runtime_error("Unable to allocate memory");
//         } else if (size != this->file_size()) {
//             fclose(f);
//             throw std::runtime_error("File size mismatch");
//         }

//         fread(data.get(), size, 1, f);
//         fclose(f);
//         this->file_on_load(data.get());
//     }

//     void save_to_file() {

//     }
// };

} // end of namespace wuh

#endif