cmake_minimum_required(VERSION 3.0)

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_executable(ledger-core-crypto-tests main.cpp DigestTest.cpp EncryptionTest.cpp DeterministicPublicKeyTest.cpp Secp256k1Test.cpp)

target_link_libraries(ledger-core-crypto-tests gtest gtest_main)
target_link_libraries(ledger-core-crypto-tests ledger-core-bundle)
target_link_libraries(ledger-core-crypto-tests ledger-test)

add_test (NAME ledger-core-crypto-tests COMMAND ledger-core-crypto-tests)
