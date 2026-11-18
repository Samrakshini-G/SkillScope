#include "Skill.h"

Skill::Skill() : m_id(0), m_name(""), m_category("") {}

Skill::Skill(int id, const std::string& name, const std::string& category)
    : m_id(id), m_name(name), m_category(category) {}

int Skill::getId() const {
    return m_id;
}

void Skill::setId(int id) {
    m_id = id;
}

std::string Skill::getName() const {
    return m_name;
}

void Skill::setName(const std::string& name) {
    m_name = name;
}

std::string Skill::getCategory() const {
    return m_category;
}

void Skill::setCategory(const std::string& category) {
    m_category = category;
}
