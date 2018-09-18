#pragma once

/*
copy from libtorrent
*/

namespace bplan
{
    namespace detail {
        int hex_to_int(char in);
        bool is_hex(char const *in, int len);
    }

    // The overload taking a ``std::string`` converts (binary) the string ``s``
    // to hexadecimal representation and returns it.
    // The overload taking a ``char const*`` and a length converts the binary
    // buffer [``in``, ``in`` + len) to hexadecimal and prints it to the buffer
    // ``out``. The caller is responsible for making sure the buffer pointed to
    // by ``out`` is large enough, i.e. has at least len * 2 bytes of space.
    std::string to_hex(std::string const& s);
    void to_hex(char const *in, int len, char* out);

    // converts the buffer [``in``, ``in`` + len) from hexadecimal to
    // binary. The binary output is written to the buffer pointed to
    // by ``out``. The caller is responsible for making sure the buffer
    // at ``out`` has enough space for the result to be written to, i.e.
    // (len + 1) / 2 bytes.
    std::string from_hex(std::string const& s);
    bool from_hex(char const *in, int len, char* out);
} // namespace bplan

