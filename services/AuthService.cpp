#include "AuthService.h"
#include <QCryptographicHash>
#include <QString>
#include <QRegularExpression>

AuthService& AuthService::instance() {
    static AuthService s_instance;
    return s_instance;
}

AuthService::AuthService() : m_currentUser(nullptr) {}

std::string AuthService::hashPassword(const std::string& plainPassword, const std::string& salt) {
    QString combined = QString::fromStdString(salt + plainPassword);
    QByteArray hash = QCryptographicHash::hash(combined.toUtf8(), QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}

bool AuthService::login(const std::string& email, const std::string& plainPassword) {
    m_lastError.clear();

    if (email.empty() || plainPassword.empty()) {
        m_lastError = "Email and password cannot be empty.";
        return false;
    }

    std::string hashed = hashPassword(plainPassword);
    auto student = m_userRepo.authenticateUser(email, hashed);

    if (!student) {
        m_lastError = "Invalid email or password.";
        return false;
    }

    m_currentUser = std::move(student);
    return true;
}

bool AuthService::registerStudent(const std::string& name, const std::string& email,
                                  const std::string& plainPassword, const std::string& department,
                                  const std::string& institution) {
    m_lastError.clear();

    if (name.empty() || email.empty() || plainPassword.empty() || department.empty() || institution.empty()) {
        m_lastError = "All registration fields are required.";
        return false;
    }

    // Basic email validation
    static const QRegularExpression emailRegex("^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$");
    if (!emailRegex.match(QString::fromStdString(email)).hasMatch()) {
        m_lastError = "Please enter a valid email address.";
        return false;
    }

    if (plainPassword.length() < 6) {
        m_lastError = "Password must be at least 6 characters long.";
        return false;
    }

    if (m_userRepo.emailExists(email)) {
        m_lastError = "An account with this email already exists.";
        return false;
    }

    std::string hashed = hashPassword(plainPassword);
    int newUserId = 0;
    bool created = m_userRepo.createUser(name, email, hashed, department, institution, newUserId);

    if (!created) {
        m_lastError = "Failed to create account in database.";
        return false;
    }

    // Auto login
    m_currentUser = std::make_unique<Student>(newUserId, name, email, department, institution);
    return true;
}

void AuthService::logout() {
    m_currentUser.reset();
    m_lastError.clear();
}

bool AuthService::isLoggedIn() const {
    return m_currentUser != nullptr;
}

Student* AuthService::getCurrentStudent() const {
    return m_currentUser.get();
}

int AuthService::getCurrentStudentId() const {
    return m_currentUser ? m_currentUser->getId() : 0;
}

std::string AuthService::getLastError() const {
    return m_lastError;
}
