# PrettyFly Engine

A (WIP) specialized game engine for 3rd-person aerial vehicle games, built from scratch in C++ just for fun.

## Prerequisites

### Windows
- Visual Studio 2026 with the **Desktop development with C++** workload
- [CMake](https://cmake.org) 3.20+
- [Git](https://git-scm.com)

### Mac/Linux
- CMake 3.20+
- Git
- [Homebrew](https://brew.sh) (Mac) or apt/pacman (Linux)
- OpenGL development headers (Linux only — installed automatically by `setup.sh`)

---

## First-Time Setup

### Windows
```powershell
powershell -ExecutionPolicy Bypass -File setup.ps1
```
Then open `engine/build/MyEngine.sln` in Visual Studio and set **MyEngine** as the startup project.

### Mac/Linux
```bash
chmod +x setup.sh configure.sh build.sh run.sh
./setup.sh
```

---

## Daily Workflow

| Task | Windows | Mac/Linux |
|---|---|---|
| Configure | `.\configure.ps1` | `./configure.sh` |
| Build | `.\build.ps1` | `./build.sh` |
| Run | `.\run.ps1` | `./run.sh` |
| Release build | `.\build.ps1 -Release` | `./build.sh --release` |
| Release run | `.\run.ps1 -Release` | `./run.sh --release` |

Re-run `configure` only when adding new dependencies or changing CMake settings. 

** For normal iteration, `run` is all you need — it will build automatically if no binary is found. **

---

## Adding Dependencies

Add packages to `engine/vcpkg.json` then re-run `configure`:

```json
{
    "name": "prettyfly-engine",
    "version": "0.1.0",
    "dependencies": [
        "glfw3",
        "glad",
        "glm",
        "your-new-package-here"
    ]
}
```
