#pragma once

#include <cstdlib>
#include <new>
#include <cstddef>

#include <iostream>

template<typename T, std::size_t Alignment>
class aligned_allocator
{
public:
    using value_type = T;

    aligned_allocator() noexcept = default;
    template<class U> constexpr aligned_allocator(const aligned_allocator<U, Alignment> &other) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n)
    {
        void* p = nullptr;
        if (posix_memalign(&p, Alignment, n * sizeof(T)) != 0)
            throw std::bad_alloc();
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept
    {
        free(p);
    }

    template<typename U>
    struct rebind { using other = aligned_allocator<U, Alignment>; };
};
