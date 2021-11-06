
<h1 align="center"> 📸 iconpp </h1>
<p align="center">
A C++17 library that allows you to grab an applications icon on linux
</p>

<p align="center">
    <a href="https://github.com/Soundux/iconpp/actions">
        <img src="https://img.shields.io/github/workflow/status/Soundux/iconpp/Test%20on%20Linux?label=Ubuntu%20Build&style=flat-square" alt="Ubuntu Build" />
    </a>
    <a href="https://github.com/Soundux/iconpp/blob/master/LICENSE">
        <img src="https://img.shields.io/github/license/Soundux/iconpp.svg?style=flat-square" alt="License" />
    </a>
</p>

---

## ⚙️ Configuration
### Tests
```cmake
set(iconpp_tests OFF)
```
> If set to `ON`, iconpp will build tests.


## 📎 Installation
- FetchContent
    ```cmake
    include(FetchContent)
    FetchContent_Declare(iconpp GIT_REPOSITORY "https://github.com/Soundux/iconpp")

    FetchContent_MakeAvailable(iconpp)
    target_link_libraries(<YourLibrary> iconpp)
    ```
- Git Submodule
    ```bash
    git submodule add "https://github.com/Soundux/iconpp"
    ```
    ```cmake
    # Somewhere in your CMakeLists.txt
    add_subdirectory("<path_to_iconpp>")
    target_link_libraries(<YourLibrary> iconpp)
    ```

## 📔 Usage

### Simple example
```cpp
#include <iconpp.hpp>

int main()
{
    auto by_pid = iconpp::icon::get(0);
    // Or by name: auto by_name = iconpp::icon::get("System Monitor");

    if (by_pid)
    {
        by_pid->save("by_pid.png");
        auto raw_image = by_pid->get_raw();
        auto base64_img = by_pid->get_base64();
    }

    return 0;
}
```

## ℹ️ Remarks
This library does not link its required libraries at compile time. Instead the dependencies are loaded at runtime.  
This is done to ensure that the library will compile everywhere but only grab icons on systems that support it - In order to make the library an optional dependency that will not break builds.

## ⛓️ Dependencies
- wnck3
- gdk3
- glib2
  
## ✅ Tested Systems
> The following distributions have been confirmed working with the listed dependencies.

| Distribution | Dependencies                                      |
| ------------ | ------------------------------------------------- |
| Ubuntu       | `libwnck-3-0` `libglib2.0-0` `libgdk-pixbuf2.0-0` |
| Arch Linux   | `libwnck3` `gdk-pixbuf2` `glib2`                  |