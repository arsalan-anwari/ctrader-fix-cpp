// #pragma once

// #include <algorithm>
// #include <array>
// #include <cstring>
// #include <functional>
// #include <random>
// #include <string>

// #include "memory.hpp"

// namespace ctrader::tools::random{

//     using namespace ctrader::tools::memory;

//     namespace internal {

//         static constexpr auto anumLookup = 
//             "0123456789"
//             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//             "abcdefghijklmnopqrstuvwxyz";



//     }

//     template<typename T = std::mt19937>
//     constexpr T new_random_generator() {
//         auto constexpr seed_bytes = sizeof(typename T::result_type) * T::state_size;
//         auto constexpr seed_len = seed_bytes / sizeof(std::seed_seq::result_type);
//         auto seed = std::array<std::seed_seq::result_type, seed_len>();
//         auto dev = std::random_device();
//         std::generate_n(begin(seed), seed_len, std::ref(dev));
//         auto seed_seq = std::seed_seq(begin(seed), end(seed));
//         return T{seed_seq};
//     }

//     template<std::size_t KEY_SIZE>
//     constexpr simple_buffer_t<char, KEY_SIZE> new_random_anum_key(){
//         auto rng = new_random_generator<>();
//         auto dist = std::uniform_int_distribution{{}, std::strlen(internal::anumLookup) - 1};
//         auto result = std::string(KEY_SIZE, '\0');
//         std::generate_n(begin(result), KEY_SIZE, [&]() { return internal::anumLookup[dist(rng)]; });

//         simple_buffer_t<char, KEY_SIZE> buff;
//         std::copy( result.begin(), result.end(), buff.data );
//         return buff;
//     }

// }