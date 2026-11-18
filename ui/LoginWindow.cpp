#include "LoginWindow.h"
#include "MainWindow.h"
#include "../services/AuthService.h"
#include "Theme.h"
#include "components/LogoBadge.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QScreen>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget(parent),
      m_loginEmailEdit(nullptr),
      m_loginPasswordEdit(nullptr),
      m_loginTogglePassBtn(nullptr),
      m_loginStatusLabel(nullptr),
      m_regNameEdit(nullptr),
      m_regEmailEdit(nullptr),
      m_regPasswordEdit(nullptr),
      m_regTogglePassBtn(nullptr),
      m_regDeptEdit(nullptr),
      m_regInstEdit(nullptr),
      m_regStatusLabel(nullptr),
      m_tabWidget(nullptr),
      m_mainWindow(nullptr) {
    setupUi();
    setWindowTitle("SkillScope — Student Authentication");
    setFixedSize(520, 680);

    // Center on screen
    if (auto screen = QApplication::primaryScreen()) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

LoginWindow::~LoginWindow() {
    if (m_mainWindow) {
        delete m_mainWindow;
    }
}

void LoginWindow::setupUi() {
    setStyleSheet("background-color: #F5F6FC;");

    QVBoxLayout* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(32, 28, 32, 28);
    rootLayout->setSpacing(16);

    // Central Card
    QFrame* mainCard = new QFrame(this);
    mainCard->setObjectName("loginMainCard");
    mainCard->setStyleSheet(
        "#loginMainCard {"
        "  background-color: #FFFFFF;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 16px;"
        "}"
        "QLabel { border: none; background: transparent; padding: 0px; margin: 0px; }"
    );
    Theme::applyCardShadow(mainCard, 18, 5, 22);

    QVBoxLayout* cardLayout = new QVBoxLayout(mainCard);
    cardLayout->setContentsMargins(28, 26, 28, 28);
    cardLayout->setSpacing(16);

    // Brand Header with Vector LogoBadge
    QVBoxLayout* brandLayout = new QVBoxLayout();
    brandLayout->setSpacing(6);
    brandLayout->setAlignment(Qt::AlignCenter);

    LogoBadge* logoBadge = new LogoBadge(54, mainCard);
    brandLayout->addWidget(logoBadge, 0, Qt::AlignCenter);

    QLabel* logoLabel = new QLabel("SkillScope", mainCard);
    logoLabel->setStyleSheet("font-size: 24px; font-weight: 800; color: #202B45; border: none; background: transparent;");
    logoLabel->setAlignment(Qt::AlignCenter);

    QLabel* tagLabel = new QLabel("AI-Powered Skill Gap Detection & Career Development", mainCard);
    tagLabel->setStyleSheet("font-size: 12px; color: #7A849B; font-weight: 500; border: none; background: transparent;");
    tagLabel->setAlignment(Qt::AlignCenter);

    brandLayout->addWidget(logoLabel);
    brandLayout->addWidget(tagLabel);
    cardLayout->addLayout(brandLayout);

    // Tab Widget
    m_tabWidget = new QTabWidget(mainCard);
    m_tabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 12px;"
        "  background-color: #FFFFFF;"
        "  padding: 18px;"
        "}"
        "QTabBar::tab {"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "  padding: 9px 26px;"
        "  border-top-left-radius: 8px;"
        "  border-top-right-radius: 8px;"
        "  background-color: #F5F6FC;"
        "  color: #7A849B;"
        "  margin-right: 4px;"
        "  border: 1px solid transparent;"
        "}"
        "QTabBar::tab:selected {"
        "  background-color: #FFFFFF;"
        "  color: #4169E1;"
        "  border: 1px solid #E8EAF2;"
        "  border-bottom: 2px solid #FFFFFF;"
        "}"
    );

    // ==========================================
    // Tab 1: Sign In
    // ==========================================
    QWidget* loginTab = new QWidget();
    QVBoxLayout* loginLayout = new QVBoxLayout(loginTab);
    loginLayout->setSpacing(14);
    loginLayout->setContentsMargins(6, 12, 6, 6);

    QVBoxLayout* formCol = new QVBoxLayout();
    formCol->setSpacing(10);

    QLabel* emailLabel = new QLabel("Student Email", loginTab);
    emailLabel->setStyleSheet("font-size: 12px; font-weight: 700; color: #202B45; border: none;");
    m_loginEmailEdit = new QLineEdit(loginTab);
    m_loginEmailEdit->setPlaceholderText("e.g. student@skillscope.edu");
    m_loginEmailEdit->setText("student@skillscope.edu");
    m_loginEmailEdit->setMinimumHeight(38);

    formCol->addWidget(emailLabel);
    formCol->addWidget(m_loginEmailEdit);

    QLabel* passLabel = new QLabel("Password", loginTab);
    passLabel->setStyleSheet("font-size: 12px; font-weight: 700; color: #202B45; border: none;");

    QHBoxLayout* passRow = new QHBoxLayout();
    passRow->setSpacing(6);

    m_loginPasswordEdit = new QLineEdit(loginTab);
    m_loginPasswordEdit->setPlaceholderText("Enter your password");
    m_loginPasswordEdit->setEchoMode(QLineEdit::Password);
    m_loginPasswordEdit->setText("student123");
    m_loginPasswordEdit->setMinimumHeight(38);

    m_loginTogglePassBtn = new QPushButton("👁", loginTab);
    m_loginTogglePassBtn->setToolTip("Show / Hide password");
    m_loginTogglePassBtn->setFixedSize(38, 38);
    m_loginTogglePassBtn->setCursor(Qt::PointingHandCursor);
    m_loginTogglePassBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #F8FAFC;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #EEF2FF; border-color: #4169E1; }"
    );
    connect(m_loginTogglePassBtn, &QPushButton::clicked, this, &LoginWindow::toggleLoginPasswordVisibility);

    passRow->addWidget(m_loginPasswordEdit, 1);
    passRow->addWidget(m_loginTogglePassBtn);

    formCol->addWidget(passLabel);
    formCol->addLayout(passRow);
    loginLayout->addLayout(formCol);

    loginLayout->addSpacing(6);

    QPushButton* loginBtn = new QPushButton("Sign In to SkillScope", loginTab);
    loginBtn->setStyleSheet(Theme::primaryButtonStyle());
    loginBtn->setMinimumHeight(40);
    loginBtn->setCursor(Qt::PointingHandCursor);
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    loginLayout->addWidget(loginBtn);

    QPushButton* demoBtn = new QPushButton("⚡  1-Click Demo Account Login", loginTab);
    demoBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #F8FAFC;"
        "  color: #4169E1;"
        "  font-weight: 700;"
        "  padding: 9px;"
        "  border-radius: 9px;"
        "  border: 1px solid #D7E1FD;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #EEF2FF;"
        "}"
    );
    demoBtn->setMinimumHeight(38);
    demoBtn->setCursor(Qt::PointingHandCursor);
    connect(demoBtn, &QPushButton::clicked, this, &LoginWindow::onDemoLoginClicked);
    loginLayout->addWidget(demoBtn);

    m_loginStatusLabel = new QLabel("", loginTab);
    m_loginStatusLabel->setAlignment(Qt::AlignCenter);
    m_loginStatusLabel->setStyleSheet("font-size: 12px; font-weight: 700; border: none;");
    loginLayout->addWidget(m_loginStatusLabel);

    connect(m_loginEmailEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);
    connect(m_loginPasswordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onLoginClicked);

    loginLayout->addStretch();
    m_tabWidget->addTab(loginTab, "Sign In");

    // ==========================================
    // Tab 2: Register Account
    // ==========================================
    QWidget* regTab = new QWidget();
    QVBoxLayout* regLayout = new QVBoxLayout(regTab);
    regLayout->setSpacing(10);
    regLayout->setContentsMargins(6, 10, 6, 6);

    QFormLayout* regForm = new QFormLayout();
    regForm->setSpacing(8);
    regForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_regNameEdit = new QLineEdit(regTab);
    m_regNameEdit->setPlaceholderText("Full student name");
    m_regNameEdit->setMinimumHeight(34);

    m_regEmailEdit = new QLineEdit(regTab);
    m_regEmailEdit->setPlaceholderText("student@institution.edu");
    m_regEmailEdit->setMinimumHeight(34);

    m_regPasswordEdit = new QLineEdit(regTab);
    m_regPasswordEdit->setPlaceholderText("Minimum 6 characters");
    m_regPasswordEdit->setEchoMode(QLineEdit::Password);
    m_regPasswordEdit->setMinimumHeight(34);

    m_regTogglePassBtn = new QPushButton("👁", regTab);
    m_regTogglePassBtn->setToolTip("Show / Hide password");
    m_regTogglePassBtn->setFixedSize(34, 34);
    m_regTogglePassBtn->setCursor(Qt::PointingHandCursor);
    m_regTogglePassBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #F8FAFC;"
        "  border: 1px solid #E8EAF2;"
        "  border-radius: 8px;"
        "}"
        "QPushButton:hover { background-color: #EEF2FF; border-color: #4169E1; }"
    );
    connect(m_regTogglePassBtn, &QPushButton::clicked, this, &LoginWindow::toggleRegPasswordVisibility);

    QHBoxLayout* regPassRow = new QHBoxLayout();
    regPassRow->setSpacing(4);
    regPassRow->addWidget(m_regPasswordEdit, 1);
    regPassRow->addWidget(m_regTogglePassBtn);

    m_regDeptEdit = new QLineEdit(regTab);
    m_regDeptEdit->setPlaceholderText("e.g. Computer Science");
    m_regDeptEdit->setMinimumHeight(34);

    m_regInstEdit = new QLineEdit(regTab);
    m_regInstEdit->setPlaceholderText("e.g. State University");
    m_regInstEdit->setMinimumHeight(34);

    regForm->addRow(new QLabel("Full Name:"), m_regNameEdit);
    regForm->addRow(new QLabel("Email:"), m_regEmailEdit);
    regForm->addRow(new QLabel("Password:"), regPassRow);
    regForm->addRow(new QLabel("Department:"), m_regDeptEdit);
    regForm->addRow(new QLabel("Institution:"), m_regInstEdit);
    regLayout->addLayout(regForm);

    QPushButton* registerBtn = new QPushButton("Register Student Account", regTab);
    registerBtn->setStyleSheet(Theme::successButtonStyle());
    registerBtn->setMinimumHeight(38);
    registerBtn->setCursor(Qt::PointingHandCursor);
    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    regLayout->addWidget(registerBtn);

    m_regStatusLabel = new QLabel("", regTab);
    m_regStatusLabel->setAlignment(Qt::AlignCenter);
    m_regStatusLabel->setStyleSheet("font-size: 12px; font-weight: 700; border: none;");
    regLayout->addWidget(m_regStatusLabel);

    regLayout->addStretch();
    m_tabWidget->addTab(regTab, "Create Account");

    cardLayout->addWidget(m_tabWidget);
    rootLayout->addWidget(mainCard);
}

void LoginWindow::toggleLoginPasswordVisibility() {
    if (m_loginPasswordEdit->echoMode() == QLineEdit::Password) {
        m_loginPasswordEdit->setEchoMode(QLineEdit::Normal);
        m_loginTogglePassBtn->setText("🔒");
    } else {
        m_loginPasswordEdit->setEchoMode(QLineEdit::Password);
        m_loginTogglePassBtn->setText("👁");
    }
}

void LoginWindow::toggleRegPasswordVisibility() {
    if (m_regPasswordEdit->echoMode() == QLineEdit::Password) {
        m_regPasswordEdit->setEchoMode(QLineEdit::Normal);
        m_regTogglePassBtn->setText("🔒");
    } else {
        m_regPasswordEdit->setEchoMode(QLineEdit::Password);
        m_regTogglePassBtn->setText("👁");
    }
}

void LoginWindow::launchMainWindow() {
    if (!m_mainWindow) {
        m_mainWindow = new MainWindow();
        m_mainWindow->setLoginWindow(this);
    }
    m_mainWindow->updateUserData();
    m_mainWindow->show();
    this->hide();
}

void LoginWindow::onLoginClicked() {
    std::string email = m_loginEmailEdit->text().trimmed().toStdString();
    std::string password = m_loginPasswordEdit->text().trimmed().toStdString();

    if (AuthService::instance().login(email, password)) {
        m_loginStatusLabel->setText("Login successful! Loading dashboard...");
        m_loginStatusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
        launchMainWindow();
    } else {
        m_loginStatusLabel->setText(QString::fromStdString(AuthService::instance().getLastError()));
        m_loginStatusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}

void LoginWindow::onDemoLoginClicked() {
    m_loginEmailEdit->setText("student@skillscope.edu");
    m_loginPasswordEdit->setText("student123");
    onLoginClicked();
}

void LoginWindow::onRegisterClicked() {
    std::string name = m_regNameEdit->text().trimmed().toStdString();
    std::string email = m_regEmailEdit->text().trimmed().toStdString();
    std::string password = m_regPasswordEdit->text().trimmed().toStdString();
    std::string dept = m_regDeptEdit->text().trimmed().toStdString();
    std::string inst = m_regInstEdit->text().trimmed().toStdString();

    if (AuthService::instance().registerStudent(name, email, password, dept, inst)) {
        m_regStatusLabel->setText("Account created successfully!");
        m_regStatusLabel->setStyleSheet("color: #20B486; font-weight: 700; border: none;");
        launchMainWindow();
    } else {
        m_regStatusLabel->setText(QString::fromStdString(AuthService::instance().getLastError()));
        m_regStatusLabel->setStyleSheet("color: #E56B75; font-weight: 700; border: none;");
    }
}
