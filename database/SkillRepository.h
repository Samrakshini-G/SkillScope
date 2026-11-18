#ifndef SKILLREPOSITORY_H
#define SKILLREPOSITORY_H

#include "../models/Skill.h"
#include <vector>
#include <map>
#include <optional>

/**
 * @brief Repository for skills and student-skill proficiency mappings.
 */
class SkillRepository {
public:
    SkillRepository() = default;

    std::vector<Skill> getAllSkills();
    std::vector<Skill> searchSkills(const std::string& query);
    std::optional<Skill> getSkillById(int id);
    std::optional<Skill> getSkillByName(const std::string& name);

    std::map<int, int> getStudentSkillLevels(int studentId);
    std::vector<Skill> getStudentSkills(int studentId);
    bool setStudentSkillLevel(int studentId, int skillId, int level);
    bool addStudentSkill(int studentId, int skillId, int level);
    bool hasStudentSkill(int studentId, int skillId);
    bool removeStudentSkill(int studentId, int skillId);
};

#endif // SKILLREPOSITORY_H
