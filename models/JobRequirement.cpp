#include "JobRequirement.h"
#include <algorithm>

JobRequirement::JobRequirement()
    : m_skillId(0), m_skillName(""), m_requiredLevel(1), m_importance("Medium") {}

JobRequirement::JobRequirement(int skillId, const std::string& skillName, int requiredLevel, const std::string& importance)
    : m_skillId(skillId), m_skillName(skillName),
      m_requiredLevel(std::clamp(requiredLevel, 1, 3)), m_importance(importance) {}

int JobRequirement::getSkillId() const {
    return m_skillId;
}

void JobRequirement::setSkillId(int skillId) {
    m_skillId = skillId;
}

std::string JobRequirement::getSkillName() const {
    return m_skillName;
}

void JobRequirement::setSkillName(const std::string& skillName) {
    m_skillName = skillName;
}

int JobRequirement::getRequiredLevel() const {
    return m_requiredLevel;
}

void JobRequirement::setRequiredLevel(int requiredLevel) {
    m_requiredLevel = std::clamp(requiredLevel, 1, 3);
}

std::string JobRequirement::getImportance() const {
    return m_importance;
}

void JobRequirement::setImportance(const std::string& importance) {
    m_importance = importance;
}

int JobRequirement::getImportanceWeight() const {
    if (m_importance == "High") return 3;
    if (m_importance == "Medium") return 2;
    return 1; // Low
}
