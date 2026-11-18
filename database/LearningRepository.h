#ifndef LEARNINGREPOSITORY_H
#define LEARNINGREPOSITORY_H

#include "../models/LearningResource.h"
#include "../models/LearningTask.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Repository for learning resources and assigned student tasks.
 */
class LearningRepository {
public:
    LearningRepository() = default;

    std::vector<std::unique_ptr<LearningResource>> getResourcesForSkill(int skillId);
    std::vector<std::unique_ptr<LearningResource>> getResourcesByType(const std::string& type);
    std::vector<std::unique_ptr<LearningResource>> filterResources(int skillId, const std::string& type);
    std::vector<std::unique_ptr<LearningResource>> getAllResources();

    std::vector<LearningTask> getTasksForStudent(int studentId);
    bool createTask(const LearningTask& task, int& outTaskId);
    bool updateTaskProgress(int taskId, int progress, const std::string& status, const std::string& completionDate = "");
    bool completeTask(int taskId, const std::string& completionDate = "");
    bool deleteTask(int taskId);
    bool deleteTasksForStudent(int studentId);
    bool hasTaskForStudentAndSkill(int studentId, int skillId);
};

#endif // LEARNINGREPOSITORY_H
