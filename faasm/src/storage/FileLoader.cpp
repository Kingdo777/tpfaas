#include "FileLoader.h"
#include <boost/filesystem/operations.hpp>
#include <stdexcept>

#include <openssl/md5.h>


#include <wavm/WAVMWasmModule.h>

#include <faabric/util/config.h>
#include <faabric/util/files.h>
#include <faabric/util/func.h>
#include <faabric/util/logging.h>

#include <boost/filesystem.hpp>

namespace storage {

std::vector<uint8_t> FileLoader::hashBytes(const std::vector<uint8_t>& bytes)
{
    std::vector<uint8_t> result(MD5_DIGEST_LENGTH);
    MD5(reinterpret_cast<const unsigned char*>(bytes.data()),
        bytes.size(),
        result.data());

    return result;
}

std::string FileLoader::getHashFilePath(const std::string& path)
{
    return path + HASH_EXT;
}

std::vector<uint8_t> FileLoader::doCodegen(std::vector<uint8_t>& bytes,
                                           const std::string& fileName)
{
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
        return wasm::wavmCodegen(bytes, fileName);
}

void FileLoader::codegenForFunction(faabric::Message& msg)
{
    std::vector<uint8_t> bytes = loadFunctionWasm(msg);

    auto logger = faabric::util::getLogger();
    const std::string funcStr = faabric::util::funcToString(msg, false);

    if (bytes.empty()) {
        throw std::runtime_error("Loaded empty bytes for " + funcStr);
    }

    // Compare hashes
    std::vector<uint8_t> newHash = hashBytes(bytes);
    std::vector<uint8_t> oldHash;
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
    oldHash = loadFunctionObjectHash(msg);

    if ((!oldHash.empty()) && newHash == oldHash) {
        logger->debug("Skipping codegen for {}", funcStr);
        return;
    } else if (oldHash.empty()) {
        logger->debug("No old hash found for {}", funcStr);
    } else {
        logger->debug("Hashes differ for {}", funcStr);
    }

    // Run the actual codegen
    std::vector<uint8_t> objBytes;
    try {
        objBytes = doCodegen(bytes, funcStr);
    } catch (std::runtime_error& ex) {
        logger->error("Codegen failed for " + funcStr);
        throw ex;
    }

    // Upload the file contents and the hash
    uploadFunctionObjectFile(msg, objBytes);
    uploadFunctionObjectHash(msg, newHash);
}

void FileLoader::codegenForSharedObject(const std::string& inputPath)
{
    auto logger = faabric::util::getLogger();

    // Load the wasm
    std::vector<uint8_t> bytes = loadSharedObjectWasm(inputPath);

    // Check the hash
    std::vector<uint8_t> newHash = hashBytes(bytes);
    std::vector<uint8_t> oldHash = loadSharedObjectObjectHash(inputPath);

    if ((!oldHash.empty()) && newHash == oldHash) {
        logger->debug("Skipping codegen for {}", inputPath);
        return;
    }

    // Run the actual codegen
    std::vector<uint8_t> objBytes = doCodegen(bytes, inputPath);

    // Do the upload
    faabric::util::SystemConfig& conf = faabric::util::getSystemConfig();
    uploadSharedObjectObjectFile(inputPath, objBytes);
    uploadSharedObjectObjectHash(inputPath, newHash);
}

void checkFileExists(const std::string& path)
{
    if (!boost::filesystem::exists(path)) {
        const std::shared_ptr<spdlog::logger>& logger =
          faabric::util::getLogger();
        logger->error("File {} does not exist", path);
        throw std::runtime_error("Expected file does not exist");
    }
}

std::vector<uint8_t> loadFileBytes(const std::string& path)
{
    checkFileExists(path);
    return faabric::util::readFileToBytes(path);
}
}
