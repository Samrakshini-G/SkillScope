#include "RecommendationService.h"
#include "SkillAnalysisService.h"
#include <unordered_set>

RecommendationService::RecommendationService() {}

std::vector<std::unique_ptr<LearningResource>> RecommendationService::getRecommendationsForGaps(const std::vector<SkillGap>& gaps) {
    std::vector<std::unique_ptr<LearningResource>> recommendations;
    std::unordered_set<int> seenSkills;

    for (const auto& gap : gaps) {
        // Only recommend for actual gaps (missing or insufficient)
        if (gap.hasGap() && seenSkills.find(gap.getSkillId()) == seenSkills.end()) {
            seenSkills.insert(gap.getSkillId());
            auto resources = m_learningRepo.getResourcesForSkill(gap.getSkillId());
            for (auto& res : resources) {
                recommendations.push_back(std::move(res));
            }
        }
    }

    return recommendations;
}

std::vector<std::unique_ptr<LearningResource>> RecommendationService::getRecommendationsForStudent(int studentId, int roleId) {
    SkillAnalysisService analysisService;
    auto analysisResult = analysisService.analyzeGaps(studentId, roleId, false);
    return getRecommendationsForGaps(analysisResult.allGaps);
}

std::vector<std::unique_ptr<LearningResource>> RecommendationService::getResourcesForSkill(int skillId) {
    return m_learningRepo.getResourcesForSkill(skillId);
}

std::vector<std::unique_ptr<LearningResource>> RecommendationService::getResourcesByType(const std::string& type) {
    return m_learningRepo.getResourcesByType(type);
}

std::vector<std::unique_ptr<LearningResource>> RecommendationService::filterResources(int skillId, const std::string& type) {
    return m_learningRepo.filterResources(skillId, type);
}
