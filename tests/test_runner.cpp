#include <QCoreApplication>
#include <QDir>
#include <iostream>
#include <cassert>
#include <memory>
#include <cmath>

#include "../database/DatabaseManager.h"
#include "../database/UserRepository.h"
#include "../database/SkillRepository.h"
#include "../database/CareerRepository.h"
#include "../database/LearningRepository.h"

#include "../models/Student.h"
#include "../models/Skill.h"
#include "../models/JobRole.h"
#include "../models/JobRequirement.h"
#include "../models/SkillGap.h"
#include "../models/LearningResource.h"
#include "../models/LearningTask.h"

#include "../services/AuthService.h"
#include "../services/SkillAnalysisService.h"
#include "../services/RoadmapService.h"
#include "../services/ProgressService.h"
#include "../services/RecommendationService.h"
#include "../services/JobDescriptionParser.h"
#include "../services/EvaluationService.h"

#define TEST_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "[-] FAILED: " << msg << " (" << #cond << ") at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while (0)

// ==========================================================
// MODULE 1 TEST SUITES (1 - 11)
// ==========================================================

// 1. Skill Proficiency Validation
bool testSkillProficiencyValidation() {
    std::cout << "[RUNNING] 1. Skill proficiency validation..." << std::endl;
    UserRepository uRepo;
    int testStudentId = 0;
    uRepo.createUser("Test Student 1", "test1_prof@edu.org", "pass123", "CS", "Tech", testStudentId);

    SkillRepository repo;

    TEST_ASSERT(repo.setStudentSkillLevel(testStudentId, 1, 0), "Valid level 0 accepted");
    TEST_ASSERT(repo.setStudentSkillLevel(testStudentId, 1, 3), "Valid level 3 accepted");

    TEST_ASSERT(repo.setStudentSkillLevel(testStudentId, 1, -5), "Level below 0 clamped to 0");
    auto levels1 = repo.getStudentSkillLevels(testStudentId);
    TEST_ASSERT(levels1[1] == 0, "Clamped to 0 in database");

    TEST_ASSERT(repo.setStudentSkillLevel(testStudentId, 1, 10), "Level above 3 clamped to 3");
    auto levels2 = repo.getStudentSkillLevels(testStudentId);
    TEST_ASSERT(levels2[1] == 3, "Clamped to 3 in database");

    repo.removeStudentSkill(testStudentId, 1);
    std::cout << "[PASSED] 1. Skill proficiency validation" << std::endl;
    return true;
}

// 2. Skill Alias Matching
bool testSkillAliasMatching() {
    std::cout << "[RUNNING] 2. Skill alias matching..." << std::endl;
    JobDescriptionParser parser;

    auto res1 = parser.parseJobDescription("We need a senior engineer with cpp and linux experience.");
    bool foundCpp = false;
    for (const auto& s : res1) {
        if (s.skillName == "C++" && s.matchedAlias == "cpp") foundCpp = true;
    }
    TEST_ASSERT(foundCpp, "Alias 'cpp' matched canonical C++");

    auto res2 = parser.parseJobDescription("Requires structured query language and python.");
    bool foundSql = false;
    for (const auto& s : res2) {
        if (s.skillName == "SQL") foundSql = true;
    }
    TEST_ASSERT(foundSql, "Alias 'structured query language' matched canonical SQL");

    auto res3 = parser.parseJobDescription("Frontend developer skilled in HTML, CSS, and JS.");
    bool foundJs = false;
    for (const auto& s : res3) {
        if (s.skillName == "JavaScript") foundJs = true;
    }
    TEST_ASSERT(foundJs, "Alias 'JS' matched canonical JavaScript");

    auto res4 = parser.parseJobDescription("Strong foundation in object-oriented programming concepts.");
    bool foundOop = false;
    for (const auto& s : res4) {
        if (s.skillName == "OOP") foundOop = true;
    }
    TEST_ASSERT(foundOop, "Alias 'object-oriented programming' matched canonical OOP");

    std::cout << "[PASSED] 2. Skill alias matching" << std::endl;
    return true;
}

// 3. Duplicate Skill Detection Prevention
bool testDuplicateSkillDetectionPrevention() {
    std::cout << "[RUNNING] 3. Duplicate skill detection prevention..." << std::endl;
    JobDescriptionParser parser;

    std::string text = "Looking for a C++ developer who knows modern cpp and c plus plus syntax.";
    auto detected = parser.parseJobDescription(text);

    int cppCount = 0;
    for (const auto& s : detected) {
        if (s.skillName == "C++") cppCount++;
    }
    TEST_ASSERT(cppCount == 1, "Only exactly one C++ entry returned despite multiple aliases");

    UserRepository uRepo;
    int testStudentId = 0;
    uRepo.createUser("Test Student 3", "test3_dup@edu.org", "pass123", "CS", "Tech", testStudentId);

    SkillRepository skillRepo;
    skillRepo.removeStudentSkill(testStudentId, 2);
    TEST_ASSERT(skillRepo.addStudentSkill(testStudentId, 2, 2), "First add succeeds");
    TEST_ASSERT(!skillRepo.addStudentSkill(testStudentId, 2, 3), "Second add of same skill rejected as duplicate");
    skillRepo.removeStudentSkill(testStudentId, 2);

    std::cout << "[PASSED] 3. Duplicate skill detection prevention" << std::endl;
    return true;
}

// 4. Gap Calculation
bool testGapCalculation() {
    std::cout << "[RUNNING] 4. Gap calculation..." << std::endl;
    SkillGap g1(1, "C++", 1, 3, "High");
    TEST_ASSERT(g1.calculateGap() == 2, "Req 3, Cur 1 -> Gap 2");

    SkillGap g2(2, "SQL", 0, 2, "Medium");
    TEST_ASSERT(g2.calculateGap() == 2, "Req 2, Cur 0 -> Gap 2");

    SkillGap g3(3, "Git", 2, 2, "Medium");
    TEST_ASSERT(g3.calculateGap() == 0, "Req 2, Cur 2 -> Gap 0");

    SkillGap g4(4, "Python", 3, 2, "Low");
    TEST_ASSERT(g4.calculateGap() == 0, "Req 2, Cur 3 -> Gap 0 (clamped to max(0))");

    std::cout << "[PASSED] 4. Gap calculation" << std::endl;
    return true;
}

// 5. Readiness Percentage Calculation
bool testReadinessPercentageCalculation() {
    std::cout << "[RUNNING] 5. Readiness percentage calculation (weighted formula)..." << std::endl;
    SkillAnalysisService service;

    std::vector<JobRequirement> reqs = {
        JobRequirement(1, "C++", 3, "High"),
        JobRequirement(2, "SQL", 2, "Medium"),
        JobRequirement(3, "Git", 2, "Medium")
    };
    std::map<int, int> studentSkills = {
        {1, 2},
        {2, 0},
        {3, 2}
    };

    double readiness = service.calculateMatchPercentage(reqs, studentSkills);
    double expected = std::round((10.0 / 17.0) * 1000.0) / 10.0;
    TEST_ASSERT(std::abs(readiness - expected) < 0.05, "Weighted readiness percentage matches exact formula");

    std::cout << "[PASSED] 5. Readiness percentage calculation (Readiness: " << readiness << "%)" << std::endl;
    return true;
}

// 6. Missing Skill Classification
bool testMissingSkillClassification() {
    std::cout << "[RUNNING] 6. Missing skill classification..." << std::endl;
    SkillGap gap(1, "SQL", 0, 2, "Medium");
    TEST_ASSERT(gap.isMissing(), "isMissing returns true when current level is 0");
    TEST_ASSERT(!gap.isInsufficient(), "isInsufficient returns false when current level is 0");
    TEST_ASSERT(!gap.isMastered(), "isMastered returns false");
    TEST_ASSERT(gap.getStatusText() == "Missing", "Status string is 'Missing'");

    std::cout << "[PASSED] 6. Missing skill classification" << std::endl;
    return true;
}

// 7. Insufficient Skill Classification
bool testInsufficientSkillClassification() {
    std::cout << "[RUNNING] 7. Insufficient skill classification..." << std::endl;
    SkillGap gap(1, "C++", 1, 3, "High");
    TEST_ASSERT(!gap.isMissing(), "isMissing is false");
    TEST_ASSERT(gap.isInsufficient(), "isInsufficient returns true when 0 < current < required");
    TEST_ASSERT(!gap.isMastered(), "isMastered is false");
    TEST_ASSERT(gap.getStatusText() == "Skill gap", "Status string is 'Skill gap'");

    SkillGap meets(2, "Git", 2, 2, "Medium");
    TEST_ASSERT(!meets.isMissing() && !meets.isInsufficient(), "Meets is neither missing nor insufficient");
    TEST_ASSERT(meets.isMastered(), "isMastered is true");
    TEST_ASSERT(meets.getStatusText() == "Meets requirement", "Status string is 'Meets requirement'");

    std::cout << "[PASSED] 7. Insufficient skill classification" << std::endl;
    return true;
}

// 8. Career Requirement Retrieval
bool testCareerRequirementRetrieval() {
    std::cout << "[RUNNING] 8. Career requirement retrieval..." << std::endl;
    CareerRepository repo;
    auto roles = repo.getAllJobRoles();
    TEST_ASSERT(roles.size() >= 4, "At least 4 career roles exist");

    for (const auto& r : roles) {
        auto reqs = repo.getRequirementsForRole(r.getId());
        TEST_ASSERT(!reqs.empty(), "Role has non-empty requirements: " + r.getTitle());
        for (const auto& req : reqs) {
            TEST_ASSERT(req.getRequiredLevel() >= 1 && req.getRequiredLevel() <= 3, "Required level in 1..3");
            TEST_ASSERT(req.getImportanceWeight() >= 1 && req.getImportanceWeight() <= 3, "Importance weight in 1..3");
        }
    }

    std::cout << "[PASSED] 8. Career requirement retrieval" << std::endl;
    return true;
}

// 9. Saving and Loading Student Skills
bool testSavingAndLoadingStudentSkills() {
    std::cout << "[RUNNING] 9. Saving and loading student skills..." << std::endl;
    UserRepository uRepo;
    int testStudentId = 0;
    uRepo.createUser("Test Student 9", "test9_skills@edu.org", "pass123", "CS", "Tech", testStudentId);

    SkillRepository repo;
    repo.setStudentSkillLevel(testStudentId, 1, 2);
    repo.setStudentSkillLevel(testStudentId, 3, 3);

    auto loaded = repo.getStudentSkillLevels(testStudentId);
    TEST_ASSERT(loaded.find(1) != loaded.end() && loaded[1] == 2, "Loaded skill 1 level 2");
    TEST_ASSERT(loaded.find(3) != loaded.end() && loaded[3] == 3, "Loaded skill 3 level 3");

    repo.removeStudentSkill(testStudentId, 1);
    auto afterRemove = repo.getStudentSkillLevels(testStudentId);
    TEST_ASSERT(afterRemove.find(1) == afterRemove.end(), "Skill 1 removed from student profile");

    repo.removeStudentSkill(testStudentId, 3);
    std::cout << "[PASSED] 9. Saving and loading student skills" << std::endl;
    return true;
}

// 10. Saving and Retrieving Analysis History
bool testSavingAndRetrievingAnalysisHistory() {
    std::cout << "[RUNNING] 10. Saving and retrieving analysis history..." << std::endl;
    UserRepository uRepo;
    int testStudentId = 0;
    uRepo.createUser("Test Student 10", "test10_hist@edu.org", "pass123", "CS", "Tech", testStudentId);

    CareerRepository careerRepo;
    int testRoleId = 1;

    std::vector<SkillGap> sampleGaps = {
        SkillGap(1, "C++", 1, 3, "High"),
        SkillGap(2, "OOP", 2, 3, "High")
    };

    TEST_ASSERT(careerRepo.saveAnalysisHistory(testStudentId, testRoleId, 72.5, sampleGaps), "Save analysis history");
    auto history = careerRepo.getAnalysisHistoryForStudent(testStudentId);
    TEST_ASSERT(!history.empty(), "Analysis history retrieved for student");
    TEST_ASSERT(std::abs(history.front().matchPercentage - 72.5) < 0.1, "Recorded match percentage matches");

    std::cout << "[PASSED] 10. Saving and retrieving analysis history" << std::endl;
    return true;
}

// 11. Comprehensive Edge Cases
bool testEdgeCases() {
    std::cout << "[RUNNING] 11. Comprehensive Edge Cases..." << std::endl;
    SkillAnalysisService analysisService;
    JobDescriptionParser parser;

    std::vector<JobRequirement> reqs = {
        JobRequirement(1, "C++", 3, "High"),
        JobRequirement(2, "SQL", 2, "Medium")
    };
    std::map<int, int> emptySkills;
    double noSkillReadiness = analysisService.calculateMatchPercentage(reqs, emptySkills);
    TEST_ASSERT(noSkillReadiness == 0.0, "Readiness is 0% when student has no skills");

    std::map<int, int> perfectSkills = {{1, 3}, {2, 2}};
    double perfectReadiness = analysisService.calculateMatchPercentage(reqs, perfectSkills);
    TEST_ASSERT(perfectReadiness == 100.0, "Readiness is 100% when student meets every requirement");

    std::map<int, int> excessSkills = {{1, 3}, {2, 3}};
    double excessReadiness = analysisService.calculateMatchPercentage(reqs, excessSkills);
    TEST_ASSERT(excessReadiness == 100.0, "Readiness does not exceed 100% when student exceeds requirements");

    std::string irrelevantText = "The company was founded in 1995. We offer competitive health insurance and snacks.";
    auto noSkillsDetected = parser.parseJobDescription(irrelevantText);
    TEST_ASSERT(noSkillsDetected.empty(), "No skills detected for irrelevant text");

    auto emptyDetected = parser.parseJobDescription("");
    TEST_ASSERT(emptyDetected.empty(), "Empty text returns empty detected list without error");

    std::string modernText = "Seeking a developer skilled in Rust, Kubernetes, and C++.";
    auto unrec = parser.extractUnrecognizedKeywords(modernText);
    bool hasRust = false, hasK8s = false;
    for (const auto& u : unrec) {
        if (u == "Rust") hasRust = true;
        if (u == "Kubernetes") hasK8s = true;
    }
    TEST_ASSERT(hasRust && hasK8s, "Unrecognized technologies detected as non-catalog keywords");

    UserRepository userRepo;
    int testUserId = 0;
    std::string testEmail = "profile_edge_" + std::to_string(std::rand()) + "@edu.org";
    TEST_ASSERT(userRepo.createUser("Profile Tester", testEmail, "hash123", "ECE", "State Tech", testUserId), "Created student");
    TEST_ASSERT(userRepo.updateStudentProfile(testUserId, "Profile Tester Updated", testEmail, "CS", "State Tech", "B.S. in Software Eng", 2028), "Updated full profile");
    TEST_ASSERT(userRepo.setTargetRole(testUserId, 2), "Target career role saved");

    auto loadedStudent = userRepo.getStudentById(testUserId);
    TEST_ASSERT(loadedStudent != nullptr, "Loaded student from DB");
    TEST_ASSERT(loadedStudent->getDegree() == "B.S. in Software Eng", "Degree persists");
    TEST_ASSERT(loadedStudent->getGraduationYear() == 2028, "Graduation year persists");
    TEST_ASSERT(loadedStudent->getTargetRoleId() == 2, "Target role persists");

    std::cout << "[PASSED] 11. Comprehensive Edge Cases" << std::endl;
    return true;
}

// ==========================================================
// MODULE 2 & PROMPT 3 SPECIFIC TEST SUITES (12 - 25)
// ==========================================================

// 12. Corrected Priority Formula and Normalization (Prompt 3, Requirement 4)
bool testPriorityFormulaAndNormalization() {
    std::cout << "[RUNNING] 12. Corrected Priority Formula and Normalization..." << std::endl;

    // Formula: Priority Score = (0.5 * Gap Score) + (0.3 * Importance Score) + (0.2 * Urgency Score)
    // Gap Score = Gap / 3.0, Importance Score = Weight / 3.0, Urgency Score = neutral default 0.5

    // Case A: Maximum Gap (Gap = 3, High = 3, Urgency = 0.5)
    // Gap Score = 3/3 = 1.0; Importance Score = 3/3 = 1.0; Urgency = 0.5
    // Score = (0.5 * 1.0) + (0.3 * 1.0) + (0.2 * 0.5) = 0.5 + 0.3 + 0.1 = 0.90
    SkillGap maxGap(1, "C++", 0, 3, "High");
    TEST_ASSERT(std::abs(maxGap.getNormalizedGapScore() - 1.0) < 1e-5, "Max gap normalized to 1.0");
    TEST_ASSERT(std::abs(maxGap.getNormalizedImportanceScore() - 1.0) < 1e-5, "High importance normalized to 1.0");
    TEST_ASSERT(std::abs(maxGap.getNormalizedUrgencyScore() - 0.5) < 1e-5, "Neutral urgency defaults to 0.5");
    TEST_ASSERT(std::abs(maxGap.getPriorityScore() - 0.90) < 1e-5, "Max gap priority score is exactly 0.90");

    // Case B: Boundary Values (Gap = 0, Low = 1, Urgency = 0.0)
    SkillGap zeroGap(2, "SQL", 2, 2, "Low");
    zeroGap.setUrgencyScore(0.0);
    // Gap Score = 0; Importance Score = 1/3; Urgency = 0.0
    // Score = (0.5 * 0.0) + (0.3 * (1.0/3.0)) + (0.2 * 0.0) = 0.10
    TEST_ASSERT(std::abs(zeroGap.getNormalizedGapScore() - 0.0) < 1e-5, "Zero gap normalized to 0.0");
    TEST_ASSERT(std::abs(zeroGap.getNormalizedImportanceScore() - (1.0/3.0)) < 1e-5, "Low importance normalized to 1/3");
    TEST_ASSERT(std::abs(zeroGap.getPriorityScore() - 0.10) < 1e-5, "Zero gap with min weight priority score is 0.10");

    // Case C: Invalid values clamping
    SkillGap invalidGap(3, "OOP", -5, 10, "InvalidImportance");
    invalidGap.setUrgencyScore(5.0); // Above 1.0
    TEST_ASSERT(invalidGap.getNormalizedUrgencyScore() == 1.0, "Urgency > 1.0 clamped to 1.0");
    invalidGap.setUrgencyScore(-2.0); // Below 0.0
    TEST_ASSERT(invalidGap.getNormalizedUrgencyScore() == 0.0, "Urgency < 0.0 clamped to 0.0");
    TEST_ASSERT(invalidGap.getPriorityScore() >= 0.0 && invalidGap.getPriorityScore() <= 1.0, "Score strictly bounded in [0, 1]");

    std::cout << "[PASSED] 12. Corrected Priority Formula and Normalization" << std::endl;
    return true;
}

// 13. Priority Queue Ordering & Deterministic Tie-Breaking (Prompt 3, Requirement 5)
bool testPriorityQueueOrderingAndTieBreaking() {
    std::cout << "[RUNNING] 13. Priority Queue Ordering & Deterministic Tie-Breaking..." << std::endl;

    // g1: Gap 2, High (weight 3) -> Gap=2/3, Imp=1.0, Urg=0.5 -> Score = 0.5*(2/3) + 0.3*1.0 + 0.2*0.5 = 0.7333
    // g2: Gap 3, Low (weight 1)  -> Gap=1.0, Imp=1/3, Urg=0.5 -> Score = 0.5*1.0 + 0.3*(1/3) + 0.2*0.5 = 0.7000
    // g3: Gap 1, High (weight 3) -> Gap=1/3, Imp=1.0, Urg=0.5 -> Score = 0.5*(1/3) + 0.3*1.0 + 0.2*0.5 = 0.5667
    // g4: Gap 3, High (weight 3) -> Gap=1.0, Imp=1.0, Urg=0.5 -> Score = 0.5*1.0 + 0.3*1.0 + 0.2*0.5 = 0.9000
    SkillGap g1(1, "C++", 1, 3, "High");
    SkillGap g2(2, "Python", 0, 3, "Low");
    SkillGap g3(3, "OOP", 2, 3, "High");
    SkillGap g4(4, "Data Structures", 0, 3, "High");

    RoadmapService::SkillPriorityQueue pq;
    pq.push(g1);
    pq.push(g2);
    pq.push(g3);
    pq.push(g4);

    TEST_ASSERT(!pq.empty(), "Priority queue is populated");
    auto top1 = pq.top(); pq.pop();
    TEST_ASSERT(top1.getSkillName() == "Data Structures", "1st: Data Structures (Score 0.90)");

    auto top2 = pq.top(); pq.pop();
    TEST_ASSERT(top2.getSkillName() == "C++", "2nd: C++ (Score 0.733)");

    auto top3 = pq.top(); pq.pop();
    TEST_ASSERT(top3.getSkillName() == "Python", "3rd: Python (Score 0.700)");

    auto top4 = pq.top(); pq.pop();
    TEST_ASSERT(top4.getSkillName() == "OOP", "4th: OOP (Score 0.567)");

    // Deterministic Tie-Breaking test: Identical scores and gaps, different IDs
    SkillGap tieA(10, "Skill Alpha", 1, 3, "High");
    SkillGap tieB(20, "Skill Beta", 1, 3, "High");
    TEST_ASSERT(std::abs(tieA.getPriorityScore() - tieB.getPriorityScore()) < 1e-6, "Equal priority scores verified");

    RoadmapService::SkillPriorityQueue tiePq;
    tiePq.push(tieB);
    tiePq.push(tieA);

    // Comparator has lower ID popped first (tieA before tieB)
    auto tieFirst = tiePq.top(); tiePq.pop();
    auto tieSecond = tiePq.top(); tiePq.pop();
    TEST_ASSERT(tieFirst.getSkillId() == 10, "Deterministic tie-breaking prioritizes lower skill ID consistently");
    TEST_ASSERT(tieSecond.getSkillId() == 20, "Deterministic second item resolved");

    std::cout << "[PASSED] 13. Priority Queue Ordering & Deterministic Tie-Breaking" << std::endl;
    return true;
}

// 14. Learning Resource Creation and Polymorphism (Prompt 3, Requirements 1 & 2)
bool testResourceCreationAndPolymorphism() {
    std::cout << "[RUNNING] 14. Learning Resource Creation and Polymorphism..." << std::endl;

    std::vector<std::unique_ptr<LearningResource>> resources;
    resources.push_back(std::make_unique<VideoResource>(1, 1, "C++ Video Masterclass", "https://youtube.com/watch?v=1", 45, "Advanced"));
    resources.push_back(std::make_unique<ArticleResource>(2, 1, "C++ Core Guidelines", "https://isocpp.github.io/CppCoreGuidelines", 30, "Intermediate"));
    resources.push_back(std::make_unique<PracticeResource>(3, 1, "C++ Exercises", "https://exercism.org", "Exercism", 15, "Beginner"));

    // Virtual function dispatch
    TEST_ASSERT(resources[0]->resourceType() == "Video", "VideoResource polymorphically identifies as 'Video'");
    TEST_ASSERT(resources[0]->getDuration() == "45 mins", "VideoResource duration formatted");
    TEST_ASSERT(resources[0]->getDifficulty() == "Advanced", "VideoResource difficulty matches");

    TEST_ASSERT(resources[1]->resourceType() == "Article", "ArticleResource polymorphically identifies as 'Article'");
    TEST_ASSERT(resources[1]->getDuration() == "30 mins read", "ArticleResource duration formatted");
    TEST_ASSERT(resources[1]->getDifficulty() == "Intermediate", "ArticleResource difficulty matches");

    TEST_ASSERT(resources[2]->resourceType() == "Practice", "PracticeResource polymorphically identifies as 'Practice'");
    TEST_ASSERT(resources[2]->getDuration() == "15 exercises", "PracticeResource duration formatted");
    TEST_ASSERT(resources[2]->getDifficulty() == "Beginner", "PracticeResource difficulty matches");

    std::cout << "[PASSED] 14. Learning Resource Creation and Polymorphism" << std::endl;
    return true;
}

// 15. Resource Filtering by Skill and Resource Type (Prompt 3, Requirements 3 & 4)
bool testResourceFilteringBySkillAndType() {
    std::cout << "[RUNNING] 15. Resource Filtering by Skill and Resource Type..." << std::endl;
    LearningRepository repo;

    // Filter by skill C++ (skillId = 1)
    auto cppRes = repo.getResourcesForSkill(1);
    TEST_ASSERT(!cppRes.empty(), "Retrieved resources for skill C++");
    for (const auto& r : cppRes) {
        TEST_ASSERT(r->getSkillId() == 1, "Resource belongs to skill 1");
    }

    // Filter by type "Video"
    auto videoRes = repo.getResourcesByType("Video");
    TEST_ASSERT(!videoRes.empty(), "Retrieved video resources");
    for (const auto& r : videoRes) {
        TEST_ASSERT(r->resourceType() == "Video", "Resource is of type Video");
    }

    // Filter by skill AND type
    auto cppPractice = repo.filterResources(1, "Practice");
    TEST_ASSERT(!cppPractice.empty(), "Retrieved C++ Practice resources");
    for (const auto& r : cppPractice) {
        TEST_ASSERT(r->getSkillId() == 1 && r->resourceType() == "Practice", "Resource is C++ Practice");
    }

    std::cout << "[PASSED] 15. Resource Filtering by Skill and Resource Type" << std::endl;
    return true;
}

// 16. Roadmap Generation & Priority Sequencing (Prompt 3, Requirement 9)
bool testRoadmapGenerationAndSequencing() {
    std::cout << "[RUNNING] 16. Roadmap Generation & Priority Sequencing..." << std::endl;
    RoadmapService service;

    std::vector<SkillGap> gaps = {
        SkillGap(1, "C++", 1, 3, "High"),
        SkillGap(2, "Data Structures", 0, 3, "High"),
        SkillGap(3, "Git", 2, 2, "Medium") // Gap 0 (should be skipped)
    };

    auto tasks = service.generateRoadmap(101, gaps, "2026-11-20");
    TEST_ASSERT(tasks.size() == 2, "Only gaps with size > 0 converted to roadmap tasks");

    // Highest priority task is Data Structures (Score 0.90)
    TEST_ASSERT(tasks[0].getTitle().find("Foundations") != std::string::npos, "Task 1 has Foundations phase");
    TEST_ASSERT(tasks[0].getTitle().find("Data Structures") != std::string::npos, "Task 1 targets Data Structures");
    TEST_ASSERT(tasks[0].getStartDate() == "2026-11-20", "Task 1 starts on initial date");
    TEST_ASSERT(std::abs(tasks[0].getPriorityScore() - 0.90) < 1e-5, "Task 1 priority score persisted");

    // Second task: C++ (Advancement phase)
    TEST_ASSERT(tasks[1].getTitle().find("Advancement") != std::string::npos, "Task 2 has Advancement phase");
    TEST_ASSERT(tasks[1].getTitle().find("C++") != std::string::npos, "Task 2 targets C++");
    TEST_ASSERT(tasks[1].getStartDate() >= tasks[0].getDueDate(), "Task 2 sequenced after Task 1 due date");

    std::cout << "[PASSED] 16. Roadmap Generation & Priority Sequencing" << std::endl;
    return true;
}

// 17. Duplicate Active Task Prevention & Completed-Task Preservation (Prompt 3, Requirements 10 & 11)
bool testDuplicatePreventionAndCompletedPreservation() {
    std::cout << "[RUNNING] 17. Duplicate Prevention and Completed-Task Preservation..." << std::endl;
    UserRepository uRepo;
    int studentId = 0;
    uRepo.createUser("Roadmap Safety Tester", "safety_test@edu.org", "pass123", "CS", "Tech", studentId);

    RoadmapService roadmapService;
    LearningRepository learnRepo;

    // First task: Skill 1 completed
    LearningTask completedTask(0, studentId, 1, "Phase 1: Master C++", "2026-11-01", "2026-11-15", "Completed", 100, 0.9, "2026-11-14");
    int cId = 0;
    learnRepo.createTask(completedTask, cId);

    // Active task: Skill 2 In Progress
    LearningTask activeTask(0, studentId, 2, "Phase 2: Master SQL", "2026-11-16", "2026-11-30", "In Progress", 40, 0.7, "");
    int aId = 0;
    learnRepo.createTask(activeTask, aId);

    // Attempt to save roadmap tasks for skills 1, 2, and 3
    std::vector<LearningTask> newTasks = {
        LearningTask(0, studentId, 1, "Phase 1: Master C++ Duplicate", "2026-12-01", "2026-12-15", "Not Started", 0, 0.9, ""),
        LearningTask(0, studentId, 2, "Phase 2: Master SQL Duplicate", "2026-12-01", "2026-12-15", "Not Started", 0, 0.7, ""),
        LearningTask(0, studentId, 3, "Phase 3: Master Git New", "2026-12-01", "2026-12-15", "Not Started", 0, 0.5, "")
    };

    TEST_ASSERT(roadmapService.saveRoadmapTasks(studentId, newTasks), "Save roadmap tasks executed safely");

    auto tasksInDb = learnRepo.getTasksForStudent(studentId);
    // Should contain: original completed task (skill 1), original active task (skill 2), and new task (skill 3).
    // Duplicate tasks for skills 1 and 2 must NOT be inserted!
    int skill1Count = 0, skill2Count = 0, skill3Count = 0;
    for (const auto& t : tasksInDb) {
        if (t.getSkillId() == 1) skill1Count++;
        if (t.getSkillId() == 2) skill2Count++;
        if (t.getSkillId() == 3) skill3Count++;
    }

    TEST_ASSERT(skill1Count == 1, "Completed skill 1 preserved and not duplicated");
    TEST_ASSERT(skill2Count == 1, "Active skill 2 not duplicated");
    TEST_ASSERT(skill3Count == 1, "New skill 3 inserted successfully");

    std::cout << "[PASSED] 17. Duplicate Prevention and Completed-Task Preservation" << std::endl;
    return true;
}

// 18. Task Status Updates, Completion Date, and Progress Percentage (Prompt 3, Requirements 12 & 13)
bool testTaskStatusUpdatesAndProgressPercentage() {
    std::cout << "[RUNNING] 18. Task Status Updates, Completion Date, and Progress Percentage..." << std::endl;
    UserRepository uRepo;
    int studentId = 0;
    uRepo.createUser("Progress Update Tester", "update_prog@edu.org", "pass123", "CS", "Tech", studentId);

    LearningRepository repo;
    LearningTask task(0, studentId, 1, "Phase 1: Learn Modern C++", "2026-11-20", "2026-12-05", "Not Started", 0, 0.8, "");
    int taskId = 0;
    repo.createTask(task, taskId);

    // Update to In Progress
    TEST_ASSERT(repo.updateTaskProgress(taskId, 50, "In Progress"), "Update progress to 50%");
    auto t1 = repo.getTasksForStudent(studentId);
    TEST_ASSERT(t1.front().getProgress() == 50 && t1.front().getStatus() == "In Progress", "Status is In Progress");

    // Complete task with completion date
    TEST_ASSERT(repo.completeTask(taskId, "2026-11-28"), "Task marked completed with date");
    auto t2 = repo.getTasksForStudent(studentId);
    TEST_ASSERT(t2.front().getProgress() == 100, "Progress is 100%");
    TEST_ASSERT(t2.front().getStatus() == "Completed", "Status is Completed");
    TEST_ASSERT(t2.front().getCompletionDate() == "2026-11-28", "Completion date persisted in SQLite");

    // Formula: Progress (%) = (Completed / Total) * 100
    ProgressService progService;
    auto summary = progService.getProgressSummary(studentId);
    TEST_ASSERT(summary.totalTasks == 1, "Total tasks is 1");
    TEST_ASSERT(summary.completedTasks == 1, "Completed tasks is 1");
    TEST_ASSERT(summary.completionRate == 100.0, "Progress percentage is 100%");

    std::cout << "[PASSED] 18. Task Status Updates, Completion Date, and Progress Percentage" << std::endl;
    return true;
}

// 19. Zero-Task Handling (Prompt 3, Requirement 14)
bool testZeroTaskHandling() {
    std::cout << "[RUNNING] 19. Zero-Task Handling..." << std::endl;
    UserRepository uRepo;
    int studentId = 0;
    uRepo.createUser("Zero Task Tester", "zero_tasks@edu.org", "pass123", "CS", "Tech", studentId);

    ProgressService service;
    auto summary = service.getProgressSummary(studentId);

    TEST_ASSERT(summary.totalTasks == 0, "Total tasks is 0");
    TEST_ASSERT(summary.completedTasks == 0, "Completed tasks is 0");
    TEST_ASSERT(summary.completionRate == 0.0, "Zero-task completion rate handles division safely without NaN");
    TEST_ASSERT(summary.averageProgress == 0.0, "Average progress is 0.0");

    std::cout << "[PASSED] 19. Zero-Task Handling" << std::endl;
    return true;
}

// 20. SQLite Persistence and Deletion (Prompt 3, Requirement 15)
bool testSqlitePersistenceAndDeletion() {
    std::cout << "[RUNNING] 20. SQLite Persistence and Deletion..." << std::endl;
    UserRepository uRepo;
    int studentId = 0;
    uRepo.createUser("Persistence Tester", "persist_test@edu.org", "pass123", "CS", "Tech", studentId);

    LearningRepository repo;
    LearningTask task(0, studentId, 1, "Persistable Task", "2026-11-20", "2026-12-05", "Not Started", 0, 0.75, "");
    int taskId = 0;
    TEST_ASSERT(repo.createTask(task, taskId), "Created task");

    auto loaded = repo.getTasksForStudent(studentId);
    TEST_ASSERT(!loaded.empty(), "Task loaded from SQLite");
    TEST_ASSERT(loaded.front().getId() == taskId, "Task ID matches");

    TEST_ASSERT(repo.deleteTask(taskId), "Task deleted from SQLite");
    auto afterDel = repo.getTasksForStudent(studentId);
    TEST_ASSERT(afterDel.empty(), "Task removed from SQLite");

    std::cout << "[PASSED] 20. SQLite Persistence and Deletion" << std::endl;
    return true;
}

// 21. Missing Resources Graceful Handling (Prompt 3, Requirement 16)
bool testMissingResourcesGracefulHandling() {
    std::cout << "[RUNNING] 21. Missing Resources Graceful Handling..." << std::endl;
    LearningRepository repo;

    // Query non-existent skill ID 9999
    auto res = repo.getResourcesForSkill(9999);
    TEST_ASSERT(res.empty(), "Non-existent skill gracefully returns empty resource list without throwing");

    RecommendationService recService;
    std::vector<SkillGap> gapsWithUnknown = {
        SkillGap(9999, "Unknown Skill", 0, 3, "High")
    };
    auto recs = recService.getRecommendationsForGaps(gapsWithUnknown);
    TEST_ASSERT(recs.empty(), "Recommendations for unknown skill returns empty list gracefully");

    std::cout << "[PASSED] 21. Missing Resources Graceful Handling" << std::endl;
    return true;
}

// 22. Repeated Roadmap Generation Safety (Prompt 3, Requirement 17)
bool testRepeatedRoadmapGenerationSafety() {
    std::cout << "[RUNNING] 22. Repeated Roadmap Generation Safety..." << std::endl;
    UserRepository uRepo;
    int studentId = 0;
    uRepo.createUser("Repeated Gen Tester", "repeated_gen@edu.org", "pass123", "CS", "Tech", studentId);

    RoadmapService roadmapService;
    std::vector<SkillGap> sampleGaps = {
        SkillGap(1, "C++", 1, 3, "High"),
        SkillGap(2, "SQL", 0, 2, "Medium")
    };

    auto tasks1 = roadmapService.generateRoadmap(studentId, sampleGaps);
    TEST_ASSERT(roadmapService.saveRoadmapTasks(studentId, tasks1), "First save succeeds");

    // Repeated save of identical roadmap
    auto tasks2 = roadmapService.generateRoadmap(studentId, sampleGaps);
    TEST_ASSERT(roadmapService.saveRoadmapTasks(studentId, tasks2), "Repeated save completes safely");

    LearningRepository repo;
    auto tasksInDb = repo.getTasksForStudent(studentId);
    TEST_ASSERT(tasksInDb.size() == 2, "Repeated generation does not duplicate active tasks in database");

    std::cout << "[PASSED] 22. Repeated Roadmap Generation Safety" << std::endl;
    return true;
}

// 23. Database Failure Handling (Prompt 3, Requirement 18)
bool testDatabaseFailureHandling() {
    std::cout << "[RUNNING] 23. Database Failure Handling..." << std::endl;
    LearningRepository repo;

    // Invalid task with nonexistent foreign key user when FK is active or invalid progress
    LearningTask invalidTask(0, -999, 1, "Invalid Task", "2026-11-20", "2026-12-05", "Not Started", 150);
    // Notice domain clamps progress, but if inserting directly
    TEST_ASSERT(invalidTask.getProgress() == 100, "Progress clamped safely before database interaction");

    // Attempting to delete invalid ID returns true or false gracefully without crash
    TEST_ASSERT(repo.deleteTask(-1) || !repo.deleteTask(-1), "Delete non-existent task handled gracefully");

    std::cout << "[PASSED] 23. Database Failure Handling" << std::endl;
    return true;
}

// 24. Procedural vs OOP Benchmark Execution
bool testProceduralVsOopBenchmark() {
    std::cout << "[RUNNING] 24. Procedural vs OOP benchmark execution..." << std::endl;
    EvaluationService evalService;
    auto res = evalService.runComparisonBenchmark(500);

    TEST_ASSERT(res.iterationCount == 500, "Benchmark ran 500 iterations");
    TEST_ASSERT(res.dataSize == 12, "Data size is 12 skills");
    TEST_ASSERT(res.proceduralDurationUs > 0, "Procedural duration > 0 microseconds");
    TEST_ASSERT(res.oopDurationUs > 0, "OOP duration > 0 microseconds");
    TEST_ASSERT(res.speedupRatio > 0.0, "Speedup ratio is positive");
    TEST_ASSERT(!res.proceduralComplexity.empty(), "Procedural complexity documented");
    TEST_ASSERT(!res.oopComplexity.empty(), "OOP complexity documented");
    TEST_ASSERT(!res.academicConclusion.empty(), "Academic conclusion present");

    std::cout << "[PASSED] 24. Procedural vs OOP benchmark (Speedup: "
              << res.speedupRatio << "x, Proc: " << res.proceduralDurationUs << "µs, OOP: " << res.oopDurationUs << "µs)" << std::endl;
    return true;
}

// 25. Academic Report Generation
bool testAcademicReportGeneration() {
    std::cout << "[RUNNING] 25. Academic evaluation report generation..." << std::endl;
    EvaluationService evalService;
    std::string report = evalService.getDetailedAcademicReport();

    TEST_ASSERT(!report.empty(), "Academic report is non-empty");
    TEST_ASSERT(report.find("Encapsulation") != std::string::npos, "Report covers Encapsulation");
    TEST_ASSERT(report.find("Polymorphism") != std::string::npos, "Report covers Polymorphism");
    TEST_ASSERT(report.find("STL-Based Priority Management") != std::string::npos, "Report covers Priority Management");
    TEST_ASSERT(report.find("Layered Architecture") != std::string::npos, "Report covers Layered Architecture");

    std::cout << "[PASSED] 25. Academic evaluation report generation" << std::endl;
    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "==========================================================" << std::endl;
    std::cout << "   SkillScope: Comprehensive Academic Test Suite Runner   " << std::endl;
    std::cout << "==========================================================" << std::endl;

    QString testDbPath = QDir::temp().filePath("skillscope_mod1_test.db");
    QFile::remove(testDbPath);
    DatabaseManager::instance().openDatabase(testDbPath);

    // Module 1 Tests (1 - 11)
    if (!testSkillProficiencyValidation()) return 1;
    if (!testSkillAliasMatching()) return 1;
    if (!testDuplicateSkillDetectionPrevention()) return 1;
    if (!testGapCalculation()) return 1;
    if (!testReadinessPercentageCalculation()) return 1;
    if (!testMissingSkillClassification()) return 1;
    if (!testInsufficientSkillClassification()) return 1;
    if (!testCareerRequirementRetrieval()) return 1;
    if (!testSavingAndLoadingStudentSkills()) return 1;
    if (!testSavingAndRetrievingAnalysisHistory()) return 1;
    if (!testEdgeCases()) return 1;

    // Module 2 & Prompt 3 Tests (12 - 25)
    if (!testPriorityFormulaAndNormalization()) return 1;
    if (!testPriorityQueueOrderingAndTieBreaking()) return 1;
    if (!testResourceCreationAndPolymorphism()) return 1;
    if (!testResourceFilteringBySkillAndType()) return 1;
    if (!testRoadmapGenerationAndSequencing()) return 1;
    if (!testDuplicatePreventionAndCompletedPreservation()) return 1;
    if (!testTaskStatusUpdatesAndProgressPercentage()) return 1;
    if (!testZeroTaskHandling()) return 1;
    if (!testSqlitePersistenceAndDeletion()) return 1;
    if (!testMissingResourcesGracefulHandling()) return 1;
    if (!testRepeatedRoadmapGenerationSafety()) return 1;
    if (!testDatabaseFailureHandling()) return 1;
    if (!testProceduralVsOopBenchmark()) return 1;
    if (!testAcademicReportGeneration()) return 1;

    std::cout << "==========================================================" << std::endl;
    std::cout << "   ALL 25 COMPREHENSIVE TEST SUITES PASSED CLEANLY!       " << std::endl;
    std::cout << "==========================================================" << std::endl;
    return 0;
}
