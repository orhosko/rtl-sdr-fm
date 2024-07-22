#ifndef CIRCULARBYTEARRAYH_H
#define CIRCULARBYTEARRAYH_H

#include <boost/circular_buffer.hpp>
#include <QBuffer>
#include <QByteArray>
#include <QIODevice>
#include <cstddef>

class CircularBuffer : public QByteArray {
public:
    CircularBuffer(std::size_t capacity)
        : buffer(capacity) {}

    // Write data to the buffer
    void write(const QByteArray& data) {
        for (char byte : data) {
            // if (buffer.full()) {
            //     buffer.pop_front(); // Overwrite the oldest data if the buffer is full
            // }
            buffer.push_back(byte);
        }
    }

    // Read data from the buffer
    auto read(std::size_t length) -> QByteArray {
        QByteArray result;
        for (std::size_t i = 0; i < length && !buffer.empty(); ++i) {
            result.append(buffer.front());
            buffer.pop_front();
        }
        return result;
    }

    // qint64 size() const {
    //     return buffer.size();
    // }

    // std::size_t capacity() const {
    //     return buffer.capacity();
    // }

    // void clear() {
    //     buffer.clear();
    // }

private:
    boost::circular_buffer<char> buffer;
};

#endif // CIRCULARBYTEARRAYH_H
