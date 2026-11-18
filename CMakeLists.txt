cmake_minimum_required(VERSION 3.16)

project(SkillScope VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.0 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Charts
)

# Common Core Sources (Models, Database, Services)
set(SKILLSCOPE_CORE_SOURCES
    # Database Layer
    database/DatabaseManager.h
    database/DatabaseManager.cpp
    database/UserRepository.h
    database/UserRepository.cpp
    database/SkillRepository.h
    database/SkillRepository.cpp
    database/CareerRepository.h
    database/CareerRepository.cpp
    database/LearningRepository.h
    database/LearningRepository.cpp

    # Model Layer
    models/User.h
    models/User.cpp
    models/Student.h
    models/Student.cpp
    models/Skill.h
    models/Skill.cpp
    models/JobRole.h
    models/JobRole.cpp
    models/JobRequirement.h
    models/JobRequirement.cpp
    models/SkillGap.h
    models/SkillGap.cpp
    models/LearningResource.h
    models/LearningResource.cpp
    models/LearningTask.h
    models/LearningTask.cpp

    # Service Layer
    services/AuthService.h
    services/AuthService.cpp
    services/SkillAnalysisService.h
    services/SkillAnalysisService.cpp
    services/JobDescriptionParser.h
    services/JobDescriptionParser.cpp
    services/RecommendationService.h
    services/RecommendationService.cpp
    services/RoadmapService.h
    services/RoadmapService.cpp
    services/ProgressService.h
    services/ProgressService.cpp
    services/EvaluationService.h
    services/EvaluationService.cpp
)

# Presentation Layer Sources
set(SKILLSCOPE_UI_SOURCES
    ui/Theme.h
    ui/components/MetricCard.h
    ui/components/MetricCard.cpp
    ui/components/LogoBadge.h
    ui/components/LogoBadge.cpp
    ui/components/CircularProgress.h
    ui/components/CircularProgress.cpp
    ui/components/StudentAvatar.h
    ui/components/StudentAvatar.cpp
    ui/components/ProficiencyBadge.h
    ui/components/ProficiencyBadge.cpp
    ui/components/EmptyStateWidget.h
    ui/components/EmptyStateWidget.cpp
    ui/LoginWindow.h
    ui/LoginWindow.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/DashboardPage.h
    ui/DashboardPage.cpp
    ui/ProfilePage.h
    ui/ProfilePage.cpp
    ui/SkillManagementPage.h
    ui/SkillManagementPage.cpp
    ui/CareerSelectionPage.h
    ui/CareerSelectionPage.cpp
    ui/SkillAnalysisPage.h
    ui/SkillAnalysisPage.cpp
    ui/RoadmapPage.h
    ui/RoadmapPage.cpp
    ui/ProgressPage.h
    ui/ProgressPage.cpp
    ui/EvaluationPage.h
    ui/EvaluationPage.cpp
)

# Main Application Target
add_executable(SkillScope
    main.cpp
    ${SKILLSCOPE_CORE_SOURCES}
    ${SKILLSCOPE_UI_SOURCES}
    resources/resources.qrc
)

target_include_directories(SkillScope PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

target_link_libraries(SkillScope PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Charts
)

# Comprehensive Unit Test Runner Target
add_executable(SkillScopeTests
    tests/test_runner.cpp
    ${SKILLSCOPE_CORE_SOURCES}
)

target_include_directories(SkillScopeTests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/services
)

target_link_libraries(SkillScopeTests PRIVATE
    Qt6::Core
    Qt6::Sql
)
