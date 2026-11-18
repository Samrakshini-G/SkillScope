#include "JobRole.h"

JobRole::JobRole() : m_id(0), m_title(""), m_description("") {}

JobRole::JobRole(int id, const std::string& title, const std::string& description)
    : m_id(id), m_title(title), m_description(description) {}

int JobRole::getId() const {
    return m_id;
}

void JobRole::setId(int id) {
    m_id = id;
}

std::string JobRole::getTitle() const {
    return m_title;
}

void JobRole::setTitle(const std::string& title) {
    m_title = title;
}

std::string JobRole::getDescription() const {
    return m_description;
}

void JobRole::setDescription(const std::string& description) {
    m_description = description;
}
