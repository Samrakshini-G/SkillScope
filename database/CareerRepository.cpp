#include "CareerRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

std::vector<JobRole> CareerRepository::getAllJobRoles() {
    std::vector<JobRole> roles;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query("SELECT id, title, description FROM job_roles ORDER BY title", db);
    while (query.next()) {
        roles.emplace_back(
            query.value("id").toInt(),
            query.value("title").toString().toStdString(),
            query.value("description").toString().toStdString()
        );
    }
    return roles;
}

std::vector<JobRole> CareerRepository::searchJobRoles(const std::string& query) {
    std::vector<JobRole> roles;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery q(db);
    q.prepare("SELECT id, title, description FROM job_roles "
              "WHERE LOWER(title) LIKE :q OR LOWER(description) LIKE :q "
              "ORDER BY title");
    QString pattern = "%" + QString::fromStdString(query).trimmed().toLower() + "%";
    q.bindValue(":q", pattern);

    if (q.exec()) {
        while (q.next()) {
            roles.emplace_back(
                q.value("id").toInt(),
                q.value("title").toString().toStdString(),
                q.value("description").toString().toStdString()
            );
        }
    }
    return roles;
}

std::optional<JobRole> CareerRepository::getJobRoleById(int id) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT id, title, description FROM job_roles WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return JobRole(
            query.value("id").toInt(),
            query.value("title").toString().toStdString(),
            query.value("description").toString().toStdString()
        );
    }
    return std::nullopt;
}

std::vector<JobRequirement> CareerRepository::getRequirementsForRole(int jobRoleId) {
    std::vector<JobRequirement> reqs;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT jrs.skill_id, s.name AS skill_name, jrs.required_level, jrs.importance "
                  "FROM job_role_skills jrs "
                  "JOIN skills s ON jrs.skill_id = s.id "
                  "WHERE jrs.job_role_id = :role_id "
                  "ORDER BY jrs.required_level DESC");
    query.bindValue(":role_id", jobRoleId);

    if (query.exec()) {
        while (query.next()) {
            reqs.emplace_back(
                query.value("skill_id").toInt(),
                query.value("skill_name").toString().toStdString(),
                query.value("required_level").toInt(),
                query.value("importance").toString().toStdString()
            );
        }
    } else {
        qWarning() << "Failed to fetch requirements for role:" << query.lastError().text();
    }
    return reqs;
}

bool CareerRepository::saveAnalysisHistory(int studentId, int jobRoleId, double matchPercentage,
                                           const std::vector<SkillGap>& gaps) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery historyQuery(db);
    historyQuery.prepare("INSERT INTO analysis_history (student_id, job_role_id, match_percentage) "
                         "VALUES (:student_id, :role_id, :match)");
    historyQuery.bindValue(":student_id", studentId);
    historyQuery.bindValue(":role_id", jobRoleId);
    historyQuery.bindValue(":match", matchPercentage);

    if (!historyQuery.exec()) {
        qWarning() << "Failed to save analysis history:" << historyQuery.lastError().text();
        return false;
    }

    int analysisId = historyQuery.lastInsertId().toInt();

    QSqlQuery gapQuery(db);
    gapQuery.prepare("INSERT INTO analysis_gaps (analysis_id, skill_id, current_level, required_level, gap_size, importance) "
                     "VALUES (:analysis_id, :skill_id, :curr, :req, :gap, :importance)");

    for (const auto& gap : gaps) {
        gapQuery.bindValue(":analysis_id", analysisId);
        gapQuery.bindValue(":skill_id", gap.getSkillId());
        gapQuery.bindValue(":curr", gap.getCurrentLevel());
        gapQuery.bindValue(":req", gap.getRequiredLevel());
        gapQuery.bindValue(":gap", gap.calculateGap());
        gapQuery.bindValue(":importance", QString::fromStdString(gap.getImportance()));
        if (!gapQuery.exec()) {
            qWarning() << "Failed to save analysis gap:" << gapQuery.lastError().text();
        }
    }

    return true;
}

std::vector<CareerRepository::AnalysisSummary> CareerRepository::getAnalysisHistoryForStudent(int studentId) {
    std::vector<AnalysisSummary> results;
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT ah.id, ah.job_role_id, jr.title, ah.match_percentage, ah.analysis_date "
                  "FROM analysis_history ah "
                  "JOIN job_roles jr ON ah.job_role_id = jr.id "
                  "WHERE ah.student_id = :student_id "
                  "ORDER BY ah.analysis_date DESC");
    query.bindValue(":student_id", studentId);

    if (query.exec()) {
        while (query.next()) {
            AnalysisSummary sum;
            sum.id = query.value("id").toInt();
            sum.roleId = query.value("job_role_id").toInt();
            sum.roleTitle = query.value("title").toString().toStdString();
            sum.matchPercentage = query.value("match_percentage").toDouble();
            sum.date = query.value("analysis_date").toString().toStdString();
            results.push_back(sum);
        }
    }
    return results;
}
