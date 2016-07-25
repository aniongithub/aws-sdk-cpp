/*
* Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License").
* You may not use this file except in compliance with the License.
* A copy of the License is located at
*
*  http://aws.amazon.com/apache2.0
*
* or in the "license" file accompanying this file. This file is distributed
* on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
* express or implied. See the License for the specific language governing
* permissions and limitations under the License.
*/
#include <aws/s3-encryption/modules/CryptoModuleFactory.h>
#include <aws/core/utils/logging/LogMacros.h>

namespace Aws
{
namespace S3Encryption
{
namespace Modules
{
static const char* const Allocation_Tag = "CryptoModuleFactory";


CryptoModuleFactory::CryptoModuleFactory()
{
    m_cryptoFactories.insert(std::pair < CryptoMode, std::shared_ptr<CryptoModuleAbstractFactory>>(CryptoModuleFactoryEO().HandlesMode(), Aws::MakeShared<CryptoModuleFactoryEO>(Allocation_Tag, CryptoModuleFactoryEO())));
    m_cryptoFactories.insert(std::pair < CryptoMode, std::shared_ptr<CryptoModuleAbstractFactory>>(CryptoModuleFactoryAE().HandlesMode(), Aws::MakeShared<CryptoModuleFactoryAE>(Allocation_Tag, CryptoModuleFactoryAE())));
    m_cryptoFactories.insert(std::pair < CryptoMode, std::shared_ptr<CryptoModuleAbstractFactory>>(CryptoModuleFactoryStrictAE().HandlesMode(), Aws::MakeShared<CryptoModuleFactoryStrictAE>(Allocation_Tag, CryptoModuleFactoryStrictAE())));
}

std::shared_ptr<CryptoModule> CryptoModuleFactory::FetchCryptoModule(const std::shared_ptr<Aws::S3Encryption::Materials::EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig,
    const std::shared_ptr<Aws::S3::S3Client>& s3Client)
{
    auto entry = m_cryptoFactories.find(cryptoConfig.GetCryptoMode());
    if (entry == m_cryptoFactories.end())
    {
        AWS_LOGSTREAM_ERROR(Allocation_Tag, "Crypto Module Factory not found. Factory does not exist for current crypto mode.")
        return nullptr;
    }
    else
    {
        std::shared_ptr<CryptoModuleAbstractFactory> factory = entry->second;
        return factory->CreateModule(encryptionMaterials, cryptoConfig, s3Client);
    }
}


CryptoModuleFactoryEO::CryptoModuleFactoryEO() :
    m_cryptoModuleEO(nullptr)
{
}

std::shared_ptr<CryptoModule> CryptoModuleFactoryEO::CreateModule(const std::shared_ptr<Aws::S3Encryption::Materials::EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig,
    const std::shared_ptr<Aws::S3::S3Client>& s3Client)
{
    if (m_cryptoModuleEO == nullptr)
    {
        m_cryptoModuleEO = Aws::MakeShared<CryptoModuleEO>(Allocation_Tag);
        m_cryptoModuleEO->SetEncryptionMaterials(encryptionMaterials);
        m_cryptoModuleEO->SetCryptoConfiguration(cryptoConfig);
        m_cryptoModuleEO->SetS3Client(s3Client);
        return m_cryptoModuleEO;
    }
    else
    {
        m_cryptoModuleEO->SetEncryptionMaterials(encryptionMaterials);
        m_cryptoModuleEO->SetCryptoConfiguration(cryptoConfig);
        m_cryptoModuleEO->SetS3Client(s3Client);
        return m_cryptoModuleEO;
    }
}

CryptoMode CryptoModuleFactoryEO::HandlesMode() const
{
    return CryptoMode::ENCRYPTION_ONLY;
}

CryptoModuleFactoryAE::CryptoModuleFactoryAE() :
    m_cryptoModuleAE(nullptr)
{
}

std::shared_ptr<CryptoModule> CryptoModuleFactoryAE::CreateModule(const std::shared_ptr<Aws::S3Encryption::Materials::EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig,
    const std::shared_ptr<Aws::S3::S3Client>& s3Client)
{
    if (m_cryptoModuleAE == nullptr)
    {
        m_cryptoModuleAE = Aws::MakeShared<CryptoModuleAE>(Allocation_Tag, encryptionMaterials, cryptoConfig, s3Client);
        return m_cryptoModuleAE;
    }
    else
    {
        m_cryptoModuleAE->SetEncryptionMaterials(encryptionMaterials);
        m_cryptoModuleAE->SetCryptoConfiguration(cryptoConfig);
        m_cryptoModuleAE->SetS3Client(s3Client);
        return m_cryptoModuleAE;
    }
}

CryptoMode CryptoModuleFactoryAE::HandlesMode() const
{
    return CryptoMode::AUTHENTICATED_ENCRYPTION;
}

CryptoModuleFactoryStrictAE::CryptoModuleFactoryStrictAE() :
    m_cryptoModuleStrictAE(nullptr)
{
}

std::shared_ptr<CryptoModule> CryptoModuleFactoryStrictAE::CreateModule(const std::shared_ptr<Aws::S3Encryption::Materials::EncryptionMaterials>& encryptionMaterials, const CryptoConfiguration& cryptoConfig,
    const std::shared_ptr<Aws::S3::S3Client>& s3Client)
{
    if (m_cryptoModuleStrictAE == nullptr)
    {
        m_cryptoModuleStrictAE = Aws::MakeShared<CryptoModuleStrictAE>(Allocation_Tag, encryptionMaterials, cryptoConfig, s3Client);
        return m_cryptoModuleStrictAE;
    }
    else
    {
        m_cryptoModuleStrictAE->SetEncryptionMaterials(encryptionMaterials);
        m_cryptoModuleStrictAE->SetCryptoConfiguration(cryptoConfig);
        m_cryptoModuleStrictAE->SetS3Client(s3Client);
        return m_cryptoModuleStrictAE;
    }
}

CryptoMode CryptoModuleFactoryStrictAE::HandlesMode() const
{
    return CryptoMode::STRICT_AUTHENTICATED_ENCRYPTION;
}

} //namespace Modules
} //namespace S3Encryption
} //namespace Aws