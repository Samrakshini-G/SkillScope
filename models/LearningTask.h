#ifndef LEARNINGTASK_H
#define LEARNINGTASK_H

#include <string>

/**
 * @brief Represents a scheduled learning task generated as part of a roadmap.
 */
class LearningTask {
public:
    LearningTask();
    LearningTask(int id, int studentId, int skillId, const std::string& title,
                 const std::string& startDate, const std::string& dueDate,
                 const std::string& status = "Not Started", int progress = 0,
                 double priorityScore = 0.0, const std::string& completionDate = "");

    int getId() const;
    void setId(int id);

    int getStudentId() const;
    void setStudentId(int studentId);

    int getSkillId() const;
    void setSkillId(int skillId);

    std::string getTitle() const;
    void setTitle(const std::string& title);

    std::string getStartDate() const;
    void setStartDate(const std::string& startDate);

    std::string getDueDate() const;
    void setDueDate(const std::string& dueDate);

    std::string getStatus() const;
    void setStatus(const std::string& status);

    int getProgress() const;

    double getPriorityScore() const;
    void setPriorityScore(double priorityScore);

    std::string getCompletionDate() const;
    void setCompletionDate(const std::string& completionDate);

    // Mutator methods enforcing business rules
    void updateProgress(int progress);
    void updateStatus(const std::string& status);

private:
    int m_id;
    int m_studentId;
    int m_skillId;
    std::string m_title;
    std::string m_startDate;
    std::string m_dueDate;
    std::string m_status; // "Not Started", "In Progress", "Completed"
    int m_progress;       // Clamped strictly between 0 and 100
    double m_priorityScore;
    std::string m_completionDate;
};

#endif // LEARNINGTASK_H
