/*
 *
 * RippleLikeWallet
 *
 * Created by El Khalil Bellakrid on 06/01/2019.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ledger
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <algorithm>

#include <core/async/wait.h>
#include <core/api/ErrorCode.hpp>
#include <core/api/AccountCallback.hpp>
#include <core/api/ConfigurationDefaults.hpp>
#include <core/api/KeychainEngines.hpp>
#include <core/wallet/database/AccountDatabaseHelper.h>
#include <database/RippleLikeAccountDatabaseHelper.h>
#include <RippleLikeWallet.h>
#include <RippleLikeAccount.h>
#include <RippleLikeExtendedPublicKey.h>

namespace ledger {
    namespace core {
        const api::WalletType RippleLikeWallet::type = api::WalletType::ETHEREUM;

        RippleLikeWallet::RippleLikeWallet(const std::string &name,
                                           const std::shared_ptr<RippleLikeBlockchainExplorer> &explorer,
                                           const std::shared_ptr<RippleLikeBlockchainObserver> &observer,
                                           const std::shared_ptr<RippleLikeKeychainFactory> &keychainFactory,
                                           const RippleLikeAccountSynchronizerFactory &synchronizer,
                                           const std::shared_ptr<WalletPool> &pool, const api::Currency &network,
                                           const std::shared_ptr<DynamicObject> &configuration,
                                           const DerivationScheme &scheme
        )
                : AbstractWallet(name, network, pool, configuration, scheme) {
            _explorer = explorer;
            _observer = observer;
            _keychainFactory = keychainFactory;
            _synchronizerFactory = synchronizer;
        }

        bool RippleLikeWallet::isSynchronizing() {
            return false;
        }

        std::shared_ptr<api::EventBus> RippleLikeWallet::synchronize() {
            return nullptr;
        }

        FuturePtr<ledger::core::api::Account>
        RippleLikeWallet::newAccountWithInfo(const api::AccountCreationInfo &info) {
            if (info.chainCodes.size() != 1 || info.publicKeys.size() != 1 || info.owners.size() != 1)
                throw make_exception(api::ErrorCode::INVALID_ARGUMENT,
                                     "Account creation info are inconsistent (only one public key is needed)");
            auto self = getSelf();
            return async<api::ExtendedKeyAccountCreationInfo>([self, info]() -> api::ExtendedKeyAccountCreationInfo {
                if (info.owners.size() != info.derivations.size() || info.owners.size() != info.chainCodes.size() ||
                    info.publicKeys.size() != info.owners.size())
                    throw make_exception(api::ErrorCode::INVALID_ARGUMENT,
                                         "Account creation info are inconsistent (size of arrays differs)");
                api::ExtendedKeyAccountCreationInfo result;

                if (info.chainCodes[0].size() != 32 || info.publicKeys[0].size() != 33)
                    throw make_exception(api::ErrorCode::INVALID_ARGUMENT,
                                         "Account creation info are inconsistent (contains invalid public key(s))");
                DerivationPath occurencePath(info.derivations[0]);

                auto xpub = RippleLikeExtendedPublicKey::fromRaw(
                        self->getCurrency(),
                        Option<std::vector<uint8_t>>(),
                        info.publicKeys[0],
                        info.chainCodes[0],
                        info.derivations[0]
                );
                result.owners.push_back(info.owners[0]);
                result.derivations.push_back(info.derivations[0]);
                result.extendedKeys.push_back(xpub->toBase58());
                result.index = info.index;
                return result;
            }).flatMap<std::shared_ptr<ledger::core::api::Account>>(getContext(),
                                                                    [self](const api::ExtendedKeyAccountCreationInfo &info) -> Future<std::shared_ptr<ledger::core::api::Account>> {
                                                                        return self->newAccountWithExtendedKeyInfo(
                                                                                info);
                                                                    });
        }

        FuturePtr<ledger::core::api::Account>
        RippleLikeWallet::newAccountWithExtendedKeyInfo(const api::ExtendedKeyAccountCreationInfo &info) {

            if (info.extendedKeys.empty()) {
                throw make_exception(api::ErrorCode::INVALID_ARGUMENT,
                                     "Empty extended keys passed to newAccountWithExtendedKeyInfo");
            }

            auto self = getSelf();
            auto scheme = getDerivationScheme();
            scheme.setCoinType(getCurrency().bip44CoinType).setAccountIndex(info.index);
            auto xpubPath = scheme.getSchemeTo(DerivationSchemeLevel::ACCOUNT_INDEX).getPath();
            auto index = info.index;
            return async<std::shared_ptr<api::Account> >([=]() -> std::shared_ptr<api::Account> {
                auto keychain = self->_keychainFactory->build(
                        index,
                        xpubPath,
                        getConfiguration(),
                        info,
                        getAccountInternalPreferences(index),
                        getCurrency()
                );
                soci::session sql(self->getDatabase()->getPool());
                soci::transaction tr(sql);
                auto accountUid = AccountDatabaseHelper::createAccountUid(self->getWalletUid(), index);
                if (AccountDatabaseHelper::accountExists(sql, self->getWalletUid(), index))
                    throw make_exception(api::ErrorCode::ACCOUNT_ALREADY_EXISTS,
                                         "Account {}, for wallet '{}', already exists", index, self->getWalletUid());
                AccountDatabaseHelper::createAccount(sql, self->getWalletUid(), index);
                RippleLikeAccountDatabaseHelper::createAccount(sql, self->getWalletUid(), index,
                                                               info.extendedKeys[info.extendedKeys.size() - 1]);
                tr.commit();
                auto account = std::static_pointer_cast<api::Account>(std::make_shared<RippleLikeAccount>(
                        self->shared_from_this(),
                        index,
                        self->_explorer,
                        self->_observer,
                        self->_synchronizerFactory(),
                        keychain
                ));
                self->addAccountInstanceToInstanceCache(std::dynamic_pointer_cast<AbstractAccount>(account));
                return account;
            });
        }

        Future<api::ExtendedKeyAccountCreationInfo>
        RippleLikeWallet::getExtendedKeyAccountCreationInfo(int32_t accountIndex) {
            auto self = std::dynamic_pointer_cast<RippleLikeWallet>(shared_from_this());
            return async<api::ExtendedKeyAccountCreationInfo>(
                    [self, accountIndex]() -> api::ExtendedKeyAccountCreationInfo {
                        api::ExtendedKeyAccountCreationInfo info;
                        info.index = accountIndex;
                        auto scheme = self->getDerivationScheme();
                        scheme.setCoinType(self->getCurrency().bip44CoinType).setAccountIndex(accountIndex);;
                        auto keychainEngine = self->getConfiguration()->getString(
                                api::Configuration::KEYCHAIN_ENGINE).value_or(
                                api::ConfigurationDefaults::DEFAULT_KEYCHAIN);
                        if (keychainEngine == api::KeychainEngines::BIP32_P2PKH ||
                            keychainEngine == api::KeychainEngines::BIP49_P2SH) {
                            auto xpubPath = scheme.getSchemeTo(DerivationSchemeLevel::ACCOUNT_INDEX).getPath();
                            info.derivations.push_back(xpubPath.toString());
                            info.owners.push_back(std::string("main"));
                        } else {
                            throw make_exception(api::ErrorCode::IMPLEMENTATION_IS_MISSING,
                                                 "No implementation found found for keychain {}", keychainEngine);
                        }

                        return info;
                    });
        }

        Future<api::AccountCreationInfo> RippleLikeWallet::getAccountCreationInfo(int32_t accountIndex) {
            auto self = std::dynamic_pointer_cast<RippleLikeWallet>(shared_from_this());
            return getExtendedKeyAccountCreationInfo(accountIndex).map<api::AccountCreationInfo>(getContext(),
                                                                                                 [self, accountIndex](
                                                                                                         const api::ExtendedKeyAccountCreationInfo info) -> api::AccountCreationInfo {
                                                                                                     api::AccountCreationInfo result;
                                                                                                     result.index = accountIndex;
                                                                                                     auto length = info.derivations.size();
                                                                                                     for (auto i = 0;
                                                                                                          i <
                                                                                                          length; i++) {
                                                                                                         DerivationPath path(
                                                                                                                 info.derivations[i]);
                                                                                                         auto owner = info.owners[i];
                                                                                                         result.derivations.push_back(
                                                                                                                 path.getParent().toString());
                                                                                                         result.derivations.push_back(
                                                                                                                 path.toString());
                                                                                                         result.owners.push_back(
                                                                                                                 owner);
                                                                                                         result.owners.push_back(
                                                                                                                 owner);
                                                                                                     }
                                                                                                     return result;
                                                                                                 });
        }

        std::shared_ptr<RippleLikeWallet> RippleLikeWallet::getSelf() {
            return std::dynamic_pointer_cast<RippleLikeWallet>(shared_from_this());
        }

        std::shared_ptr<AbstractAccount>
        RippleLikeWallet::createAccountInstance(soci::session &sql, const std::string &accountUid) {
            RippleLikeAccountDatabaseEntry entry;
            RippleLikeAccountDatabaseHelper::queryAccount(sql, accountUid, entry);
            auto scheme = getDerivationScheme();
            scheme.setCoinType(getCurrency().bip44CoinType).setAccountIndex(entry.index);
            auto xpubPath = scheme.getSchemeTo(DerivationSchemeLevel::ACCOUNT_INDEX).getPath();
            auto keychain = _keychainFactory->restore(entry.index, xpubPath, getConfiguration(), entry.address,
                                                      getAccountInternalPreferences(entry.index), getCurrency());
            return std::make_shared<RippleLikeAccount>(shared_from_this(),
                                                       entry.index,
                                                       _explorer,
                                                       _observer,
                                                       _synchronizerFactory(),
                                                       keychain);
        }

        std::shared_ptr<RippleLikeBlockchainExplorer> RippleLikeWallet::getBlockchainExplorer() {
            return _explorer;
        }
    }
}
