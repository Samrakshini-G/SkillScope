#ifndef RECOMMENDATIONSERVICE_H
#define RECOMMENDATIONSERVICE_H

#include "../models/SkillGap.h"
#include "../models/LearningResource.h"
#include "../database/LearningRepository.h"
#include <vector>
#include <memory>

/**
 * @brief Service providing personalized learning resource recommendations for identified skill gaps.
 */
class RecommendationService {
public:
    RecommendationService();

    std::vector<std::unique_ptr<LearningResource>> getRecommendationsForGaps(const std::vector<SkillGap>& gaps);
    std::vector<std::unique_ptr<LearningResource>> getRecommendationsForStudent(int studentId, int roleId);
    std::vector<std::unique_ptr<LearningResource>> getResourcesForSkill(int skillId);
    std::vector<std::unique_ptr<LearningResource>> getResourcesByType(const std::string& type);
    std::vector<std::unique_ptr<LearningResource>> filterResources(int skillId, const std::string& type);

private:
    LearningRepository m_learningRepo;
};

#endif // RECOMMENDATIONSERVICE_H
