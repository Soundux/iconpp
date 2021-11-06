#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <tl/expected.hpp>

namespace iconpp
{
    enum class error_t
    {
        wnck_not_found,
        process_not_found,
        icon_saving_failed,
    };

    class icon
    {
        static void *g_default_screen;

      private:
        icon();

      private:
        std::size_t m_size;
        std::shared_ptr<unsigned char[]> m_icon;

      public:
        std::string get_base64() const;
        void save(const std::filesystem::path &) const;
        std::shared_ptr<unsigned char[]> get_raw() const noexcept;

      public:
        static bool is_found() noexcept;
        static tl::expected<icon, error_t> get(int pid);
        static tl::expected<icon, error_t> get(const std::string &name);
    };
} // namespace iconpp