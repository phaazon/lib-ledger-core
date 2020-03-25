// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from stellar_like_wallet.djinni

#ifndef DJINNI_GENERATED_STELLARLIKETRANSACTION_HPP_JNI_
#define DJINNI_GENERATED_STELLARLIKETRANSACTION_HPP_JNI_

#include "../../api/StellarLikeTransaction.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class StellarLikeTransaction final : ::djinni::JniInterface<::ledger::core::api::StellarLikeTransaction, StellarLikeTransaction> {
public:
    using CppType = std::shared_ptr<::ledger::core::api::StellarLikeTransaction>;
    using CppOptType = std::shared_ptr<::ledger::core::api::StellarLikeTransaction>;
    using JniType = jobject;

    using Boxed = StellarLikeTransaction;

    ~StellarLikeTransaction();

    static CppType toCpp(JNIEnv* jniEnv, JniType j) { return ::djinni::JniClass<StellarLikeTransaction>::get()._fromJava(jniEnv, j); }
    static ::djinni::LocalRef<JniType> fromCppOpt(JNIEnv* jniEnv, const CppOptType& c) { return {jniEnv, ::djinni::JniClass<StellarLikeTransaction>::get()._toJava(jniEnv, c)}; }
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c) { return fromCppOpt(jniEnv, c); }

private:
    StellarLikeTransaction();
    friend ::djinni::JniClass<StellarLikeTransaction>;
    friend ::djinni::JniInterface<::ledger::core::api::StellarLikeTransaction, StellarLikeTransaction>;

};

}  // namespace djinni_generated
#endif //DJINNI_GENERATED_STELLARLIKETRANSACTION_HPP_JNI_