#include <iostream>
#include "Ap4.h"

int main() {
    std::string filename = "/Users/ian/Desktop/Bed00086523_V2-0002.mov";
    AP4_ByteStream* input = NULL;
    AP4_Result result = AP4_FileByteStream::Create(filename.c_str(),
                                                   AP4_FileByteStream::STREAM_MODE_READ,
                                                   input);
    if (AP4_FAILED(result)) {
        fprintf(stderr, "ERROR: cannot open input file %s (%d)\n", filename.c_str(), result);
        return 1;
    }

    std::cout << "OK!" << std::endl;
    return 0;
}