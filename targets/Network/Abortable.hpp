/*
 * TIN 2015
 *
 * Pawel Kubik
 */

#pragma once

#include "Socket.hpp"

namespace network {

/**
 * @brief abortableRead Reads specified amount from the socket and allow aborting
 *
 * Blocks if there is nothing to read. Might return less then required number of bytes.
 *
 * @param socket    socket object
 * @param buffer    buffer to which the data is read
 * @param size      number of bytes to read
 * @param pipeEnd   fd which must be written to in order to abort
 * @param timeout   timeout
 * @return          number of bytes read, and information if it was aborted
 */
std::pair<size_t, bool> abortableRead(Socket& socket,
                                      char* buffer,
                                      const size_t size,
                                      const short pipeEnd,
                                      const int timeout = -1);
/**
 * @brief abortableWrite Writes specified amount to the socket and allow aborting
 *
 * Returns only after all data has been written or after it was aborted
 *
 * @param socket    socket object
 * @param buffer    buffer to which the data is read
 * @param size      number of bytes to read
 * @param pipeEnd   fd which must be written to in order to abort
 * @param timeout   timeout
 * @return          information whether the function was aborted
 */
bool abortableWriteAll(Socket& socket,
                       const char* buffer,
                       const size_t size,
                       const short pipeEnd,
                       const int timeout = -1);

/**
 * @see abortableWriteAll
 */
bool abortableReadAll(Socket& socket,
                      char* buffer,
                      const size_t size,
                      const short pipeEnd,
                      const int timeout = -1);

}
