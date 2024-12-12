#pragma once

#include "converter.h"

#include "core/console/console.h"

#include <filesystem>

static bool compress_data = false;

void decode(std::filesystem::path input, std::filesystem::path output);
void encode(std::filesystem::path input, std::filesystem::path output);

void program(wk::ArgumentParser& args);
int main(int, char**);