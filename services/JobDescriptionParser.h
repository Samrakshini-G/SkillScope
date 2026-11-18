#ifndef JOBDESCRIPTIONPARSER_H
#define JOBDESCRIPTIONPARSER_H

#include "../models/Skill.h"
#include "../models/JobRole.h"
#include "../models/JobRequirement.h"
#include "../database/SkillRepository.h"
#include "../database/CareerRepository.h"
#include <string>
#include <vector>
#include <map>
#include <utility>

struct DetectedSkill {
    int skillId;              // 0 if unrecognized in database catalog
    std::string skillName;    // Canonical catalog name
    std::string matchedAlias; // Specific phrase/word that matched in input
    int suggestedProficiency; // 1 to 3 (default 2 if unspecified)
    std::string importance;   // "High", "Medium", "Low"
    bool isRecognized;
};

/**
 * @brief Keyword-based parser that scans free-text job descriptions for skills,
 *        applies regex/word-boundary alias matching, and estimates proficiency.
 */
class JobDescriptionParser {
public:
    JobDescriptionParser();

    // Alias dictionary management
    void addAlias(const std::string& canonicalSkillName, const std::string& alias);
    std::map<std::string, std::vector<std::string>> getAliasDictionary() const;

    // Parsing methods
    std::vector<DetectedSkill> parseJobDescription(const std::string& text);
    std::vector<Skill> extractSkillsFromText(const std::string& text);
    std::vector<std::string> extractUnrecognizedKeywords(const std::string& text);
    std::vector<JobRequirement> convertToRequirements(const std::vector<DetectedSkill>& detectedList) const;

    std::vector<std::pair<JobRole, int>> matchRolesFromText(const std::string& text);

private:
    void initializeDefaultAliases();
    int detectProficiencyLevel(const std::string& text, const std::string& matchedTerm) const;
    bool containsPhrase(const std::string& text, const std::string& phrase) const;

    SkillRepository m_skillRepo;
    CareerRepository m_careerRepo;
    std::map<std::string, std::vector<std::string>> m_aliases;
};

#endif // JOBDESCRIPTIONPARSER_H
