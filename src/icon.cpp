#include <cstring>
#include <dlfcn.h>
#include <fstream>
#include <icon.hpp>

namespace iconpp
{
    struct glist_t
    {
        void *data;
        glist_t *next;
        glist_t *prev;
    };

    bool (*gdk_init_check)(int *, char **);
    const char *(*g_base64_encode)(const unsigned char *, std::size_t);
    bool (*gdk_pixbuf_save_to_buffer)(void *, char **, std::size_t *, const char *, void *, void *);

    void *(*wnck_screen_get_default)();
    int (*wnck_window_get_pid)(void *);
    void *(*wnck_window_get_icon)(void *);
    void (*wnck_screen_force_update)(void *);
    glist_t *(*wnck_screen_get_windows)(void *);
    const char *(*wnck_window_get_name)(void *);

    bool load_symbols()
    {
        auto *gdk = dlopen("libgdk-3.so.0", RTLD_LAZY);
        auto *wnck = dlopen("libwnck-3.so.0", RTLD_LAZY);
        auto *glib = dlopen("libglib-2.0.so.0", RTLD_LAZY);
        auto *pixbuf = dlopen("libgdk_pixbuf-2.0.so.0", RTLD_LAZY);

        if (gdk && wnck && glib && pixbuf)
        {
            gdk_init_check = reinterpret_cast<decltype(gdk_init_check)>(dlsym(gdk, "gdk_init_check"));
            gdk_pixbuf_save_to_buffer = reinterpret_cast<decltype(gdk_pixbuf_save_to_buffer)>(dlsym(pixbuf, "gdk_pixbuf_save_to_buffer"));

            g_base64_encode = reinterpret_cast<decltype(g_base64_encode)>(dlsym(glib, "g_base64_encode"));

            wnck_window_get_pid = reinterpret_cast<decltype(wnck_window_get_pid)>(dlsym(wnck, "wnck_window_get_pid"));
            wnck_window_get_icon = reinterpret_cast<decltype(wnck_window_get_icon)>(dlsym(wnck, "wnck_window_get_icon"));
            wnck_window_get_name = reinterpret_cast<decltype(wnck_window_get_name)>(dlsym(wnck, "wnck_window_get_name"));
            wnck_screen_get_default = reinterpret_cast<decltype(wnck_screen_get_default)>(dlsym(wnck, "wnck_screen_get_default"));
            wnck_screen_get_windows = reinterpret_cast<decltype(wnck_screen_get_windows)>(dlsym(wnck, "wnck_screen_get_windows"));
            wnck_screen_force_update = reinterpret_cast<decltype(wnck_screen_force_update)>(dlsym(wnck, "wnck_screen_force_update"));

            return true;
        }

        return false;
    }

    void *icon::g_default_screen = []() -> void * {
        if (load_symbols())
        {
            if (gdk_init_check(nullptr, nullptr))
            {
                return wnck_screen_get_default();
            }
        }
        return nullptr;
    }();

    icon::icon() = default;

    tl::expected<icon, error_t> icon::get(int pid)
    {
        if (!g_default_screen)
            return tl::make_unexpected(error_t::wnck_not_found);

        wnck_screen_force_update(g_default_screen);
        auto *windows = wnck_screen_get_windows(g_default_screen);

        for (auto *item = windows; item != nullptr; item = item->next)
        {
            auto *window = reinterpret_cast<void *>(item->data);
            if (wnck_window_get_pid(window) == pid)
            {
                auto *icon_buf = wnck_window_get_icon(window);

                char *buffer{};
                std::size_t size = 0;

                if (!gdk_pixbuf_save_to_buffer(icon_buf, &buffer, &size, "png", nullptr, nullptr))
                {
                    return tl::make_unexpected(error_t::icon_saving_failed);
                }

                icon rtn;
                rtn.m_size = size;
                rtn.m_icon = std::make_shared<unsigned char[]>(size);

                memcpy(rtn.m_icon.get(), buffer, size);
                return rtn;
            }
        }

        return tl::make_unexpected(error_t::process_not_found);
    }

    tl::expected<icon, error_t> icon::get(const std::string &name)
    {
        if (!g_default_screen)
            return tl::make_unexpected(error_t::wnck_not_found);

        wnck_screen_force_update(g_default_screen);
        auto *windows = wnck_screen_get_windows(g_default_screen);

        for (auto *item = windows; item != nullptr; item = item->next)
        {
            auto *window = item->data;
            if (wnck_window_get_name(window) == name)
            {
                return get(wnck_window_get_pid(window));
            }
        }

        return tl::make_unexpected(error_t::process_not_found);
    }

    std::size_t icon::get_size() const noexcept
    {
        return m_size;
    }

    bool icon::is_found() noexcept
    {
        return g_default_screen;
    }

    std::string icon::get_base64() const
    {
        return g_base64_encode(m_icon.get(), m_size);
    }

    std::shared_ptr<unsigned char[]> icon::get_raw() const noexcept
    {
        return m_icon;
    }

    void icon::save(const std::filesystem::path &path) const
    {
        std::ofstream out(path);
        out.write(reinterpret_cast<const char *>(m_icon.get()), static_cast<std::streamsize>(m_size));

        out.close();
    }

    tl::expected<std::vector<std::pair<int, std::string>>, error_t> icon::get_windows()
    {
        if (!g_default_screen)
            return tl::make_unexpected(error_t::wnck_not_found);

        std::vector<std::pair<int, std::string>> rtn;

        wnck_screen_force_update(g_default_screen);
        auto *windows = wnck_screen_get_windows(g_default_screen);

        for (auto *item = windows; item != nullptr; item = item->next)
        {
            auto *window = reinterpret_cast<void *>(item->data);
            rtn.emplace_back(std::make_pair(wnck_window_get_pid(window), wnck_window_get_name(window)));
        }

        return rtn;
    }
} // namespace iconpp