#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../models/Student.h"
#include "../database/UserRepository.h"
#include <memory>
#include <string>

/**
 * @brief Service managing user authentication, registration, session state, and password hashing.
 */
class AuthService {
public:
    static AuthService& instance();

    bool login(const std::string& email, const std::string& plainPassword);
    bool registerStudent(const std::string& name, const std::string& email,
                         const std::string& plainPassword, const std::string& department,
                         const std::string& institution);
    void logout();

    bool isLoggedIn() const;
    Student* getCurrentStudent() const;
    int getCurrentStudentId() const;

    static std::string hashPassword(const std::string& plainPassword,
                                   const std::string& salt = "SkillScopeSalt_2026");

    std::string getLastError() const;

private:
    AuthService();
    ~AuthService() = default;

    UserRepository m_userRepo;
    std::unique_ptr<Student> m_currentUser;
    std::string m_lastError;
};

#endif // AUTHSERVICE_H
