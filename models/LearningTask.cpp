#include "LearningTask.h"
#include <algorithm>

LearningTask::LearningTask()
    : m_id(0), m_studentId(0), m_skillId(0), m_title(""),
      m_startDate(""), m_dueDate(""), m_status("Not Started"), m_progress(0),
      m_priorityScore(0.0), m_completionDate("") {}

LearningTask::LearningTask(int id, int studentId, int skillId, const std::string& title,
                           const std::string& startDate, const std::string& dueDate,
                           const std::string& status, int progress,
                           double priorityScore, const std::string& completionDate)
    : m_id(id), m_studentId(studentId), m_skillId(skillId), m_title(title),
      m_startDate(startDate), m_dueDate(dueDate), m_status(status),
      m_progress(std::clamp(progress, 0, 100)),
      m_priorityScore(priorityScore),
      m_completionDate(completionDate) {
    if (m_progress == 100) {
        m_status = "Completed";
    } else if (m_progress > 0 && m_status == "Not Started") {
        m_status = "In Progress";
    }
}

int LearningTask::getId() const {
    return m_id;
}

void LearningTask::setId(int id) {
    m_id = id;
}

int LearningTask::getStudentId() const {
    return m_studentId;
}

void LearningTask::setStudentId(int studentId) {
    m_studentId = studentId;
}

int LearningTask::getSkillId() const {
    return m_skillId;
}

void LearningTask::setSkillId(int skillId) {
    m_skillId = skillId;
}

std::string LearningTask::getTitle() const {
    return m_title;
}

void LearningTask::setTitle(const std::string& title) {
    m_title = title;
}

std::string LearningTask::getStartDate() const {
    return m_startDate;
}

void LearningTask::setStartDate(const std::string& startDate) {
    m_startDate = startDate;
}

std::string LearningTask::getDueDate() const {
    return m_dueDate;
}

void LearningTask::setDueDate(const std::string& dueDate) {
    m_dueDate = dueDate;
}

std::string LearningTask::getStatus() const {
    return m_status;
}

void LearningTask::setStatus(const std::string& status) {
    m_status = status;
}

int LearningTask::getProgress() const {
    return m_progress;
}

double LearningTask::getPriorityScore() const {
    return m_priorityScore;
}

void LearningTask::setPriorityScore(double priorityScore) {
    m_priorityScore = priorityScore;
}

std::string LearningTask::getCompletionDate() const {
    return m_completionDate;
}

void LearningTask::setCompletionDate(const std::string& completionDate) {
    m_completionDate = completionDate;
}

void LearningTask::updateProgress(int progress) {
    m_progress = std::clamp(progress, 0, 100);
    if (m_progress == 100) {
        m_status = "Completed";
    } else if (m_progress > 0) {
        m_status = "In Progress";
    } else {
        m_status = "Not Started";
    }
}

void LearningTask::updateStatus(const std::string& status) {
    m_status = status;
    if (m_status == "Completed") {
        m_progress = 100;
    } else if (m_status == "Not Started") {
        m_progress = 0;
    }
}
