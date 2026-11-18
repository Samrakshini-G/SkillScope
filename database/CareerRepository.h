#ifndef CAREERREPOSITORY_H
#define CAREERREPOSITORY_H

#include "../models/JobRole.h"
#include "../models/JobRequirement.h"
#include "../models/SkillGap.h"
#include <vector>
#include <optional>
#include <utility>

/**
 * @brief Repository for career roles, job requirements, and skill gap analysis history.
 */
class CareerRepository {
public:
    CareerRepository() = default;

    std::vector<JobRole> getAllJobRoles();
    std::vector<JobRole> searchJobRoles(const std::string& query);
    std::optional<JobRole> getJobRoleById(int id);
    std::vector<JobRequirement> getRequirementsForRole(int jobRoleId);

    bool saveAnalysisHistory(int studentId, int jobRoleId, double matchPercentage,
                             const std::vector<SkillGap>& gaps);

    struct AnalysisSummary {
        int id;
        int roleId;
        std::string roleTitle;
        double matchPercentage;
        std::string date;
    };

    std::vector<AnalysisSummary> getAnalysisHistoryForStudent(int studentId);
};

#endif // CAREERREPOSITORY_H
