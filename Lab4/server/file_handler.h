#pragma once

#include <string>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <cassert>

class FileHandler {
public:
    explicit FileHandler(std::string file_name_) : kFileName(std::move(file_name_)) {}

    std::string read() {
        fd = open(kFileName.c_str(), O_CREAT | O_RDWR, S_IRWXU);

        while (this->isFileLocked());
        assert(this->lockFile() == 0);

        struct stat st{};
        stat(kFileName.c_str(), &st); 
        std::string buffer(st.st_size, '\0');
        ::read(fd, buffer.data(), buffer.size());

        assert(this->unlockFile() == 0);
        return buffer;
    }

    void write(const std::string& buf) {
        fd = open(kFileName.c_str(), O_CREAT | O_RDWR, S_IRWXU);

        while (this->isFileLocked());
        assert(this->lockFile() == 0);

        ::write(fd, buf.c_str(), buf.size());

        assert(this->unlockFile() == 0);
    }

    void rewriteFile(const std::string& buf) {
        fd = open(kFileName.c_str(), O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

        while (this->isFileLocked());
        assert(this->lockFile() == 0);

        ::write(fd, buf.c_str(), buf.size());

        assert(this->unlockFile() == 0);
    }

private: // functions

    int lockFile() const {
       return flock(fd, LOCK_EX); 
    }

    int unlockFile()  {
        return flock(fd, LOCK_UN); 
    }

    bool isFileLocked() {
       return flock(fd, LOCK_EX | LOCK_NB) == kFailure; 
    }

private:
    const std::string kFileName;
    int fd = 0;   // file descriptor
    const int kFailure = -1; 
};
