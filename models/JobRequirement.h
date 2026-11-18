#ifndef JOBREQUIREMENT_H
#define JOBREQUIREMENT_H

#include <string>

/**
 * @brief Represents a skill requirement for a career job role.
 */
class JobRequirement {
public:
    JobRequirement();
    JobRequirement(int skillId, const std::string& skillName, int requiredLevel, const std::string& importance);

    int getSkillId() const;
    void setSkillId(int skillId);

    std::string getSkillName() const;
    void setSkillName(const std::string& skillName);

    int getRequiredLevel() const;
    void setRequiredLevel(int requiredLevel);

    std::string getImportance() const;
    void setImportance(const std::string& importance);

    int getImportanceWeight() const;

private:
    int m_skillId;
    std::string m_skillName;
    int m_requiredLevel;     // 1 to 3
    std::string m_importance; // "High", "Medium", "Low"
};

#endif // JOBREQUIREMENT_H
