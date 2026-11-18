#include "Student.h"

Student::Student(int id, const std::string& name, const std::string& email,
                 const std::string& department, const std::string& institution,
                 const std::string& degree, int graduationYear, int targetRoleId)
    : User(id, name, email), m_department(department), m_institution(institution),
      m_degree(degree), m_graduationYear(graduationYear), m_targetRoleId(targetRoleId) {}

std::string Student::getDepartment() const {
    return m_department;
}

void Student::setDepartment(const std::string& department) {
    m_department = department;
}

std::string Student::getInstitution() const {
    return m_institution;
}

void Student::setInstitution(const std::string& institution) {
    m_institution = institution;
}

std::string Student::getDegree() const {
    return m_degree;
}

void Student::setDegree(const std::string& degree) {
    m_degree = degree;
}

int Student::getGraduationYear() const {
    return m_graduationYear;
}

void Student::setGraduationYear(int year) {
    m_graduationYear = year;
}

int Student::getTargetRoleId() const {
    return m_targetRoleId;
}

void Student::setTargetRoleId(int roleId) {
    m_targetRoleId = roleId;
}

std::string Student::userType() const {
    return "Student";
}

std::string Student::dashboardMessage() const {
    return "Welcome back, " + m_name + "! Track your career goals and skill roadmap here.";
}
