
namespace MemoryMapped
{
    size_t GetPageSize()
    {
        return sysconf(_SC_PAGESIZE);
    }

    bool File::open(const std::string& filename, size_t mappedBytes, CacheHint hint)
    {
        struct stat statInfo;
        // already open ?
        if (isValid())
        {
            return false;
        }
        m_file       = 0;
        m_filesize   = 0;
        m_hint       = hint;
        m_mappedView = NULL;
        m_file = ::open(filename.c_str(), O_RDONLY | O_LARGEFILE);
        if(m_file == -1)
        {
            m_file = 0;
            return errOpenFail(filename, "open() failed");
        }
        // file size
        if (fstat(m_file, &statInfo) < 0)
        {
            return errOpenFail(filename, "stat() failed");
        }
        m_filesize = statInfo.st_size;
        // initial mapping
        remap(0, mappedBytes);
        if (!m_mappedView)
        {
            // remap() may throw an exception already
            return false;
        }
        // everything's fine
        return true;
    }

    void File::close()
    {
        m_filesize = 0;
        // kill pointer
        if (m_mappedView)
        {
            ::munmap(m_mappedView, m_filesize);
            m_mappedView = NULL;
        }
        //close underlying file
        if(m_file)
        {
            ::close(m_file);
            m_file = 0;
        }
    }

    bool File::remap(uint64_t offset, size_t mappedBytes)
    {
        int linuxHint;
        if (!m_file)
        {
            return false;
        }
        if (mappedBytes == WholeFile)
        {
            mappedBytes = m_filesize;
        }
        // close old mapping
        if(m_mappedView)
        {
            ::munmap(m_mappedView, m_mappedBytes);
            m_mappedView = NULL;
        }
        // don't go further than end of file
        if(offset > m_filesize)
        {
            return errOffset();
        }
        if((offset + mappedBytes) > m_filesize)
        {
            mappedBytes = size_t(m_filesize - offset);
        } 
        // new mapping
        m_mappedView = ::mmap(NULL, mappedBytes, PROT_READ, MAP_SHARED, m_file, offset);
        if(m_mappedView == MAP_FAILED)
        {
            m_mappedBytes = 0;
            m_mappedView  = NULL;
            throw IOError("mmap == MAP_FAILED");
            return false;
        }
        m_mappedBytes = mappedBytes;
        linuxHint = 0;
        switch (m_hint)
        {
            case Normal:
                linuxHint = MADV_NORMAL;
                break;
            case SequentialScan:
                linuxHint = MADV_SEQUENTIAL;
                break;
            case RandomAccess:
                linuxHint = MADV_RANDOM;
                break;
            default:
                break;
        }
        // assume that file will be accessed soon
        //linuxHint |= MADV_WILLNEED;
        // assume that file will be large
        //linuxHint |= MADV_HUGEPAGE;
        ::madvise(m_mappedView, m_mappedBytes, linuxHint);
        return true;
    }
}
