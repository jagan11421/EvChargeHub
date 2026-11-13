
cmake_minimum_required(VERSION 3.16)

project(EVChargeHub VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Sql
)

add_executable(EVChargeHub

    src/main.cpp

    # Models
    src/models/Station.h
    src/models/ChargingPoint.h
    src/models/Vehicle.h
    src/models/Customer.h
    src/models/Booking.h
    # Database
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp

    src/database/StationRepository.h
    src/database/StationRepository.cpp

    src/database/ChargingPointRepository.h
    src/database/ChargingPointRepository.cpp

    src/database/VehicleRepository.cpp
    src/database/VehicleRepository.h

    src/database/CustomerRepository.h
    src/database/CustomerRepository.cpp

    src/database/BookingRepository.h
    src/database/BookingRepository.cpp

    # Main window
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp

    # Pages
    src/ui/pages/DashboardPage.h
    src/ui/pages/DashboardPage.cpp

    src/ui/pages/PlaceholderPage.h
    src/ui/pages/PlaceholderPage.cpp

    src/ui/pages/StationsPage.h
    src/ui/pages/StationsPage.cpp

    src/ui/pages/ChargingPointsPage.h
    src/ui/pages/ChargingPointsPage.cpp

    src/ui/pages/VehiclesPage.h
    src/ui/pages/VehiclesPage.cpp

    src/ui/pages/CustomersPage.h
    src/ui/pages/CustomersPage.cpp
    
    src/ui/pages/BookingsPage.h
    src/ui/pages/BookingsPage.cpp
    

)

target_include_directories(EVChargeHub PRIVATE src)

target_link_libraries(EVChargeHub PRIVATE
    Qt6::Widgets
    Qt6::Sql
)
