#ifndef PROGRESSSERVICE_H
#define PROGRESSSERVICE_H

#include "../models/LearningTask.h"
#include "../database/LearningRepository.h"
#include "../database/SkillRepository.h"
#include <vector>

struct ProgressSummary {
    int totalTasks;
    int completedTasks;
    int inProgressTasks;
    int notStartedTasks;
    double completionRate; // (completedTasks / totalTasks) * 100.0 %
    double averageProgress; // average progress across all tasks
    int skillsAcquired;
    int totalSkillsTracked;
};

struct SkillProgressSummary {
    int skillId;
    std::string skillName;
    int currentLevel;
    int totalTasks;
    int completedTasks;
    double progressPercent;
};

/**
 * @brief Service computing completion metrics and monitoring career progression.
 */
class ProgressService {
public:
    ProgressService();

    ProgressSummary getProgressSummary(int studentId);
    std::vector<SkillProgressSummary> getSkillWiseProgress(int studentId);
    std::vector<LearningTask> getStudentTasks(int studentId);
    bool updateTask(int taskId, int progress, const std::string& status, const std::string& completionDate = "");
    bool completeTask(int taskId, const std::string& completionDate = "");
    bool updateStudentSkillProficiency(int studentId, int skillId, int newLevel);

private:
    LearningRepository m_learningRepo;
    SkillRepository m_skillRepo;
};

#endif // PROGRESSSERVICE_H
