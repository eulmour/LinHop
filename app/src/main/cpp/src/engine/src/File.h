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
    static File asset(const char* path);
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

} // end of namespace wuh

#endif