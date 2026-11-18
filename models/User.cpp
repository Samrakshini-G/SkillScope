#include "User.h"

User::User(int id, const std::string& name, const std::string& email)
    : m_id(id), m_name(name), m_email(email) {}

int User::getId() const {
    return m_id;
}

void User::setId(int id) {
    m_id = id;
}

std::string User::getName() const {
    return m_name;
}

void User::setName(const std::string& name) {
    m_name = name;
}

std::string User::getEmail() const {
    return m_email;
}

void User::setEmail(const std::string& email) {
    m_email = email;
}
