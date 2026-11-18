#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <iostream>

#include "database/DatabaseManager.h"
#include "services/AuthService.h"
#include "ui/LoginWindow.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SkillScope");
    app.setOrganizationName("SkillScopeAcademic");

    // Initialize SQLite Database
    if (!DatabaseManager::instance().openDatabase()) {
        std::cerr << "Fatal: Failed to initialize SkillScope SQLite database: "
                  << DatabaseManager::instance().lastError().toStdString() << std::endl;
        return 1;
    }

    std::cout << "[SkillScope] Database initialized successfully at: "
              << DatabaseManager::instance().getDatabasePath().toStdString() << std::endl;

    // Load Application Stylesheet
    QFile styleFile(":/styles/theme.qss");
    if (!styleFile.open(QFile::ReadOnly)) {
        // Fallback to local relative file if resource wasn't loaded
        styleFile.setFileName("styles/theme.qss");
        (void)styleFile.open(QFile::ReadOnly);
    }
    if (styleFile.isOpen()) {
        QString style = QLatin1String(styleFile.readAll());
        app.setStyleSheet(style);
        styleFile.close();
    }

    // Check for command line flags
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--test" || arg == "--verify") {
            std::cout << "[SkillScope] Running self-verification check..." << std::endl;
            bool ok = AuthService::instance().login("student@skillscope.edu", "student123");
            if (ok) {
                std::cout << "[SkillScope] Demo user authentication verified successfully!" << std::endl;
                std::cout << "[SkillScope] Student: " << AuthService::instance().getCurrentStudent()->getName() << std::endl;
                return 0;
            } else {
                std::cerr << "[SkillScope] Verification failed: " << AuthService::instance().getLastError() << std::endl;
                return 1;
            }
        } else if (arg == "--screenshots") {
            std::cout << "[SkillScope] Capturing screenshots of redesigned UI..." << std::endl;
            QDir().mkpath("screenshots");

            // 1. Capture Login Window
            LoginWindow loginWin;
            loginWin.show();
            app.processEvents();
            loginWin.grab().save("screenshots/01_login_window.png");
            std::cout << "[SkillScope] Captured: screenshots/01_login_window.png" << std::endl;
            loginWin.hide();

            // 2. Authenticate demo user and capture all pages of MainWindow
            bool ok = AuthService::instance().login("student@skillscope.edu", "student123");
            if (ok) {
                MainWindow mainWin;
                mainWin.updateUserData();
                mainWin.show();
                app.processEvents();

                const char* pageNames[] = {
                    "02_dashboard_page.png",
                    "03_profile_page.png",
                    "04_skills_page.png",
                    "05_career_selection_page.png",
                    "06_career_analysis_page.png",
                    "07_learning_roadmap_page.png",
                    "08_progress_tracking_page.png",
                    "09_academic_evaluation_page.png"
                };

                for (int p = 0; p < 8; ++p) {
                    mainWin.navigateToPage(p);
                    app.processEvents();
                    mainWin.grab().save(QString("screenshots/%1").arg(pageNames[p]));
                    std::cout << "[SkillScope] Captured: screenshots/" << pageNames[p] << std::endl;
                }
                std::cout << "[SkillScope] All 9 UI screenshots captured successfully in screenshots/!" << std::endl;
                return 0;
            } else {
                std::cerr << "[SkillScope] Screenshot login failed: " << AuthService::instance().getLastError() << std::endl;
                return 1;
            }
        }
    }

    LoginWindow loginWin;
    loginWin.show();

    return app.exec();
}
