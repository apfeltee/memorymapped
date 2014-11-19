/*
* memorymapped.h
* Original Copyright (c) 2013 Stephan Brumme. All rights reserved.
* see http://create.stephan-brumme.com/disclaimer.html
*/

#pragma once

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

// define fixed size integer types
#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64) // || defined(__CYGWIN__)
    #define MEMMAPPED_USING_WINDOWS
    #if !defined(__CYGWIN__)
        using uint64_t = unsigned __int64;
    #endif
#else
    #include <stdint.h>
#endif



namespace MemoryMapped
{
    #if defined(MEMMAPPED_USING_WINDOWS)
        using FileHandle = void*;
    #else
        using FileHandle = int;
    #endif

    /// get OS page size (for remap)
    size_t GetPageSize();

    class IOError: public std::runtime_error
    {
        private:
            std::string m_filename;

        public:
            IOError(const std::string& fn, const std::string& msg):
                std::runtime_error(msg), m_filename(fn)
            {
            }

            std::string filename() const
            {
                return m_filename;
            }
    };

    /// Portable read-only memory mapping (Windows and Linux)
    /// Filesize limited by size_t, usually 2^32 or 2^64
    class File
    {
        public:
            /// tweak performance
            enum CacheHint
            {
                /// good overall performance
                Normal,
                /// read file only once with few seeks
                SequentialScan,
                /// jump around
                RandomAccess,
            };

            /// how much should be mappend
            enum MapRange
            {
                /// everything at once - be careful when file is larger than memory
                WholeFile = 0,
            };

        protected:
            /// file name
            std::string m_filename;

            /// file size
            uint64_t    m_filesize;

            /// caching strategy
            CacheHint   m_hint;

            /// file handle
            FileHandle  m_handle;

            /// mapped size
            size_t      m_mappedBytes;

            /// pointer to the file contents mapped into memory
            void*       m_mappedView;
            void*       m_mappedFile;

        private:
            /// don't copy object
            File(const File&);

            /// don't copy object
            File& operator=(const File&);

        protected:
            /// impl-defined open
            bool openReal(size_t mappedBytes, CacheHint hint);

            /// error handlers
            bool errOffset();

        public:
            /// do nothing, must use open()
            File();

            /// calls open() with the given params
            File(const std::string& filename, size_t mappedBytes = WholeFile, CacheHint hint = Normal);

            /// close file (see close())
            ~File();

            /// open file, mappedBytes = 0 maps the whole file
            bool open(const std::string& filename, size_t mappedBytes = WholeFile, CacheHint hint = Normal);

            /// close file
            void close();

            /// access position, no range checking (faster)
            unsigned char operator[](size_t offset) const;

            /// access position, including range checking
            unsigned char at(size_t offset) const;

            /// raw access
            const unsigned char* data() const;

            /// true if file successfully opened
            bool isValid() const;

            /// get file size
            uint64_t size() const;

            /// get number of actually mapped bytes
            size_t mappedSize() const;

            /// replace mapping by a new one of the same file, offset MUST be a multiple of the page size
            bool remap(uint64_t offset, size_t mappedBytes);
    };
}
