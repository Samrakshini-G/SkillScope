#include "LearningRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include <QDate>

std::vector<std::unique_ptr<LearningResource>> LearningRepository::getResourcesForSkill(int skillId) {
    return filterResources(skillId, "All");
}

std::vector<std::unique_ptr<LearningResource>> LearningRepository::getResourcesByType(const std::string& type) {
    return filterResources(-1, type);
}

std::vector<std::unique_ptr<LearningResource>> LearningRepository::filterResources(int skillId, const std::string& type) {
    std::vector<std::unique_ptr<LearningResource>> resources;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QString queryStr = "SELECT id, skill_id, title, resource_type, url, description, extra_metric, extra_str "
                       "FROM learning_resources WHERE 1=1";
    if (skillId > 0) {
        queryStr += " AND skill_id = :skill_id";
    }
    if (!type.empty() && type != "All") {
        queryStr += " AND resource_type = :resource_type";
    }
    queryStr += " ORDER BY id ASC";

    QSqlQuery query(db);
    query.prepare(queryStr);
    if (skillId > 0) query.bindValue(":skill_id", skillId);
    if (!type.empty() && type != "All") query.bindValue(":resource_type", QString::fromStdString(type));

    if (query.exec()) {
        while (query.next()) {
            int id = query.value("id").toInt();
            int sId = query.value("skill_id").toInt();
            std::string title = query.value("title").toString().toStdString();
            std::string rType = query.value("resource_type").toString().toStdString();
            std::string url = query.value("url").toString().toStdString();
            int metric = query.value("extra_metric").toInt();
            std::string extra = query.value("extra_str").toString().toStdString();

            if (rType == "Video") {
                resources.push_back(std::make_unique<VideoResource>(id, sId, title, url, metric));
            } else if (rType == "Article") {
                resources.push_back(std::make_unique<ArticleResource>(id, sId, title, url, metric));
            } else if (rType == "Practice") {
                resources.push_back(std::make_unique<PracticeResource>(id, sId, title, url, extra, metric));
            }
        }
    }
    return resources;
}

std::vector<std::unique_ptr<LearningResource>> LearningRepository::getAllResources() {
    return filterResources(-1, "All");
}

std::vector<LearningTask> LearningRepository::getTasksForStudent(int studentId) {
    std::vector<LearningTask> tasks;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT id, student_id, skill_id, title, start_date, due_date, status, progress, "
                  "COALESCE(priority_score, 0.0) AS priority_score, "
                  "COALESCE(completion_date, '') AS completion_date "
                  "FROM learning_tasks WHERE student_id = :student_id ORDER BY id DESC");
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        while (query.next()) {
            tasks.emplace_back(
                query.value("id").toInt(),
                query.value("student_id").toInt(),
                query.value("skill_id").toInt(),
                query.value("title").toString().toStdString(),
                query.value("start_date").toString().toStdString(),
                query.value("due_date").toString().toStdString(),
                query.value("status").toString().toStdString(),
                query.value("progress").toInt(),
                query.value("priority_score").toDouble(),
                query.value("completion_date").toString().toStdString()
            );
        }
    }
    return tasks;
}

bool LearningRepository::createTask(const LearningTask& task, int& outTaskId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("INSERT INTO learning_tasks (student_id, skill_id, title, start_date, due_date, status, progress, priority_score, completion_date) "
                  "VALUES (:student_id, :skill_id, :title, :start_date, :due_date, :status, :progress, :priority_score, :completion_date)");
    query.bindValue(":student_id", task.getStudentId());
    query.bindValue(":skill_id", task.getSkillId());
    query.bindValue(":title", QString::fromStdString(task.getTitle()));
    query.bindValue(":start_date", QString::fromStdString(task.getStartDate()));
    query.bindValue(":due_date", QString::fromStdString(task.getDueDate()));
    query.bindValue(":status", QString::fromStdString(task.getStatus()));
    query.bindValue(":progress", task.getProgress());
    query.bindValue(":priority_score", task.getPriorityScore());
    query.bindValue(":completion_date", QString::fromStdString(task.getCompletionDate()));

    if (!query.exec()) {
        qWarning() << "Failed to insert learning task:" << query.lastError().text();
        return false;
    }
    outTaskId = query.lastInsertId().toInt();
    return true;
}

bool LearningRepository::updateTaskProgress(int taskId, int progress, const std::string& status, const std::string& completionDate) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    if (!completionDate.empty()) {
        query.prepare("UPDATE learning_tasks SET progress = :progress, status = :status, completion_date = :completion_date WHERE id = :id");
        query.bindValue(":completion_date", QString::fromStdString(completionDate));
    } else {
        query.prepare("UPDATE learning_tasks SET progress = :progress, status = :status WHERE id = :id");
    }
    query.bindValue(":progress", progress);
    query.bindValue(":status", QString::fromStdString(status));
    query.bindValue(":id", taskId);

    if (!query.exec()) {
        qWarning() << "Failed to update task progress:" << query.lastError().text();
        return false;
    }
    return true;
}

bool LearningRepository::completeTask(int taskId, const std::string& completionDate) {
    std::string dateStr = completionDate;
    if (dateStr.empty()) {
        dateStr = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    }
    return updateTaskProgress(taskId, 100, "Completed", dateStr);
}

bool LearningRepository::deleteTask(int taskId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("DELETE FROM learning_tasks WHERE id = :id");
    query.bindValue(":id", taskId);

    if (!query.exec()) {
        qWarning() << "Failed to delete task:" << query.lastError().text();
        return false;
    }
    return true;
}

bool LearningRepository::deleteTasksForStudent(int studentId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("DELETE FROM learning_tasks WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (!query.exec()) {
        qWarning() << "Failed to delete tasks for student:" << query.lastError().text();
        return false;
    }
    return true;
}

bool LearningRepository::hasTaskForStudentAndSkill(int studentId, int skillId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM learning_tasks WHERE student_id = :student_id AND skill_id = :skill_id");
    query.bindValue(":student_id", studentId);
    query.bindValue(":skill_id", skillId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
