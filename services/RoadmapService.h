#ifndef ROADMAPSERVICE_H
#define ROADMAPSERVICE_H

#include "../models/SkillGap.h"
#include "../models/LearningTask.h"
#include "../database/LearningRepository.h"
#include <vector>
#include <queue>
#include <string>
#include <cmath>
#include <cstdint>

/**
 * @brief Functor comparator for ordering SkillGap items in an STL max-priority queue.
 *        Uses quantized integer priority components (micro-units: scale 10^6) to strictly satisfy
 *        the C++ strict weak ordering requirements (irreflexivity, asymmetry, transitivity,
 *        and transitivity of equivalence) without floating-point epsilon instability.
 *        Orders by normalized Priority Score = (0.5 * Gap) + (0.3 * Importance) + (0.2 * Urgency).
 *        Deterministic tie-breaking: Gap Score -> Importance Score -> Skill ID.
 */
struct SkillGapPriorityComparator {
    bool operator()(const SkillGap& a, const SkillGap& b) const {
        int64_t qScoreA = a.getQuantizedPriorityScore();
        int64_t qScoreB = b.getQuantizedPriorityScore();
        if (qScoreA != qScoreB) {
            return qScoreA < qScoreB; // Higher score comes first in max-heap
        }
        int64_t qGapA = a.getQuantizedGapScore();
        int64_t qGapB = b.getQuantizedGapScore();
        if (qGapA != qGapB) {
            return qGapA < qGapB;
        }
        int64_t qImpA = a.getQuantizedImportanceScore();
        int64_t qImpB = b.getQuantizedImportanceScore();
        if (qImpA != qImpB) {
            return qImpA < qImpB;
        }
        // Deterministic tie-breaker: lower ID comes first in max-heap
        return a.getSkillId() > b.getSkillId();
    }
};

/**
 * @brief Service generating structured learning roadmaps using STL-based Priority Management.
 */
class RoadmapService {
public:
    RoadmapService();

    // Priority queue type definition
    using SkillPriorityQueue = std::priority_queue<SkillGap, std::vector<SkillGap>, SkillGapPriorityComparator>;

    std::vector<LearningTask> generateRoadmap(int studentId, const std::vector<SkillGap>& gaps,
                                             const std::string& startDateStr = "2026-11-20");

    bool saveRoadmapTasks(int studentId, const std::vector<LearningTask>& tasks);

private:
    LearningRepository m_learningRepo;
};

#endif // ROADMAPSERVICE_H
