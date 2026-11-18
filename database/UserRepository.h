#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "../models/Student.h"
#include <memory>
#include <string>
#include <optional>

/**
 * @brief Repository for user and student profile persistence operations.
 */
class UserRepository {
public:
    UserRepository() = default;

    bool createUser(const std::string& name, const std::string& email,
                    const std::string& passwordHash, const std::string& department,
                    const std::string& institution, int& outUserId);

    std::unique_ptr<Student> authenticateUser(const std::string& email, const std::string& passwordHash);

    std::unique_ptr<Student> getStudentById(int userId);

    bool updateStudentProfile(int userId, const std::string& name,
                              const std::string& department, const std::string& institution);

    bool updateStudentProfile(int userId, const std::string& name, const std::string& email,
                              const std::string& department, const std::string& institution,
                              const std::string& degree, int graduationYear);

    bool setTargetRole(int userId, int roleId);
    int getTargetRole(int userId);

    bool emailExists(const std::string& email);
    bool emailExistsForOtherUser(const std::string& email, int userId);

    std::string getPasswordHash(const std::string& email);
};

#endif // USERREPOSITORY_H
