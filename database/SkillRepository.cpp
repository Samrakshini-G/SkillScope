#include "SkillRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <algorithm>

std::vector<Skill> SkillRepository::getAllSkills() {
    std::vector<Skill> skills;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query("SELECT id, name, category FROM skills ORDER BY category, name", db);
    while (query.next()) {
        skills.emplace_back(
            query.value("id").toInt(),
            query.value("name").toString().toStdString(),
            query.value("category").toString().toStdString()
        );
    }
    return skills;
}

std::vector<Skill> SkillRepository::searchSkills(const std::string& query) {
    std::vector<Skill> skills;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery q(db);
    q.prepare("SELECT id, name, category FROM skills "
              "WHERE LOWER(name) LIKE :query OR LOWER(category) LIKE :query "
              "ORDER BY category, name");
    QString pattern = "%" + QString::fromStdString(query).trimmed().toLower() + "%";
    q.bindValue(":query", pattern);

    if (q.exec()) {
        while (q.next()) {
            skills.emplace_back(
                q.value("id").toInt(),
                q.value("name").toString().toStdString(),
                q.value("category").toString().toStdString()
            );
        }
    }
    return skills;
}

std::optional<Skill> SkillRepository::getSkillById(int id) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT id, name, category FROM skills WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return Skill(
            query.value("id").toInt(),
            query.value("name").toString().toStdString(),
            query.value("category").toString().toStdString()
        );
    }
    return std::nullopt;
}

std::optional<Skill> SkillRepository::getSkillByName(const std::string& name) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT id, name, category FROM skills WHERE LOWER(name) = LOWER(:name)");
    query.bindValue(":name", QString::fromStdString(name));

    if (query.exec() && query.next()) {
        return Skill(
            query.value("id").toInt(),
            query.value("name").toString().toStdString(),
            query.value("category").toString().toStdString()
        );
    }
    return std::nullopt;
}

std::map<int, int> SkillRepository::getStudentSkillLevels(int studentId) {
    std::map<int, int> studentSkills;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT skill_id, proficiency_level FROM student_skills WHERE student_id = :student_id");
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        while (query.next()) {
            studentSkills[query.value("skill_id").toInt()] = query.value("proficiency_level").toInt();
        }
    } else {
        qWarning() << "Failed to fetch student skills:" << query.lastError().text();
    }

    return studentSkills;
}

std::vector<Skill> SkillRepository::getStudentSkills(int studentId) {
    std::vector<Skill> skills;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT s.id, s.name, s.category FROM skills s "
                  "JOIN student_skills ss ON s.id = ss.skill_id "
                  "WHERE ss.student_id = :student_id "
                  "ORDER BY s.category, s.name");
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        while (query.next()) {
            skills.emplace_back(
                query.value("id").toInt(),
                query.value("name").toString().toStdString(),
                query.value("category").toString().toStdString()
            );
        }
    }
    return skills;
}

bool SkillRepository::hasStudentSkill(int studentId, int skillId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM student_skills WHERE student_id = :student_id AND skill_id = :skill_id");
    query.bindValue(":student_id", studentId);
    query.bindValue(":skill_id", skillId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool SkillRepository::addStudentSkill(int studentId, int skillId, int level) {
    if (hasStudentSkill(studentId, skillId)) {
        return false; // Prevent duplicate skill addition
    }
    return setStudentSkillLevel(studentId, skillId, level);
}

bool SkillRepository::setStudentSkillLevel(int studentId, int skillId, int level) {
    int clamped = std::clamp(level, 0, 3);
    QSqlDatabase& db = DatabaseManager::instance().database();

    // Use SQLite INSERT OR REPLACE / UPSERT
    QSqlQuery query(db);
    query.prepare("INSERT INTO student_skills (student_id, skill_id, proficiency_level) "
                  "VALUES (:student_id, :skill_id, :level) "
                  "ON CONFLICT(student_id, skill_id) DO UPDATE SET proficiency_level = :level");
    query.bindValue(":student_id", studentId);
    query.bindValue(":skill_id", skillId);
    query.bindValue(":level", clamped);

    if (!query.exec()) {
        qWarning() << "Failed to save student skill level:" << query.lastError().text();
        return false;
    }
    return true;
}

bool SkillRepository::removeStudentSkill(int studentId, int skillId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("DELETE FROM student_skills WHERE student_id = :student_id AND skill_id = :skill_id");
    query.bindValue(":student_id", studentId);
    query.bindValue(":skill_id", skillId);

    if (!query.exec()) {
        qWarning() << "Failed to delete student skill:" << query.lastError().text();
        return false;
    }
    return true;
}
