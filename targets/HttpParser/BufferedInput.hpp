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
#include <limits>

namespace parser {

class SourceReader {
public:
    virtual int read(char* buffer, size_t length) = 0;
    virtual ~SourceReader() = default;
};

/**
 * Buffered byte input class.
 */
class BufferedInput {
private:
    SourceReader& source;
    std::vector<char> buffer;

    /** back buffer to allow peeking */
    char backBuffer = 0;
    bool useBackBuffer = false;

    /** index of a last available character + 1, 0 for EOF */
    size_t end = 0;
    /** index of the current character inside buffer */
    size_t cursor = 0;
    /** current line of input assuming unix endl */
    size_t line = 1;
    /** current column of input assuming unix endl */
    size_t column = 1;

    /** limit of characters to read */
    static const size_t LIMIT_INACTIVE = std::numeric_limits<size_t>::max();
    size_t limit = LIMIT_INACTIVE;

    void incCursor() {
        if (limit != LIMIT_INACTIVE) {
            --limit;
        }

        ++cursor;

        if (cursor >= end) {
            cursor = 0;
            end = source.read(buffer.data(), buffer.size());

            if (end == 0) {
                limit = 0;
                buffer[cursor] = END;
            }
        }
    }

    /** can be done only once until next incCursor */
    void decCursor() {
        if (cursor == 0) {
            backBuffer = buffer[cursor];
            useBackBuffer = true;
        } else {
            --cursor;
            if (limit != LIMIT_INACTIVE && limit != 0) {
                ++limit;
            }
        }
    }

    void putBack() {
        decCursor();
    }

public:
    static const char END = 0;

    BufferedInput(SourceReader& source, size_t bufferLength)
        : source(source)
        , buffer(bufferLength)
    {}

    char getChar() {
        if (limit == 0) {
            return END;
        }

        if (useBackBuffer) {
            useBackBuffer = false;
            return backBuffer;
        }

        incCursor();

        char c = buffer[cursor];

        ++column;
        if (c == '\n') {
            ++line;
            column = 1;
        }

        return c;
    }

    char peekChar() {
        char c = getChar();
        putBack();
        return c;
    }

    size_t getLine() const {
        return line;
    }

    size_t getColumn() const {
        return column;
    }

    void setLimit(size_t newLimit) {
        limit = newLimit;
    }

    operator bool () const {
        return end;
    }
};

}
