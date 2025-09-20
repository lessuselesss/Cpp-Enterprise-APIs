#include <circular/ccertificate.hpp>
#include <circular/circular_enterprise_apis.hpp>
#include <circular/utils.hpp>

namespace circular {

CCertificate::CCertificate()
    : data_("")
    , previous_tx_id_("")
    , previous_block_("")
    , version_(LIB_VERSION)
{
}

void CCertificate::set_data(const std::string& data) {
    data_ = data;
}

std::string CCertificate::get_data() const {
    return data_;
}

std::string CCertificate::get_json_certificate() const {
    try {
        nlohmann::json json_cert = to_json();
        return json_cert.dump();
    } catch (const std::exception&) {
        return ""; // Return empty string on error, matching Rust behavior
    }
}

size_t CCertificate::get_certificate_size() const {
    return get_json_certificate().length();
}

void CCertificate::set_previous_tx_id(const std::string& tx_id) {
    previous_tx_id_ = tx_id;
}

void CCertificate::set_previous_block(const std::string& block) {
    previous_block_ = block;
}

std::string CCertificate::get_previous_tx_id() const {
    return previous_tx_id_;
}

std::string CCertificate::get_previous_block() const {
    return previous_block_;
}

nlohmann::json CCertificate::to_json() const {
    nlohmann::json json_cert;
    json_cert["data"] = data_;
    json_cert["version"] = version_;
    return json_cert;
}

} // namespace circular