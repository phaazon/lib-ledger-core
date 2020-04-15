// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from messages.djinni

#ifndef DJINNI_GENERATED_COSMOSLIKEMSGEDITVALIDATOR_HPP_JNI_
#define DJINNI_GENERATED_COSMOSLIKEMSGEDITVALIDATOR_HPP_JNI_

#include "../../api/CosmosLikeMsgEditValidator.hpp"
#include "djinni_support.hpp"

namespace djinni_generated {

class CosmosLikeMsgEditValidator final {
public:
    using CppType = ::ledger::core::api::CosmosLikeMsgEditValidator;
    using JniType = jobject;

    using Boxed = CosmosLikeMsgEditValidator;

    ~CosmosLikeMsgEditValidator();

    static CppType toCpp(JNIEnv* jniEnv, JniType j);
    static ::djinni::LocalRef<JniType> fromCpp(JNIEnv* jniEnv, const CppType& c);

private:
    CosmosLikeMsgEditValidator();
    friend ::djinni::JniClass<CosmosLikeMsgEditValidator>;

    const ::djinni::GlobalRef<jclass> clazz { ::djinni::jniFindClass("co/ledger/core/CosmosLikeMsgEditValidator") };
    const jmethodID jconstructor { ::djinni::jniGetMethodID(clazz.get(), "<init>", "(Lco/ledger/core/CosmosLikeValidatorDescription;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V") };
    const jfieldID field_description { ::djinni::jniGetFieldID(clazz.get(), "description", "Lco/ledger/core/CosmosLikeValidatorDescription;") };
    const jfieldID field_validatorAddress { ::djinni::jniGetFieldID(clazz.get(), "validatorAddress", "Ljava/lang/String;") };
    const jfieldID field_commissionRate { ::djinni::jniGetFieldID(clazz.get(), "commissionRate", "Ljava/lang/String;") };
    const jfieldID field_minSelfDelegation { ::djinni::jniGetFieldID(clazz.get(), "minSelfDelegation", "Ljava/lang/String;") };
};

}  // namespace djinni_generated
#endif //DJINNI_GENERATED_COSMOSLIKEMSGEDITVALIDATOR_HPP_JNI_