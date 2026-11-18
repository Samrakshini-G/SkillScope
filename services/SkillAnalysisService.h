#ifndef SKILLANALYSISSERVICE_H
#define SKILLANALYSISSERVICE_H

#include "../models/SkillGap.h"
#include "../models/JobRole.h"
#include "../models/JobRequirement.h"
#include "../database/SkillRepository.h"
#include "../database/CareerRepository.h"
#include <vector>
#include <map>
#include <string>

struct AnalysisResult {
    int jobRoleId;
    std::string jobRoleTitle;
    double matchPercentage;
    int totalRequiredSkills;
    int meetingCount;
    int insufficientCount;
    int missingCount;
    std::vector<SkillGap> allGaps;
    std::vector<SkillGap> missingSkills;
    std::vector<SkillGap> insufficientSkills;
    std::vector<SkillGap> masteredSkills;
};

/**
 * @brief Service responsible for evaluating career skill gaps using STL data structures
 *        and weighted readiness mathematical formulas.
 */
class SkillAnalysisService {
public:
    SkillAnalysisService();

    AnalysisResult analyzeGaps(int studentId, int jobRoleId, bool saveToHistory = true);

    AnalysisResult analyzeParsedSkills(int studentId, const std::vector<JobRequirement>& requirements,
                                       const std::string& customTitle = "Custom Job Description",
                                       bool saveToHistory = false);

    double calculateMatchPercentage(const std::vector<JobRequirement>& requirements,
                                    const std::map<int, int>& studentSkills);

private:
    AnalysisResult computeGaps(int studentId, int jobRoleId, const std::string& roleTitle,
                              const std::vector<JobRequirement>& requirements, bool saveToHistory);

    SkillRepository m_skillRepo;
    CareerRepository m_careerRepo;
};

#endif // SKILLANALYSISSERVICE_H
