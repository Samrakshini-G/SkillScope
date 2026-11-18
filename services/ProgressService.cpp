#include "ProgressService.h"
#include <numeric>
#include <cmath>
#include <QDate>
#include <map>

ProgressService::ProgressService() {}

ProgressSummary ProgressService::getProgressSummary(int studentId) {
    ProgressSummary summary = {0, 0, 0, 0, 0.0, 0.0, 0, 0};

    std::vector<LearningTask> tasks = m_learningRepo.getTasksForStudent(studentId);
    summary.totalTasks = static_cast<int>(tasks.size());

    int totalProgressSum = 0;
    for (const auto& task : tasks) {
        totalProgressSum += task.getProgress();
        if (task.getStatus() == "Completed" || task.getProgress() == 100) {
            summary.completedTasks++;
        } else if (task.getStatus() == "In Progress" || task.getProgress() > 0) {
            summary.inProgressTasks++;
        } else {
            summary.notStartedTasks++;
        }
    }

    if (summary.totalTasks > 0) {
        // Required Formula: Progress (%) = (Completed Tasks / Total Tasks) * 100
        summary.completionRate = std::round((static_cast<double>(summary.completedTasks) / summary.totalTasks) * 1000.0) / 10.0;
        summary.averageProgress = std::round((static_cast<double>(totalProgressSum) / summary.totalTasks) * 10.0) / 10.0;
    }

    auto studentSkills = m_skillRepo.getStudentSkillLevels(studentId);
    summary.totalSkillsTracked = static_cast<int>(studentSkills.size());

    for (const auto& [skillId, level] : studentSkills) {
        if (level >= 2) {
            summary.skillsAcquired++;
        }
    }

    return summary;
}

std::vector<SkillProgressSummary> ProgressService::getSkillWiseProgress(int studentId) {
    std::vector<SkillProgressSummary> result;
    auto tasks = m_learningRepo.getTasksForStudent(studentId);
    auto skillLevels = m_skillRepo.getStudentSkillLevels(studentId);

    // Group tasks by skillId
    std::map<int, std::pair<int, int>> skillCounts; // skillId -> (completed, total)
    for (const auto& t : tasks) {
        skillCounts[t.getSkillId()].second++;
        if (t.getStatus() == "Completed" || t.getProgress() == 100) {
            skillCounts[t.getSkillId()].first++;
        }
    }

    for (const auto& [sId, counts] : skillCounts) {
        SkillProgressSummary sp;
        sp.skillId = sId;
        auto skOpt = m_skillRepo.getSkillById(sId);
        sp.skillName = skOpt.has_value() ? skOpt->getName() : ("Skill #" + std::to_string(sId));
        sp.currentLevel = skillLevels.count(sId) ? skillLevels[sId] : 0;
        sp.completedTasks = counts.first;
        sp.totalTasks = counts.second;
        sp.progressPercent = counts.second > 0 ? (static_cast<double>(counts.first) / counts.second) * 100.0 : 0.0;
        result.push_back(sp);
    }
    return result;
}

std::vector<LearningTask> ProgressService::getStudentTasks(int studentId) {
    return m_learningRepo.getTasksForStudent(studentId);
}

bool ProgressService::updateTask(int taskId, int progress, const std::string& status, const std::string& completionDate) {
    std::string finalDate = completionDate;
    if (status == "Completed" && finalDate.empty()) {
        finalDate = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    }
    return m_learningRepo.updateTaskProgress(taskId, progress, status, finalDate);
}

bool ProgressService::completeTask(int taskId, const std::string& completionDate) {
    std::string dateStr = completionDate.empty() ? QDate::currentDate().toString("yyyy-MM-dd").toStdString() : completionDate;
    return m_learningRepo.completeTask(taskId, dateStr);
}

bool ProgressService::updateStudentSkillProficiency(int studentId, int skillId, int newLevel) {
    return m_skillRepo.setStudentSkillLevel(studentId, skillId, newLevel);
}
