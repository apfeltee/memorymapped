
#include <iostream>
#include "memorymapped.h"

int main(int argc, char* argv[])
{
    size_t i;
    size_t sz;
    // not really necessary; merely count blocks until the next flush
    // you can safely remove any code involving flushcnt in production code
    size_t flushcnt;
    std::string filename;
    if(argc > 1)
    {
        filename = argv[1];
        try
        {
            MemoryMapped::File fh(filename);
            sz = fh.size();
            for(i=0, flushcnt=0; i<sz; i++, flushcnt++)
            {
                std::cout << fh[i];
                if(flushcnt == 128)
                {
                    std::cout << std::flush;
                    flushcnt = 0;
                }
            }
            std::cout << std::endl;
            fh.close();
        }
        catch(MemoryMapped::IOError& err)
        {
            std::cerr << "failed to open '" << filename << "': " << err.what() << std::endl;
            return 1;
        }
    }
    else
    {
        std::cerr << "usage: " << argv[0] << " <filename>" << std::endl;
    }
    return 0;
}
