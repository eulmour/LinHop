#include "File.h"
#include "Framework.h"
#include <malloc.h>
#include <cstdio>
#include <stdexcept>

namespace {

void save(const char* path, void* data, std::size_t size) {
#if defined(ANDROID)
    std::string new_path = "data/data/com.pachuch.linhop/files/" + path;
#else
    std::string new_path = path;
#endif

    FILE* f = fopen(new_path.c_str(), "wb"); if(!f) {
        throw std::runtime_error("IO: Opening file \"" + new_path + "\" for write failed");
    }

// #ifndef NDEBUG
//     LOGI("IO: Writing to file \"%s\"", path);
// #endif

    fwrite(data, size, 1, f);
    fclose(f);
}

}

namespace wuh {

File::File(const char* path) : path_(path) {
    if (path_.empty()) { throw std::runtime_error("Path is empty"); }
    this->load();
}

File::File(const char* path, std::unique_ptr<uint8_t> data, std::size_t size)
    : data_(std::move(data))
    , size_(size)
    , path_(path)
{}

File::File(File&& other)
    : data_(std::move(other.data_))
    , size_(other.size_)
    , path_(other.path_)
{}

void File::remove() const {
    File::remove(path_.c_str());
}

void File::remove(const char* path) {
    if (std::remove(path) != 0) {
        throw std::runtime_error(std::string(strerror(errno)));
    }
}

void File::save(const char* path, void* data, std::size_t size) {
    ::save(path, data, size);
}

void File::save() const {
    save(path_.c_str(), reinterpret_cast<void*>(data_.get()), size_);
}

void File::load() {

    FILE* f = fopen(path_.c_str(), "r"); if(!f) {
        throw std::runtime_error("IO: Opening file \"" + path_ + "\" for read failed");
    }

    fseek(f, 0, SEEK_END);
    size_ = ftell(f);
    fseek(f, 0, SEEK_SET);

#if defined(ANDROID)
    path_ = "data/data/com.pachuch.linhop/files/" + path_;
#endif

    data_ = std::unique_ptr<uint8_t>(new uint8_t[size_]);

    if (data_ == nullptr) {
		fclose(f);
        throw std::runtime_error("Unable to allocate memory");
    }

    fread(data_.get(), size_, 1, f);
    fclose(f);

// #ifndef NDEBUG
    // LOGI("IO: File read \"%s\"", path);
// #endif
}

#if defined(__ANDROID__) || defined(ANDROID)
#   include <unistd.h>

File File::asset(const char* path) {

    if (!engine_instance->asset_mgr) {
        throw std::runtime_error("Bad android asset manager pointer");
    }

    AAsset* asset = AAssetManager_open(engine_instance->asset_mgr, path, AASSET_MODE_BUFFER);

    if (asset == nullptr) {
        throw std::runtime_error("Failed to read file to string, asset not found. (file: " + std::string(path) + ")");
    }

    std::size_t size = AAsset_getLength(asset);

    uint8_t* data = (uint8_t*)malloc(size);
    // std::unique_ptr<uint8_t> data(new uint8_t[size]);
    memcpy(data, AAsset_getBuffer(asset), size);
    AAsset_close(asset);

    return File(path, std::unique_ptr<uint8_t>(data), size);
}

std::string File::cwd() {

    char cwd_buf[256] = {0};
    if (getcwd(cwd_buf, sizeof(cwd_buf))) {
        return std::string(cwd_buf);
    } else {
        throw std::runtime_error(std::string(strerror(errno)));
    }
}

#elif defined (__unix__) || (defined (__APPLE__) && defined(__MACH__))
#   include "unistd.h"

File File::asset(const char* path) {
    return File(path);
}

std::string File::cwd() {

    char cwd_buf[256] = {0};
    if (getcwd(cwd_buf, sizeof(cwd_buf))) {
        return std::string(cwd_buf);
    } else {
        throw std::runtime_error(std::string(strerror(errno)));
    }
}

bool File::exists(const char *path) {
    return access(path, F_OK) == 0;
}

#elif defined (WIN32) || defined (_WIN32)
#   include <Windows.h>

File File::asset(const char* path) {
    return File(path);
}

std::string File::cwd() {

    char cwd_buf[256] = {0};
    if (GetCurrentDirectoryA(static_cast<DWORD>(sizeof(cwd_buf)), cwd_buf) != 0) {
        return std::string(cwd_buf);
    } else {
        throw std::runtime_error(std::string(strerror(errno)));
    }
}

bool File::exists(const char *path) {
	DWORD dwAttrib = GetFileAttributes(path);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

#endif

} // end of namespace wuh