# Chai UI

![Version](https://img.shields.io/badge/version-v0.1.0-blue)

**Chai UI is a cozy and modern UI and windowing engine for Windows.**

> **Warning:** Chai UI is still in the alpha stage of development. APIs may change, and there might be bugs.

---

## About The Project

Chai UI is a native Windows library for creating graphical user interfaces. It aims to provide a simple and intuitive API for building applications with a focus on performance and debuggability. If you are looking for a lightweight and easy-to-use UI library for your Windows-based C++ projects, Chai UI might be a great fit.

### Features

*   **Lightweight and Performant:** Designed to be fast and have a small footprint.
*   **Modern C++:** Utilizes modern C++ features for a clean and maintainable codebase.
*   **Easy to Debug:** Built with debuggability in mind to help you find and fix issues quickly.
*   **Windows Native:** Built specifically for Windows to ensure the best performance and integration.

## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

*   A C++ compiler that supports C++17 or later.
*   Windows 10 or later.
*   CMake (optional, but recommended for building).

### Installation

1.  **Clone the repo**
    ```sh
    git clone https://github.com/your_username/chai-ui.git
    ```
2.  **Include the headers**
    
    Add the `include` directory to your project's include paths.
    
3.  **Link the library**
    
    Build the library and link it with your project. (Further instructions on building will be added soon).
    

## Usage

Here is a basic example of how to create a window using Chai UI:

```cpp
#include <Chai/Inter/Display.hpp>

int main() {
    Chai::Display display("My First Chai UI Window", 800, 600);

    while (display.is_open()) {
        display.poll_events();
    }

    return 0;
}
```

_(Note: This is a hypothetical example based on the file structure. The actual API might differ.)_

## Contributing

Contributions are what make the open-source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".

1.  Fork the Project
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4.  Push to the Branch (`git push origin feature/AmazingFeature`)
5.  Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.
