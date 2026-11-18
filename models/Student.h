#ifndef STUDENT_H
#define STUDENT_H

#include "User.h"
#include <string>

/**
 * @brief Represents a student user with department and institution information.
 */
class Student : public User {
public:
    Student(int id, const std::string& name, const std::string& email,
            const std::string& department, const std::string& institution,
            const std::string& degree = "", int graduationYear = 2026, int targetRoleId = 0);
    ~Student() override = default;

    std::string getDepartment() const;
    void setDepartment(const std::string& department);

    std::string getInstitution() const;
    void setInstitution(const std::string& institution);

    std::string getDegree() const;
    void setDegree(const std::string& degree);

    int getGraduationYear() const;
    void setGraduationYear(int year);

    int getTargetRoleId() const;
    void setTargetRoleId(int roleId);

    // Overridden virtual methods
    std::string userType() const override;
    std::string dashboardMessage() const override;

private:
    std::string m_department;
    std::string m_institution;
    std::string m_degree;
    int m_graduationYear;
    int m_targetRoleId;
};

#endif // STUDENT_H
