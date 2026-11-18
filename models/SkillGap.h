#ifndef SKILLGAP_H
#define SKILLGAP_H

#include <string>
#include <cstdint>

/**
 * @brief Represents a detected gap between a student's skill level and a career requirement.
 */
class SkillGap {
public:
    SkillGap();
    SkillGap(int skillId, const std::string& skillName, int currentLevel,
             int requiredLevel, const std::string& importance);

    int getSkillId() const;
    void setSkillId(int skillId);

    std::string getSkillName() const;
    void setSkillName(const std::string& skillName);

    int getCurrentLevel() const;
    void setCurrentLevel(int currentLevel);

    int getRequiredLevel() const;
    void setRequiredLevel(int requiredLevel);

    std::string getImportance() const;
    void setImportance(const std::string& importance);

    // Domain methods
    int calculateGap() const;
    bool hasGap() const;
    bool isMissing() const;
    bool isInsufficient() const;
    bool isMastered() const;
    std::string getStatusText() const;

    int getImportanceWeight() const;
    double getNormalizedGapScore() const;
    double getNormalizedImportanceScore() const;
    double getNormalizedUrgencyScore() const;
    void setUrgencyScore(double urgency);
    double getPriorityScore() const;

    // Quantized integer priority components for deterministic strict weak ordering in STL containers
    int64_t getQuantizedPriorityScore(int64_t scale = 1000000LL) const;
    int64_t getQuantizedGapScore(int64_t scale = 1000000LL) const;
    int64_t getQuantizedImportanceScore(int64_t scale = 1000000LL) const;

private:
    int m_skillId;
    std::string m_skillName;
    int m_currentLevel;     // 0 to 3
    int m_requiredLevel;    // 1 to 3
    std::string m_importance; // "High", "Medium", "Low"
    double m_urgencyScore;  // 0.0 to 1.0 (neutral default 0.5)
};

#endif // SKILLGAP_H
