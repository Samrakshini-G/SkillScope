#include "SkillGap.h"
#include <algorithm>
#include <cmath>

SkillGap::SkillGap()
    : m_skillId(0), m_skillName(""), m_currentLevel(0), m_requiredLevel(1),
      m_importance("Medium"), m_urgencyScore(0.5) {}

SkillGap::SkillGap(int skillId, const std::string& skillName, int currentLevel,
                   int requiredLevel, const std::string& importance)
    : m_skillId(skillId), m_skillName(skillName),
      m_currentLevel(std::clamp(currentLevel, 0, 3)),
      m_requiredLevel(std::clamp(requiredLevel, 1, 3)),
      m_importance(importance),
      m_urgencyScore(0.5) {}

int SkillGap::getSkillId() const {
    return m_skillId;
}

void SkillGap::setSkillId(int skillId) {
    m_skillId = skillId;
}

std::string SkillGap::getSkillName() const {
    return m_skillName;
}

void SkillGap::setSkillName(const std::string& skillName) {
    m_skillName = skillName;
}

int SkillGap::getCurrentLevel() const {
    return m_currentLevel;
}

void SkillGap::setCurrentLevel(int currentLevel) {
    m_currentLevel = std::clamp(currentLevel, 0, 3);
}

int SkillGap::getRequiredLevel() const {
    return m_requiredLevel;
}

void SkillGap::setRequiredLevel(int requiredLevel) {
    m_requiredLevel = std::clamp(requiredLevel, 1, 3);
}

std::string SkillGap::getImportance() const {
    return m_importance;
}

void SkillGap::setImportance(const std::string& importance) {
    m_importance = importance;
}

int SkillGap::calculateGap() const {
    return std::max(0, m_requiredLevel - m_currentLevel);
}

bool SkillGap::hasGap() const {
    return calculateGap() > 0;
}

bool SkillGap::isMissing() const {
    return m_currentLevel == 0;
}

bool SkillGap::isInsufficient() const {
    return m_currentLevel > 0 && m_currentLevel < m_requiredLevel;
}

bool SkillGap::isMastered() const {
    return m_currentLevel >= m_requiredLevel;
}

std::string SkillGap::getStatusText() const {
    if (isMissing()) {
        return "Missing";
    }
    if (isInsufficient()) {
        return "Skill gap";
    }
    return "Meets requirement";
}

int SkillGap::getImportanceWeight() const {
    if (m_importance == "High") return 3;
    if (m_importance == "Medium") return 2;
    return 1;
}

double SkillGap::getNormalizedGapScore() const {
    // Gap ranges from 0 to 3. Normalized: clamp(gap / 3.0, 0.0, 1.0)
    return std::clamp(static_cast<double>(calculateGap()) / 3.0, 0.0, 1.0);
}

double SkillGap::getNormalizedImportanceScore() const {
    // Importance weight ranges from 1 to 3. Normalized: clamp(weight / 3.0, 0.0, 1.0)
    return std::clamp(static_cast<double>(getImportanceWeight()) / 3.0, 0.0, 1.0);
}

double SkillGap::getNormalizedUrgencyScore() const {
    // Neutral documented default is 0.5 (on a 0.0 to 1.0 scale)
    return std::clamp(m_urgencyScore, 0.0, 1.0);
}

void SkillGap::setUrgencyScore(double urgency) {
    m_urgencyScore = std::clamp(urgency, 0.0, 1.0);
}

double SkillGap::getPriorityScore() const {
    // Required Formula: (0.5 * Gap Score) + (0.3 * Importance Score) + (0.2 * Urgency Score)
    // All inputs are normalized to 0.0 - 1.0. Final score is clamped strictly between 0.0 and 1.0.
    double gapScore = getNormalizedGapScore();
    double impScore = getNormalizedImportanceScore();
    double urgScore = getNormalizedUrgencyScore();
    double score = (0.5 * gapScore) + (0.3 * impScore) + (0.2 * urgScore);
    return std::clamp(score, 0.0, 1.0);
}

int64_t SkillGap::getQuantizedPriorityScore(int64_t scale) const {
    return std::llround(getPriorityScore() * static_cast<double>(scale));
}

int64_t SkillGap::getQuantizedGapScore(int64_t scale) const {
    return std::llround(getNormalizedGapScore() * static_cast<double>(scale));
}

int64_t SkillGap::getQuantizedImportanceScore(int64_t scale) const {
    return std::llround(getNormalizedImportanceScore() * static_cast<double>(scale));
}
