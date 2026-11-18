#include "SkillAnalysisService.h"
#include <algorithm>
#include <cmath>

SkillAnalysisService::SkillAnalysisService() {}

double SkillAnalysisService::calculateMatchPercentage(const std::vector<JobRequirement>& requirements,
                                                      const std::map<int, int>& studentSkills) {
    if (requirements.empty()) {
        return 100.0;
    }

    double sumWeightedAchieved = 0.0;
    double sumWeightedRequired = 0.0;

    for (const auto& req : requirements) {
        int weight = req.getImportanceWeight();
        if (weight <= 0) weight = 1;

        int reqLevel = req.getRequiredLevel();
        if (reqLevel <= 0) reqLevel = 1;

        auto it = studentSkills.find(req.getSkillId());
        int studentLevel = (it != studentSkills.end()) ? it->second : 0;

        int achieved = std::min(studentLevel, reqLevel);

        sumWeightedAchieved += static_cast<double>(weight * achieved);
        sumWeightedRequired += static_cast<double>(weight * reqLevel);
    }

    if (sumWeightedRequired <= 0.0) {
        return 0.0;
    }

    // Formula: [sum(Weight x min(Current, Required)) / sum(Weight x Required)] x 100
    double readiness = (sumWeightedAchieved / sumWeightedRequired) * 100.0;
    return std::round(readiness * 10.0) / 10.0;
}

AnalysisResult SkillAnalysisService::computeGaps(int studentId, int jobRoleId, const std::string& roleTitle,
                                                 const std::vector<JobRequirement>& requirements,
                                                 bool saveToHistory) {
    AnalysisResult result;
    result.jobRoleId = jobRoleId;
    result.jobRoleTitle = roleTitle;
    result.totalRequiredSkills = static_cast<int>(requirements.size());
    result.meetingCount = 0;
    result.insufficientCount = 0;
    result.missingCount = 0;

    std::map<int, int> studentSkills = m_skillRepo.getStudentSkillLevels(studentId);

    // Calculate overall career readiness using the weighted formula
    result.matchPercentage = calculateMatchPercentage(requirements, studentSkills);

    for (const auto& req : requirements) {
        auto it = studentSkills.find(req.getSkillId());
        // If student does not have the skill, treat current level as 0
        int currentLevel = (it != studentSkills.end()) ? it->second : 0;

        SkillGap gap(
            req.getSkillId(),
            req.getSkillName(),
            currentLevel,
            req.getRequiredLevel(),
            req.getImportance()
        );
        result.allGaps.push_back(gap);

        if (gap.isMissing()) {
            result.missingCount++;
        } else if (gap.isInsufficient()) {
            result.insufficientCount++;
        } else {
            result.meetingCount++;
        }
    }

    // STL copy_if to partition into sub-lists
    std::copy_if(result.allGaps.begin(), result.allGaps.end(),
                 std::back_inserter(result.missingSkills),
                 [](const SkillGap& g) { return g.isMissing(); });

    std::copy_if(result.allGaps.begin(), result.allGaps.end(),
                 std::back_inserter(result.insufficientSkills),
                 [](const SkillGap& g) { return g.isInsufficient(); });

    std::copy_if(result.allGaps.begin(), result.allGaps.end(),
                 std::back_inserter(result.masteredSkills),
                 [](const SkillGap& g) { return g.isMastered(); });

    // Sort all gaps by priority score descending using STL std::sort
    std::sort(result.allGaps.begin(), result.allGaps.end(),
              [](const SkillGap& a, const SkillGap& b) {
                  return a.getPriorityScore() > b.getPriorityScore();
              });

    if (saveToHistory && studentId > 0 && jobRoleId > 0) {
        m_careerRepo.saveAnalysisHistory(studentId, jobRoleId, result.matchPercentage, result.allGaps);
    }

    return result;
}

AnalysisResult SkillAnalysisService::analyzeGaps(int studentId, int jobRoleId, bool saveToHistory) {
    std::string roleTitle = "Unknown Role";
    auto roleOpt = m_careerRepo.getJobRoleById(jobRoleId);
    if (roleOpt.has_value()) {
        roleTitle = roleOpt->getTitle();
    }

    std::vector<JobRequirement> requirements = m_careerRepo.getRequirementsForRole(jobRoleId);
    return computeGaps(studentId, jobRoleId, roleTitle, requirements, saveToHistory);
}

AnalysisResult SkillAnalysisService::analyzeParsedSkills(int studentId,
                                                         const std::vector<JobRequirement>& requirements,
                                                         const std::string& customTitle,
                                                         bool saveToHistory) {
    return computeGaps(studentId, 0, customTitle, requirements, saveToHistory);
}
