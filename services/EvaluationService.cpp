#include "EvaluationService.h"
#include "../models/SkillGap.h"
#include "../services/RoadmapService.h"
#include <chrono>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>

EvaluationService::EvaluationService() {}

// Plain C-style procedural structs
struct ProcRequirement {
    int skillId;
    int requiredLevel;
    int weight;
};

struct ProcStudentSkill {
    int skillId;
    int level;
};

struct ProcGap {
    int skillId;
    int currentLevel;
    int requiredLevel;
    int gap;
    int priority;
};

// Procedural C-style bubble sort on raw array
static void proceduralBubbleSort(std::vector<ProcGap>& gaps) {
    int n = static_cast<int>(gaps.size());
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (gaps[j].priority < gaps[j + 1].priority) {
                ProcGap temp = gaps[j];
                gaps[j] = gaps[j + 1];
                gaps[j + 1] = temp;
            }
        }
    }
}

void EvaluationService::runProceduralBenchmark(int iterations, long long& outDurationUs) {
    // Dataset of 12 skills
    std::vector<ProcRequirement> reqs = {
        {1, 3, 3}, {2, 3, 3}, {3, 3, 3}, {4, 2, 3},
        {5, 3, 3}, {6, 2, 2}, {7, 2, 3}, {8, 2, 2},
        {9, 3, 1}, {10, 3, 1}, {11, 2, 1}, {12, 2, 2}
    };

    std::vector<ProcStudentSkill> studentSkills = {
        {1, 1}, {2, 0}, {3, 2}, {4, 0},
        {5, 1}, {6, 2}, {7, 0}, {8, 1},
        {9, 0}, {10, 0}, {11, 1}, {12, 1}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int it = 0; it < iterations; ++it) {
        std::vector<ProcGap> gaps;
        gaps.reserve(reqs.size());

        // Procedural linear scan
        for (size_t i = 0; i < reqs.size(); ++i) {
            int currentLevel = 0;
            for (size_t j = 0; j < studentSkills.size(); ++j) {
                if (studentSkills[j].skillId == reqs[i].skillId) {
                    currentLevel = studentSkills[j].level;
                    break;
                }
            }

            int gap = reqs[i].requiredLevel - currentLevel;
            if (gap > 0) {
                int priority = gap * reqs[i].weight;
                gaps.push_back({reqs[i].skillId, currentLevel, reqs[i].requiredLevel, gap, priority});
            }
        }

        // Procedural sorting
        proceduralBubbleSort(gaps);
    }

    auto end = std::chrono::high_resolution_clock::now();
    outDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

void EvaluationService::runOopBenchmark(int iterations, long long& outDurationUs) {
    std::vector<JobRequirement> reqs = {
        JobRequirement(1, "C++", 3, "High"),
        JobRequirement(2, "OOP", 3, "High"),
        JobRequirement(3, "Data Structures", 3, "High"),
        JobRequirement(4, "Algorithms", 2, "High"),
        JobRequirement(5, "STL", 3, "High"),
        JobRequirement(6, "SQL", 2, "Medium"),
        JobRequirement(7, "Git", 2, "High"),
        JobRequirement(8, "Problem Solving", 2, "Medium"),
        JobRequirement(9, "HTML", 3, "Low"),
        JobRequirement(10, "CSS", 3, "Low"),
        JobRequirement(11, "JavaScript", 2, "Low"),
        JobRequirement(12, "Python", 2, "Medium")
    };

    std::map<int, int> studentSkillMap = {
        {1, 1}, {2, 0}, {3, 2}, {4, 0},
        {5, 1}, {6, 2}, {7, 0}, {8, 1},
        {9, 0}, {10, 0}, {11, 1}, {12, 1}
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (int it = 0; it < iterations; ++it) {
        std::vector<SkillGap> gaps;
        gaps.reserve(reqs.size());

        for (const auto& req : reqs) {
            auto sIt = studentSkillMap.find(req.getSkillId());
            int currentLevel = (sIt != studentSkillMap.end()) ? sIt->second : 0;
            SkillGap gap(req.getSkillId(), req.getSkillName(), currentLevel, req.getRequiredLevel(), req.getImportance());
            if (gap.hasGap()) {
                gaps.push_back(gap);
            }
        }

        // STL Priority Queue
        RoadmapService::SkillPriorityQueue pq;
        for (const auto& gap : gaps) {
            pq.push(gap);
        }

        std::vector<SkillGap> sortedGaps;
        sortedGaps.reserve(gaps.size());
        while (!pq.empty()) {
            sortedGaps.push_back(pq.top());
            pq.pop();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    outDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

EvaluationBenchmarkResult EvaluationService::runComparisonBenchmark(int iterations) {
    EvaluationBenchmarkResult result;
    result.iterationCount = iterations;
    result.dataSize = 12;

    runProceduralBenchmark(iterations, result.proceduralDurationUs);
    runOopBenchmark(iterations, result.oopDurationUs);

    if (result.oopDurationUs > 0) {
        result.speedupRatio = static_cast<double>(result.proceduralDurationUs) / result.oopDurationUs;
    } else {
        result.speedupRatio = 1.0;
    }

    result.proceduralComplexity = "Search: O(N*M) linear scan, Sort: O(K^2) bubble sort. Space: O(K) flat structs.";
    result.oopComplexity = "Search: O(N log M) via std::map, Sort: O(K log K) via std::priority_queue (heap). Space: O(K) encapsulated objects.";

    std::ostringstream ss;
    ss << "Academic Evaluation Conclusion:\n"
       << "- Algorithmic Efficiency: The OOP/STL approach utilizes a binary heap priority queue (O(K log K))\n"
       << "  and logarithmic associative lookups (O(log M)), yielding superior asymptotic scaling.\n"
       << "- Software Engineering: The OOP architecture encapsulates domain rules (clamping, gap calculations,\n"
       << "  polymorphic resource dispatch) within strongly-typed classes, reducing defect probability.\n"
       << "- Maintainability: Changes to importance scoring or task generation require zero modifications\n"
       << "  to calling presentation layers due to clear layer separation.";
    result.academicConclusion = ss.str();

    return result;
}

std::string EvaluationService::getDetailedAcademicReport() const {
    std::ostringstream ss;
    ss << "========================================================================\n"
       << " SKILLSCOPE ACADEMIC EVALUATION: PROCEDURAL VS OBJECT-ORIENTED PARADIGM\n"
       << "========================================================================\n\n"
       << "1. Encapsulation & Domain Invariants:\n"
       << "   - Procedural: Raw structs allow direct manipulation of variables without\n"
       << "     bounds enforcement. Negative proficiency or invalid task progress can corrupt state.\n"
       << "   - OOP: Invariants are guaranteed by constructors and setters (e.g., skill level\n"
       << "     clamped to 0..3, task progress to 0..100%).\n\n"
       << "2. Polymorphism vs Switch-Case Dispatch:\n"
       << "   - Procedural: Handling multiple resource types requires manual enum tagging\n"
       << "     and switch statements scattered across business logic.\n"
       << "   - OOP: LearningResource provides pure virtual methods (resourceType, learningDescription)\n"
       << "     specialized cleanly in VideoResource, ArticleResource, and PracticeResource.\n\n"
       << "3. STL-Based Priority Management:\n"
       << "   - Procedural: Manual array sorting incurs high complexity and boilerplate code.\n"
       << "   - OOP: std::priority_queue paired with custom functor comparators cleanly\n"
       << "     prioritizes multi-attribute skill gaps efficiently.\n\n"
       << "4. Layered Architecture & Separation of Concerns:\n"
       << "   - The 4-layer architecture (Presentation -> Service -> Model -> Database)\n"
       << "     ensures that Qt widgets never execute raw SQL and models remain decoupled\n"
       << "     from GUI controls.\n";
    return ss.str();
}
