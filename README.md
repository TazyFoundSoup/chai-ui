# Chai-ui
A **cozy and modern** UI and windowing engine.

![Version | v1.0.0][ver]

 ---
 > Warning: Chai-ui is still in alpha stages

Chai-ui is a Windows only UI and windowing library with the goal to have minimum stress and maximum debuggability.

## To use

> For a full in-depth guide of chai-ui, check out [the documentation.][docs]

**Make sure you have the following requirements: Windows + MSVC + Direct2D/DirectWrite libraries**

Visit the [releases][releases] page and download the `.lib` file attached.

When compiling your application, make sure to link the file. For example:

```cmd
cl.exe /EHsc main.cpp ChaiUI.lib /Iinclude /link /subsystem:windows d2d1.lib dwrite.lib ole32.lib uuid.lib gdi32.lib user32.lib
````


- `main.cpp` → your source  
- `ChaiUI.lib` → the static library you downloaded  
- `/Iinclude` → points to the header files  
- Link the required Windows/Direct2D libraries


[ver]: https://img.shields.io/badge/version-C1.0.R0-blue
[releases]: https://github.com/TazyFoundSoup/chai-ui/releases/latest
[docs]: https://github.com/TazyFoundSoup/chai-ui/tree/main/docs