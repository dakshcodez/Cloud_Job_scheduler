# Running on Windows

This guide provides several options for running the Cloud Job Scheduler Simulator on Windows.

## Option 1: Windows Subsystem for Linux (WSL) - Recommended

WSL allows you to run Linux directly on Windows. This is the easiest option since the project is already configured for Linux.

### Installation Steps:

1. **Install WSL** (if not already installed):
   ```powershell
   wsl --install
   ```
   Or for older Windows versions, follow: https://docs.microsoft.com/windows/wsl/install

2. **Restart your computer** when prompted

3. **Open WSL terminal** (Ubuntu or your chosen Linux distribution)

4. **Navigate to your project**:
   ```bash
   cd /mnt/c/path/to/CPU_scheduler
   # Or wherever you cloned/downloaded the project
   ```

5. **Build and run**:
   ```bash
   make clean
   make
   ./scheduler
   ```

**Advantages:**
- No code changes needed
- Works exactly as on Linux
- Can use all Linux tools

---

## Option 2: MinGW-w64 (Native Windows)

MinGW provides a GCC compiler for Windows.

### Installation Steps:

1. **Download MinGW-w64**:
   - Option A: Use MSYS2 (recommended): https://www.msys2.org/
   - Option B: Use standalone installer: https://www.mingw-w64.org/downloads/

2. **If using MSYS2**:
   ```bash
   # In MSYS2 terminal
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc
   pacman -S make
   ```

3. **Add to PATH**:
   - Add `C:\msys64\mingw64\bin` to your Windows PATH environment variable
   - Or use the MSYS2 terminal directly

4. **Build the project**:
   ```bash
   # In MSYS2 MinGW 64-bit terminal
   cd /c/path/to/CPU_scheduler
   make clean
   make
   ./scheduler.exe
   ```

**Note:** You may need to modify the Makefile slightly for Windows paths.

---

## Option 3: Visual Studio / MSVC

### Using Visual Studio:

1. **Install Visual Studio Community** (free): https://visualstudio.microsoft.com/

2. **Install C/C++ workload** during installation

3. **Create a new project** or open existing:
   - File → New → Project → Visual C++ → Empty Project

4. **Add all source files**:
   - Right-click project → Add → Existing Item
   - Add all `.c` and `.h` files

5. **Build and run**:
   - Press F5 or Build → Build Solution
   - Run from Debug menu

### Using Command Line:

```cmd
# Open "Developer Command Prompt for VS" or "x64 Native Tools Command Prompt"
cd C:\path\to\CPU_scheduler

# Compile all files
cl /Fe:scheduler.exe *.c

# Run
scheduler.exe
```

**Note:** You'll need to adapt the code slightly for MSVC (different function names, etc.)

---

## Option 4: Code::Blocks / Dev-C++

### Using Code::Blocks:

1. **Download Code::Blocks**: http://www.codeblocks.org/

2. **Create new project**:
   - File → New → Project → Console Application → C

3. **Add source files**:
   - Right-click project → Add files
   - Add all `.c` and `.h` files

4. **Build and run**:
   - Build → Build and Run (F9)

### Using Dev-C++:

1. **Download Dev-C++**: https://www.bloodshed.net/devcpp.html

2. **Open project** or create new project

3. **Add all source files** to the project

4. **Compile and run** (F9)

---

## Option 5: Online Compilers (Quick Testing)

For quick testing without installation:

1. **OnlineGDB**: https://www.onlinegdb.com/online_c_compiler
   - Upload all `.c` and `.h` files
   - Compile and run

2. **Replit**: https://replit.com/
   - Create C project
   - Upload files
   - Run

**Limitation:** CLI interaction may be limited in online compilers.

---

## Quick Start Script for Windows (Batch File)

Create `build.bat` in the project directory:

```batch
@echo off
echo Building Cloud Job Scheduler Simulator...

REM For MinGW
gcc -Wall -Wextra -std=c11 -g -o scheduler.exe main.c priority_queue.c hash_table.c node_list.c job_list.c scheduler.c persistence.c

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo.
    echo Running scheduler...
    scheduler.exe
) else (
    echo Build failed!
    pause
)
```

Run with: `build.bat`

---

## Troubleshooting

### Issue: "make: command not found"
**Solution:** 
- Use WSL (Option 1), or
- Install make via MSYS2, or
- Use the batch file (Option 5)

### Issue: "gcc: command not found"
**Solution:**
- Install MinGW-w64 or use WSL
- Make sure compiler is in PATH

### Issue: Path issues with backslashes
**Solution:**
- Use forward slashes `/` in paths
- Or escape backslashes `\\`
- Or use WSL which handles paths better

### Issue: Line ending differences (CRLF vs LF)
**Solution:**
- Git should handle this automatically
- Or configure Git: `git config core.autocrlf true`
- Or use a text editor that handles both

---

## Recommended Approach

**For easiest setup:** Use **WSL (Option 1)** - it requires minimal configuration and works exactly as intended.

**For native Windows:** Use **MinGW-w64 via MSYS2 (Option 2)** - provides a Linux-like environment on Windows.

**For IDE users:** Use **Code::Blocks (Option 4)** - simple GUI-based development.

---

## Testing on Windows

Once built, testing works the same way:

```bash
# In WSL or MSYS2
./quick_test.sh

# Or manually
./scheduler.exe
# Then enter commands interactively
```

All the same commands work:
- `add-node <cpu> <ram>`
- `add-job <priority> <cpu> <ram> <duration>`
- `run-tick`
- `status`
- `save <filename>`
- `load <filename>`
- `exit`


