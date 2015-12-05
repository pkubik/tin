/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <array>

/**
 * Buffered byte input class.
 *
 * SourceType - class providing a read method
 * BS - buffer size, number of bytes that can be looked ahead
 */
template <typename SourceType, size_t BS = 64>
class BufferedInput {
private:
    typedef std::array<char, BS> BufferType;

    SourceType& source;
    BufferType buffer;
    size_t cursor = 0;

    void moveCursor() {
        ++cursor;

        if (cursor >= BS) {
            cursor = 0;
            source.read(buffer.data(), BS);
        }
    }

public:
    BufferedInput(SourceType& source)
        : source(source) {
        source.read(buffer.data(), BS);
    }

    char getChar() {
        char c = buffer[cursor];
        moveCursor();
        return c;
    }
};
