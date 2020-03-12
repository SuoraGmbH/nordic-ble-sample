#pragma once

#include <array>
#include <type_traits>

namespace suora::ble {

namespace detail {

    template <class T, std::size_t N, std::size_t... Is>
    constexpr std::array<T, N - 1> to_array_without_null_implementation(const char (&data)[N],
                                                                        std::index_sequence<Is...> indexSequence) {
        static_assert(indexSequence.size() == N - 1);
        return {{static_cast<T>(data[Is])...}};
    }
}

template <class T = std::uint8_t, std::size_t N>
constexpr std::array<T, N - 1> to_array_without_null(const char (&data)[N]) {
    return detail::to_array_without_null_implementation<T>(data, std::make_index_sequence<N - 1>{});
}
}
