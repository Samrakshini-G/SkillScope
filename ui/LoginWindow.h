#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

class MainWindow;

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow() override;

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onDemoLoginClicked();
    void toggleLoginPasswordVisibility();
    void toggleRegPasswordVisibility();

private:
    void setupUi();
    void launchMainWindow();

    // Login Form widgets
    QLineEdit* m_loginEmailEdit;
    QLineEdit* m_loginPasswordEdit;
    QPushButton* m_loginTogglePassBtn;
    QLabel* m_loginStatusLabel;

    // Register Form widgets
    QLineEdit* m_regNameEdit;
    QLineEdit* m_regEmailEdit;
    QLineEdit* m_regPasswordEdit;
    QPushButton* m_regTogglePassBtn;
    QLineEdit* m_regDeptEdit;
    QLineEdit* m_regInstEdit;
    QLabel* m_regStatusLabel;

    QTabWidget* m_tabWidget;
    MainWindow* m_mainWindow;
};

#endif // LOGINWINDOW_H
