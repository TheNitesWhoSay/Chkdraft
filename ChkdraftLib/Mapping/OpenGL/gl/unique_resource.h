#pragma once
#include <algorithm>
#include <utility>

namespace gl
{
    template <typename T>
    struct unique_resource // unique_resourcs cannot be copied, only moved or swapped, moving swaps the contained resources
    {
        unique_resource(unique_resource &) = delete;
        unique_resource & operator=(unique_resource &) = delete;

        T resource {};

        constexpr unique_resource() = default;
        constexpr unique_resource(const T & resource) : resource(resource) {}
        constexpr unique_resource(T && resource) : resource(std::move(resource)) {}

        unique_resource(unique_resource && other) noexcept { std::swap(resource, other.resource); }
        unique_resource & operator=(unique_resource && other) noexcept { std::swap(resource, other.resource); return *this; };

        constexpr T & operator=(const T & resource) { return this->resource = resource; }
        constexpr T & operator=(T && resource) { return this->resource = std::move(resource); }
        
        constexpr auto operator*() noexcept { return resource; }
        constexpr auto operator*() const noexcept { return resource; }
        constexpr auto operator->() noexcept { return &resource; }
        constexpr auto operator->() const noexcept { return &resource; }
        constexpr auto operator&() noexcept { return &resource; }
        constexpr auto operator&() const noexcept { return &resource; }
        friend constexpr auto operator<=>(const unique_resource&, const unique_resource&) = default;

        friend constexpr void swap(unique_resource<T> & l, unique_resource<T> & r) noexcept { std::swap(l.resource, r.resource); }
    };
}
