#ifndef SRC_AGENT_ENDPOINT_H_
#define SRC_AGENT_ENDPOINT_H_

#include "src/module.h"

#include <cthun-client/connector/connector.h>
#include <cthun-client/message/chunks.h>       // ParsedChunk
#include <cthun-client/message/message.h>      // schema names
#include <cthun-client/validator/schema.h>     // ContentType, Schema

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <map>
#include <memory>
#include <string>

namespace CthunAgent {

class Agent {
  public:
    Agent() = delete;

    Agent(const std::string& bin_path,
          const std::string& server_url,
          const std::string& ca_crt_path,
          const std::string& client_crt_path,
          const std::string& client_key_path);

    void start();

  private:
    // Cthun connector
    CthunClient::Connector connector_;

    // Modules
    std::map<std::string, std::shared_ptr<Module>> modules_;

    // Load the modules from the src/modules directory.
    void loadInternalModules_();

    // Load the external modules contained in the specified directory.
    void loadExternalModulesFrom_(boost::filesystem::path modules_dir_path);

    // Log the loaded modules.
    void logLoadedModules_() const;

    // Returns the json validation schema for a cnc request.
    CthunClient::Schema getCncRequestSchema_() const;

    // Callback for the CthunClient::Connector to handle incoming
    // messages. It will reply to the sender with the requested
    // output.
    // Throw a request_validation error in case: no parsed data; data
    // is not in JSON format; unknown module.
    void cncRequestCallback_(const CthunClient::ParsedChunks& parsed_chunks);
};

}  // namespace CthunAgent

#endif  // SRC_AGENT_ENDPOINT_H_
