#include <iostream>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <chrono>
#include <vector>
#include <iomanip>

#include "skip.h"
#include "nlohmann/json.hpp"
#include "pugixml.hpp"

using json = nlohmann::json;

struct BenchmarkData {
    std::vector<int32_t> integers;
    std::vector<double> doubles;
    std::vector<std::string> strings;
};

void generate_benchmark_data(BenchmarkData& data, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        data.integers.push_back(i);
        data.doubles.push_back(i * 3.14159);
        data.strings.push_back("string_" + std::to_string(i));
    }
}

void benchmark_skip(const BenchmarkData& data) {
    std::cout << "--- Benchmarking SKIP ---" << std::endl;

    auto start_enc = std::chrono::high_resolution_clock::now();

    void* config = skip_create_base_config();
    skip_push_type_to_config(config, skip_int32, data.integers.size());
    skip_push_type_to_config(config, skip_float64, data.doubles.size());
    for (const auto& s : data.strings) {
        skip_push_type_to_config(config, skip_char, s.length() + 1);
    }

    uint64_t buffer_size = skip_get_data_size(config);
    char* buffer = new char[buffer_size];

    skip_write_index_to_buffer(config, buffer, buffer_size, (void*)data.integers.data(), 0);
    skip_write_index_to_buffer(config, buffer, buffer_size, (void*)data.doubles.data(), 1);
    for (size_t i = 0; i < data.strings.size(); ++i) {
        skip_write_index_to_buffer(config, buffer, buffer_size, (void*)data.strings[i].c_str(), 2 + i);
    }

    auto end_enc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> enc_duration = end_enc - start_enc;

    auto start_dec = std::chrono::high_resolution_clock::now();

    BenchmarkData decoded_data;
    decoded_data.integers.resize(data.integers.size());
    decoded_data.doubles.resize(data.doubles.size());

    skip_read_index_from_buffer(config, buffer, buffer_size, decoded_data.integers.data(), 0);
    skip_read_index_from_buffer(config, buffer, buffer_size, decoded_data.doubles.data(), 1);
    for (size_t i = 0; i < data.strings.size(); ++i) {
        char* str_ptr = (char*)skip_get_index_ptr(config, buffer, 2 + i);
        decoded_data.strings.push_back(str_ptr);
    }

    auto end_dec = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dec_duration = end_dec - start_dec;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Output Size: " << buffer_size << " bytes" << std::endl;
    std::cout << "Encoding Time: " << enc_duration.count() << " ms" << std::endl;
    std::cout << "Decoding Time: " << dec_duration.count() << " ms" << std::endl;

    skip_free_cfg(config);
    delete[] buffer;
}

void benchmark_json(const BenchmarkData& data) {
    std::cout << "--- Benchmarking JSON ---" << std::endl;

    auto start_enc = std::chrono::high_resolution_clock::now();

    json j;
    j["integers"] = data.integers;
    j["doubles"] = data.doubles;
    j["strings"] = data.strings;
    std::string json_str = j.dump();

    auto end_enc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> enc_duration = end_enc - start_enc;

    auto start_dec = std::chrono::high_resolution_clock::now();

    json decoded_j = json::parse(json_str);
    BenchmarkData decoded_data;
    decoded_data.integers = decoded_j["integers"].get<std::vector<int32_t>>();
    decoded_data.doubles = decoded_j["doubles"].get<std::vector<double>>();
    decoded_data.strings = decoded_j["strings"].get<std::vector<std::string>>();

    auto end_dec = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dec_duration = end_dec - start_dec;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Output Size: " << json_str.length() << " bytes" << std::endl;
    std::cout << "Encoding Time: " << enc_duration.count() << " ms" << std::endl;
    std::cout << "Decoding Time: " << dec_duration.count() << " ms" << std::endl;
}

void benchmark_xml(const BenchmarkData& data) {
    std::cout << "--- Benchmarking XML ---" << std::endl;

    auto start_enc = std::chrono::high_resolution_clock::now();

    pugi::xml_document doc;
    pugi::xml_node root = doc.append_child("root");

    pugi::xml_node integers = root.append_child("integers");
    for (int32_t i : data.integers) {
        integers.append_child("item").text().set(i);
    }

    pugi::xml_node doubles = root.append_child("doubles");
    for (double d : data.doubles) {
        doubles.append_child("item").text().set(d);
    }

    pugi::xml_node strings = root.append_child("strings");
    for (const auto& s : data.strings) {
        strings.append_child("item").text().set(s.c_str());
    }

    std::stringstream ss;
    doc.save(ss);
    std::string xml_str = ss.str();

    auto end_enc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> enc_duration = end_enc - start_enc;

    auto start_dec = std::chrono::high_resolution_clock::now();

    pugi::xml_document decoded_doc;
    decoded_doc.load_string(xml_str.c_str());
    BenchmarkData decoded_data;

    for (pugi::xml_node item : decoded_doc.child("root").child("integers").children("item")) {
        decoded_data.integers.push_back(item.text().as_int());
    }
    for (pugi::xml_node item : decoded_doc.child("root").child("doubles").children("item")) {
        decoded_data.doubles.push_back(item.text().as_double());
    }
    for (pugi::xml_node item : decoded_doc.child("root").child("strings").children("item")) {
        decoded_data.strings.push_back(item.text().as_string());
    }

    auto end_dec = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dec_duration = end_dec - start_dec;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Output Size: " << xml_str.length() << " bytes" << std::endl;
    std::cout << "Encoding Time: " << enc_duration.count() << " ms" << std::endl;
    std::cout << "Decoding Time: " << dec_duration.count() << " ms" << std::endl;
}

int main() {
    BenchmarkData data;
    generate_benchmark_data(data, 1000);

    benchmark_skip(data);
    std::cout << std::endl;
    benchmark_json(data);
    std::cout << std::endl;
    benchmark_xml(data);

    return 0;
}
