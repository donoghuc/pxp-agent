#ifndef CTHUN_SRC_MESSAGE_H_
#define CTHUN_SRC_MESSAGE_H_

#include "src/data_container.h"
#include "src/message_serialization.h"

#include <string>
#include <vector>
#include <map>
#include <initializer_list>
#include <stdint.h>  // uint8_t

namespace CthunAgent {

//
// ChunkDescriptor
//

namespace ChunkDescriptor {
    static const uint8_t MASK { 0x0F };

    static const uint8_t ENVELOPE { 0x01 };
    static const uint8_t DATA { 0x02 };
    static const uint8_t DEBUG { 0x03 };

    static std::map<uint8_t, const std::string> names {
        { ENVELOPE, "envelope" },
        { DATA, "data" },
        { DEBUG, "debug" }
    };

}  // namespace ChunkDescriptor

//
// MessageChunk
//

struct MessageChunk {
    uint8_t descriptor;
    uint32_t size;  // [byte]
    std::string data_portion;

    MessageChunk();

    MessageChunk(uint8_t _descriptor, uint32_t _size, std::string _data_portion);

    void serializeOn(SerializedMessage& buffer) const;
};

//
// Message
//

class Message {
  public:
    // The default ctor is deleted since, for the Cthun protocol, a
    // valid message must have an envelope chunk (invariant)
    Message() = delete;

    // Construct a Message by parsing the payload delivered
    // by the transport layer as a std::string.
    // Throw a message_processing_error in case of invalid message.
    Message(const std::string& transport_payload);

    // Create a new message with a given envelope.
    // Throw a message_processing_error in case of invalid chunk
    // (unknown descriptor or wrong size).
    Message(MessageChunk envelope);

    // ... and a data chunk; throw a message_processing_error as above
    Message(MessageChunk envelope,
            MessageChunk data_chunk);

    // ... and a debug chunk; throw a message_processing_error as above
    Message(MessageChunk envelope,
            MessageChunk data_chunk,
            MessageChunk debug_chunk);

    // Add optional chunks after validating it.
    // Throw a message_processing_error in case of invalid chunk
    // (unknown descriptor or wrong size).
    void setDataChunk(MessageChunk data_chunk);
    void addDebugChunk(MessageChunk debug_chunk);

    // Getters
    uint8_t getVersion() const;
    MessageChunk getEnvelopeChunk() const;
    MessageChunk getDataChunk() const;
    std::vector<MessageChunk> getDebugChunks() const;

    // Inspectors
    bool hasData() const;
    bool hasDebug() const;

    // Return the buffer containing the bytes of the serialized
    // message.
    // Throw a message_processing_error in case it fails to allocate
    // memory for the buffer.
    SerializedMessage getSerialized() const;

  private:
    uint8_t version_;
    MessageChunk envelope_chunk_;
    MessageChunk data_chunk_;
    std::vector<MessageChunk> debug_chunks_;

    void parseMessage_(const std::string& transport_msg);

    void validateVersion_(const uint8_t& version) const;
    void validateChunk_(const MessageChunk& chunk) const;
};

}  // namespace CthunAgent

#endif  // CTHUN_SRC_DATA_CONTAINER_H_
