#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "libuplinkc.h"

const char* access_string = "12Qy3wN5ED7M9ieApcdJYmQuxJDrzeEcVyPmeesnHCtYWJwsgzfP6UQ51yGEYMfiH8sUrPEfhm2NqhjtZMUvQ2HWHLqJ876AvL959vuW2WJccpAyxs7k669HdN6rBs2AqNoj34uSzD6vQGXLUhu9t8bgwCByeoVZovcKUc7wxgpfPpBLWpwsfZZtBd7PSE44LrEqoqp7PCg2dF3j1bDeRB7SH17wnkzWW6FZGzm3JTsYDXeybMvNxNLMEXCLD8XZ3dnfZwsSKq23NsADJmGd7yoQ3qTdqpJKfDtx3BvXiSyTbMLNfGrN1gHngvTz9jw7fYgmRLzzoofhVB5zH3nzFchqvjN8";

#define require_noerror(err)                                                                                           \
    do {                                                                                                               \
        if (err != NULL) {                                                                                             \
            printf("failed:\n\t%s:%d: [%d] %s\n", __FILE__, __LINE__, err->code, err->message);                        \
            exit(1);                                                                                                   \
        }                                                                                                              \
    } while (0)

int main(int argc, char* argv[]) {

    int iterations = 0;
    if (argc > 1) {
        iterations = std::atoi(argv[1]);
    } else {
        char* env_value = std::getenv("ITERATIONS");
        iterations = std::atoi(env_value);
    }
    if (iterations < 1) {
        iterations = 1;
    }

    UplinkAccessResult access_result = uplink_parse_access(access_string);
    require_noerror(access_result.error);
    UplinkProjectResult project_result = uplink_open_project(access_result.access);
    require_noerror(project_result.error);

    for (int i = 0; i < iterations; i++) {
        const auto start_time = std::chrono::steady_clock::now();
        std::string filename = "upload_file_" + std::to_string(i);
        UplinkUploadResult upload_result = uplink_upload_object(project_result.project, "gramine-test", filename.c_str(), NULL);
        require_noerror(upload_result.error);
        UplinkUpload *upload = upload_result.upload;

        const int buffer_size = 64*1024;
        char buffer[buffer_size];

        // Open the binary file in binary mode
        std::ifstream file("./64K", std::ios::binary);

        // Check if the file is opened successfully
        if (!file.is_open()) {
            std::cerr << "Error opening file." << std::endl;
            return 1;
        }

        size_t uploaded_total = 0;
        while (!file.eof()) {
            file.read(buffer, buffer_size);
            std::streamsize bytesRead = file.gcount();
            UplinkWriteResult result = uplink_upload_write(upload, buffer, bytesRead);
            uploaded_total += result.bytes_written;
            uplink_free_write_result(result);
        }
        UplinkError *commit_err = uplink_upload_commit(upload);
        require_noerror(commit_err);
        const auto end_time = std::chrono::steady_clock::now();
        std::cout << i+1 << " - " << uploaded_total << " bytes successfully uploaded in " << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() << "ms" << std::endl;
        uplink_free_upload_result(upload_result);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    uplink_free_project_result(project_result);
    uplink_free_access_result(access_result);
}