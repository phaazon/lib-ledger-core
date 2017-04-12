// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from bitcoin_public_key_provider.djinni

#pragma once

#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace ledger { namespace core { namespace api {

struct BitcoinLikePublicKey final {
    std::vector<uint8_t> publicKey;
    std::vector<uint8_t> chainCode;

    BitcoinLikePublicKey(std::vector<uint8_t> publicKey_,
                         std::vector<uint8_t> chainCode_)
    : publicKey(std::move(publicKey_))
    , chainCode(std::move(chainCode_))
    {}

    BitcoinLikePublicKey(const BitcoinLikePublicKey& cpy) {
       this->publicKey = cpy.publicKey;
       this->chainCode = cpy.chainCode;
    }

    BitcoinLikePublicKey() = default;


    BitcoinLikePublicKey& operator=(const BitcoinLikePublicKey& cpy) {
       this->publicKey = cpy.publicKey;
       this->chainCode = cpy.chainCode;
       return *this;
    }

    template <class Archive>
    void load(Archive& archive) {
        archive(publicKey, chainCode);
    }

    template <class Archive>
    void save(Archive& archive) const {
        archive(publicKey, chainCode);
    }
};

} } }  // namespace ledger::core::api