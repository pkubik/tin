/*
 * TIN 2015
 *
 * Krystian Czapiga
 * Robert Kluz
 * Pawel Kubik
 * Patryk Szypulski
 */

#pragma once

#include <cstdlib>
#include <vector>

namespace parser {

class SourceReader {
public:
    virtual int read(char* buffer, size_t length) = 0;
};

/**
 * Buffered byte input class.
 */
class BufferedInput {
private:
    SourceReader& source;
    std::vector<char> buffer;

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
            end = source.read(buffer.data(), buffer.size());

            if (end == 0) {
                buffer[0] = END;
            }
        }
    }

public:
    BufferedInput(SourceReader& source, size_t bufferLength)
        : source(source)
        , buffer(bufferLength) {

        end = source.read(buffer.data(), bufferLength);
    }

    static const char END = 0;

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

}
