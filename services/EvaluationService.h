#ifndef EVALUATIONSERVICE_H
#define EVALUATIONSERVICE_H

#include "../models/SkillGap.h"
#include "../models/JobRequirement.h"
#include <string>
#include <vector>

struct EvaluationBenchmarkResult {
    long long proceduralDurationUs;
    long long oopDurationUs;
    int iterationCount;
    int dataSize;
    double speedupRatio;
    std::string proceduralComplexity;
    std::string oopComplexity;
    std::string academicConclusion;
};

/**
 * @brief Service performing academic comparison between Procedural programming and
 *        Object-Oriented Programming (OOP) with STL priority management.
 */
class EvaluationService {
public:
    EvaluationService();

    EvaluationBenchmarkResult runComparisonBenchmark(int iterations = 5000);

    std::string getDetailedAcademicReport() const;

private:
    void runProceduralBenchmark(int iterations, long long& outDurationUs);
    void runOopBenchmark(int iterations, long long& outDurationUs);
};

#endif // EVALUATIONSERVICE_H
