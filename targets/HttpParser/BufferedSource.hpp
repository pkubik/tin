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

    /** index of a last available character + 1, 0 for EOF */
    size_t end = 0;
    /** index of the current character inside buffer */
    size_t cursor = 0;
    /** current line of input assuming unix endl */
    size_t line = 1;
    /** current column of input assuming unix endl */
    size_t column = 1;

    void moveCursor() {
        ++cursor;

        if (cursor >= end) {
            cursor = 0;
            end = source.read(buffer.data(), BS);
        }
    }

public:
    BufferedInput(SourceType& source)
        : source(source) {
        end = source.read(buffer.data(), BS);
    }

    char getChar() {
        char c = buffer[cursor];

        ++column;
        if (c == '\n') {
            ++line;
            column = 1;
        }

        moveCursor();
        return c;
    }

    char peekChar() const {
        return buffer[cursor];
    }

    size_t getLine() const {
        return line;
    }

    size_t getColumn() const {
        return column;
    }

    operator bool () const {
        return end;
    }
};
