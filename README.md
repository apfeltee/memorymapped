
## MemoryMapped

... is a thin wrapper around memory-mapping facilities.<br />
Specifically, `mmap` on Linux, and `CreateFileMapping` on Windows.


Example:

    MemoryMapped::File fh("thing");

    // loop over mapped data ...
    for(size_t i=0; i<fh.size(); i++)
    {
        do_something_with(fh[i]);
    }

    // or use the chunk directly
    std::cout.write(fh.data(), fh.size());

    // MemoryMapped knows about RAII, but you're a nice programmer who wouldn't
    // just leave memory chunks hanging around. Right?
    fh.close();

See also `test.cpp`, which accepts a single argument as file and open/read/prints it.
