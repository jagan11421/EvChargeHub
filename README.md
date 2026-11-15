# ⚡ EVChargeHub – EV Charging Station Management System

EVChargeHub is a desktop-based Electric Vehicle (EV) Charging Station Management System developed using **C++17, Qt 6, SQLite, and CMake**.

The application provides a centralized platform to manage EV charging stations, charging points, customers, vehicles, bookings, charging sessions, payments, and reports.

---

## ✨ Features

### 1. Dashboard
- View an overview of charging station operations.
- Monitor stations, charging points, customers, and vehicles.
- View booking and charging session statistics.
- Visualize operational data using charts.
- View recent bookings.

### 2. Station Management
- Add, update, and manage charging stations.
- Store station names, addresses, and power capacity.
- Manage station status.

### 3. Charging Point Management
- Add charging points to stations.
- Configure charger type and power capacity.
- Set charging prices per kWh.
- Monitor available and occupied charging points.

### 4. Customer Management
- Register customers.
- Store customer contact information.
- Update customer details.
- Manage customer account status.

### 5. Vehicle Management
- Register customer vehicles.
- Store registration numbers, brands, and models.
- Maintain battery capacity information.
- Configure connector types.
- Associate vehicles with customers.

### 6. Booking Management
- Create charging reservations.
- Select customers, vehicles, and charging points.
- Set booking start and end times.
- Check charging point availability.
- Prevent overlapping reservations.
- Manage booking statuses.

### 7. Charging Session Management
- Start charging sessions using bookings.
- Track energy consumption.
- Calculate charging costs automatically.
- Complete or cancel active sessions.
- Release charging points after sessions end.

### 8. Payment Management
- Manage charging-related payment records.
- Track payment information and transaction statuses.

### 9. Reports and Analytics
- View charging and operational reports.
- Analyze booking and charging activity.
- Monitor system performance.

---

## 🛠️ Technologies Used

| Technology | Purpose |
|---|---|
| C++17 | Core application development |
| Qt 6 Widgets | Desktop graphical interface |
| Qt SQL | Database connectivity |
| Qt Charts | Dashboard visualizations |
| SQLite | Local database storage |
| CMake | Build configuration |
| Git & GitHub | Version control |

---

## 📁 Project Structure

```text
EVChargeHub/
│
├── src/
│   ├── main.cpp
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   ├── database/
│   ├── models/
│   ├── repositories/
│   ├── services/
│   └── ui/
│       └── pages/
│
├── database/
│   └── schema.sql
│
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

# 🚀 Installation and Setup

Choose the instructions for your operating system.

## 🪟 Windows Installation

### Step 1: Install prerequisites

Install the following software:

| Software | Download |
|---|---|
| Git | https://git-scm.com/downloads |
| CMake | https://cmake.org/download/ |
| Visual Studio 2022 | https://visualstudio.microsoft.com/ |
| Qt 6 | https://www.qt.io/download |

**Visual Studio configuration:**

During installation, select the workload:

- Desktop development with C++

Make sure these components are installed:
- MSVC C++ compiler
- Windows SDK
- C++ CMake tools

**Qt configuration:**

Using the Qt Online Installer, install:
- Qt 6
- MSVC 2022 64-bit kit
- Qt Charts module
- Qt SQL module
- SQLite SQL driver plugin

Use a Qt kit compatible with your installed Visual Studio compiler.

### Step 2: Clone the repository

Open PowerShell or Command Prompt:

```powershell
git clone https://github.com/jagan11421/EvChargeHub.git

cd EVChargeHub
```
### Step 3: Configure the project

Open **Developer PowerShell for VS 2022**.

Run the following command:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
```

Replace the Qt path with your actual installation path.

For example:

```text
C:/Qt/6.8.3/msvc2022_64
```

### Step 4: Build the application

```powershell
cmake --build build --config Release
```

### Step 5: Run the application

```powershell
.\build\Release\EVChargeHub.exe
```

### Step 6: Deploy Qt dependencies

To run the application on another Windows computer, deploy the required Qt libraries and plugins.

Open the Qt MSVC environment and run:

```powershell
windeployqt .\build\Release\EVChargeHub.exe
```

This copies the required Qt runtime files and plugins alongside the executable.

The target computer may also need the Microsoft Visual C++ Redistributable.

---

## 🍎 macOS Installation

### Step 1: Install prerequisites

Install Homebrew if it is not already installed:

https://brew.sh/

Install the required tools:

```bash
brew install cmake
brew install qt
brew install qtcharts
```

Check installations:

```bash
cmake --version
brew --prefix qt
```

### Step 2: Clone the repository

```bash
git clone https://github.com/jagan11421/EvChargeHub.git

cd EVChargeHub
```

### Step 3: Configure the project

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### Step 4: Build the application

```bash
cmake --build build --parallel
```

### Step 5: Run the application

```bash
./build/EVChargeHub
```

---

## 🐧 Linux Installation (Ubuntu / Debian)

### Step 1: Install prerequisites

Open the terminal and run:

```bash
sudo apt update
```

Install the required packages:

```bash
sudo apt install -y \
    build-essential \
    cmake \
    git \
    qt6-base-dev \
    qt6-charts-dev \
    libqt6sql6-sqlite
```

### Step 2: Clone the repository

```bash
git clone https://github.com/jagan11421/EvChargeHub.git

cd EVChargeHub
```

### Step 3: Configure the project

```bash
cmake -S . -B build
```

### Step 4: Build the application

```bash
cmake --build build --parallel
```

### Step 5: Run the application

```bash
./build/EVChargeHub
```

---

# 🗄️ Database Information

EVChargeHub uses **SQLite** as its local relational database.

The database manages:

- Charging stations
- Charging points
- Customers
- Vehicles
- Bookings
- Charging sessions
- Payments

### Database Initialization

The application should initialize the database and create the required tables when it runs for the first time.

Each installation maintains its own local database.

The actual database file location depends on the path configured in the database initialization code.

**Important:** The repository should include the database schema or initialization code, rather than a personal database containing actual records.

---

# 🔄 Updating the Project

To download the latest changes from GitHub:

```bash
git pull origin main
```

Rebuild the application:

```bash
cmake --build build
```

Run the application again using the command for your operating system.

If the CMake configuration changes significantly, remove the build directory and configure the project again.

---

# 🧹 Clean Build

If you encounter errors caused by outdated generated build files, remove the build directory.

### macOS / Linux

```bash
rm -rf build

cmake -S . -B build
cmake --build build
```

On macOS, include the Qt prefix:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### Windows PowerShell

```powershell
Remove-Item -Recurse -Force build
```

Configure again:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2022_64"
```

Build:

```powershell
cmake --build build --config Release
```

---

# 🛠️ Troubleshooting

| Problem | Possible solution |
|---|---|
| CMake cannot find Qt | Verify `CMAKE_PREFIX_PATH` |
| Qt SQL driver not found | Install the Qt SQLite driver plugin |
| Qt Charts not found | Install the Qt Charts module |
| Compiler errors | Verify compiler and Qt kit compatibility |
| Application cannot find Qt DLLs on Windows | Run `windeployqt` |
| Old build configuration errors | Delete `build/` and configure again |

---

# 🔮 Future Improvements

- User authentication and role-based access.
- Real-time charging station monitoring.
- Payment gateway integration.
- Advanced analytics and reporting.
- Export reports to PDF and Excel.
- Remote charging station monitoring.

---

# 👨‍💻 Author

Developed as a C++ and Qt-based desktop application project.

---


