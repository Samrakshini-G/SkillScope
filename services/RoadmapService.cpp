#include "RoadmapService.h"
#include <QDate>
#include <unordered_set>

RoadmapService::RoadmapService() {}

std::vector<LearningTask> RoadmapService::generateRoadmap(int studentId, const std::vector<SkillGap>& gaps,
                                                         const std::string& startDateStr) {
    std::vector<LearningTask> generatedTasks;

    // 1. Build priority queue from identified skill gaps
    SkillPriorityQueue pq;
    for (const auto& gap : gaps) {
        if (gap.hasGap()) {
            pq.push(gap);
        }
    }

    QDate currentDate = QDate::fromString(QString::fromStdString(startDateStr), "yyyy-MM-dd");
    if (!currentDate.isValid()) {
        currentDate = QDate::currentDate();
    }

    int step = 1;
    // 2. Extract elements from priority queue in prioritized order
    while (!pq.empty()) {
        SkillGap topGap = pq.top();
        pq.pop();

        int gapSize = topGap.calculateGap();
        // Estimated duration: 1 to 2 weeks per gap level
        int weeksNeeded = std::max(1, gapSize * 2);
        QDate taskStartDate = currentDate;
        QDate taskDueDate = currentDate.addDays(weeksNeeded * 7);

        std::string phaseName;
        if (topGap.isMissing()) {
            phaseName = "Phase " + std::to_string(step) + " (Foundations): Master " + topGap.getSkillName();
        } else {
            phaseName = "Phase " + std::to_string(step) + " (Advancement): Elevate " + topGap.getSkillName() +
                        " to Level " + std::to_string(topGap.getRequiredLevel());
        }

        double priorityScore = topGap.getPriorityScore();

        LearningTask task(
            0, // New task ID (assigned upon database insertion)
            studentId,
            topGap.getSkillId(),
            phaseName,
            taskStartDate.toString("yyyy-MM-dd").toStdString(),
            taskDueDate.toString("yyyy-MM-dd").toStdString(),
            "Not Started",
            0,
            priorityScore,
            ""
        );

        generatedTasks.push_back(task);

        // Sequence next task start date after current task
        currentDate = taskDueDate.addDays(1);
        step++;
    }

    return generatedTasks;
}

bool RoadmapService::saveRoadmapTasks(int studentId, const std::vector<LearningTask>& tasks) {
    auto existingTasks = m_learningRepo.getTasksForStudent(studentId);
    std::unordered_set<int> activeSkillIds;
    std::unordered_set<int> completedSkillIds;

    for (const auto& et : existingTasks) {
        if (et.getStatus() == "Completed") {
            completedSkillIds.insert(et.getSkillId());
        } else {
            activeSkillIds.insert(et.getSkillId());
        }
    }

    bool allSuccess = true;
    for (const auto& task : tasks) {
        // Prevent duplicate active tasks for the same student and skill
        if (activeSkillIds.find(task.getSkillId()) != activeSkillIds.end()) {
            continue;
        }
        // Preserve previously completed tasks - do not regenerate
        if (completedSkillIds.find(task.getSkillId()) != completedSkillIds.end()) {
            continue;
        }

        LearningTask t = task;
        t.setStudentId(studentId);
        int outId = 0;
        if (!m_learningRepo.createTask(t, outId)) {
            allSuccess = false;
        } else {
            activeSkillIds.insert(task.getSkillId());
        }
    }
    return allSuccess;
}
