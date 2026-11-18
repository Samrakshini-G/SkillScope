#include "UserRepository.h"
#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

bool UserRepository::createUser(const std::string& name, const std::string& email,
                                const std::string& passwordHash, const std::string& department,
                                const std::string& institution, int& outUserId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery userQuery(db);
    userQuery.prepare("INSERT INTO users (name, email, password_hash, user_type) "
                      "VALUES (:name, :email, :password_hash, :user_type)");
    userQuery.bindValue(":name", QString::fromStdString(name));
    userQuery.bindValue(":email", QString::fromStdString(email));
    userQuery.bindValue(":password_hash", QString::fromStdString(passwordHash));
    userQuery.bindValue(":user_type", "Student");

    if (!userQuery.exec()) {
        qWarning() << "Failed to insert user:" << userQuery.lastError().text();
        return false;
    }

    outUserId = userQuery.lastInsertId().toInt();

    QSqlQuery profileQuery(db);
    profileQuery.prepare("INSERT INTO student_profiles (user_id, department, institution) "
                         "VALUES (:user_id, :department, :institution)");
    profileQuery.bindValue(":user_id", outUserId);
    profileQuery.bindValue(":department", QString::fromStdString(department));
    profileQuery.bindValue(":institution", QString::fromStdString(institution));

    if (!profileQuery.exec()) {
        qWarning() << "Failed to insert student profile:" << profileQuery.lastError().text();
        return false;
    }

    return true;
}

std::unique_ptr<Student> UserRepository::authenticateUser(const std::string& email, const std::string& passwordHash) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT u.id, u.name, u.email, sp.department, sp.institution, "
                  "COALESCE(sp.degree, '') AS degree, "
                  "COALESCE(sp.graduation_year, 2026) AS graduation_year, "
                  "COALESCE(sp.target_role_id, 0) AS target_role_id "
                  "FROM users u "
                  "LEFT JOIN student_profiles sp ON u.id = sp.user_id "
                  "WHERE u.email = :email AND u.password_hash = :hash");
    query.bindValue(":email", QString::fromStdString(email));
    query.bindValue(":hash", QString::fromStdString(passwordHash));

    if (!query.exec()) {
        qWarning() << "Auth query failed:" << query.lastError().text();
        return nullptr;
    }

    if (query.next()) {
        int id = query.value("id").toInt();
        std::string name = query.value("name").toString().toStdString();
        std::string userEmail = query.value("email").toString().toStdString();
        std::string dept = query.value("department").toString().toStdString();
        std::string inst = query.value("institution").toString().toStdString();
        std::string degree = query.value("degree").toString().toStdString();
        int gradYear = query.value("graduation_year").toInt();
        int targetRoleId = query.value("target_role_id").toInt();

        return std::make_unique<Student>(id, name, userEmail, dept, inst, degree, gradYear, targetRoleId);
    }

    return nullptr;
}

std::unique_ptr<Student> UserRepository::getStudentById(int userId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT u.id, u.name, u.email, sp.department, sp.institution, "
                  "COALESCE(sp.degree, '') AS degree, "
                  "COALESCE(sp.graduation_year, 2026) AS graduation_year, "
                  "COALESCE(sp.target_role_id, 0) AS target_role_id "
                  "FROM users u "
                  "LEFT JOIN student_profiles sp ON u.id = sp.user_id "
                  "WHERE u.id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        int id = query.value("id").toInt();
        std::string name = query.value("name").toString().toStdString();
        std::string email = query.value("email").toString().toStdString();
        std::string dept = query.value("department").toString().toStdString();
        std::string inst = query.value("institution").toString().toStdString();
        std::string degree = query.value("degree").toString().toStdString();
        int gradYear = query.value("graduation_year").toInt();
        int targetRoleId = query.value("target_role_id").toInt();

        return std::make_unique<Student>(id, name, email, dept, inst, degree, gradYear, targetRoleId);
    }

    return nullptr;
}

bool UserRepository::updateStudentProfile(int userId, const std::string& name,
                                          const std::string& department, const std::string& institution) {
    auto current = getStudentById(userId);
    std::string email = current ? current->getEmail() : "";
    std::string degree = current ? current->getDegree() : "";
    int gradYear = current ? current->getGraduationYear() : 2026;
    return updateStudentProfile(userId, name, email, department, institution, degree, gradYear);
}

bool UserRepository::updateStudentProfile(int userId, const std::string& name, const std::string& email,
                                          const std::string& department, const std::string& institution,
                                          const std::string& degree, int graduationYear) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    if (!email.empty() && emailExistsForOtherUser(email, userId)) {
        qWarning() << "Cannot update profile: email already in use by another user.";
        return false;
    }

    QSqlQuery userQuery(db);
    if (!email.empty()) {
        userQuery.prepare("UPDATE users SET name = :name, email = :email WHERE id = :id");
        userQuery.bindValue(":name", QString::fromStdString(name));
        userQuery.bindValue(":email", QString::fromStdString(email));
        userQuery.bindValue(":id", userId);
    } else {
        userQuery.prepare("UPDATE users SET name = :name WHERE id = :id");
        userQuery.bindValue(":name", QString::fromStdString(name));
        userQuery.bindValue(":id", userId);
    }

    if (!userQuery.exec()) {
        qWarning() << "Failed to update user table:" << userQuery.lastError().text();
        return false;
    }

    QSqlQuery profileQuery(db);
    profileQuery.prepare("INSERT INTO student_profiles (user_id, department, institution, degree, graduation_year) "
                         "VALUES (:id, :dept, :inst, :deg, :grad) "
                         "ON CONFLICT(user_id) DO UPDATE SET "
                         "department = :dept, institution = :inst, degree = :deg, graduation_year = :grad");
    profileQuery.bindValue(":id", userId);
    profileQuery.bindValue(":dept", QString::fromStdString(department));
    profileQuery.bindValue(":inst", QString::fromStdString(institution));
    profileQuery.bindValue(":deg", QString::fromStdString(degree));
    profileQuery.bindValue(":grad", graduationYear);

    if (!profileQuery.exec()) {
        qWarning() << "Failed to update student profile:" << profileQuery.lastError().text();
        return false;
    }

    return true;
}

bool UserRepository::setTargetRole(int userId, int roleId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("INSERT INTO student_profiles (user_id, department, institution, target_role_id) "
                  "VALUES (:id, '', '', :role_id) "
                  "ON CONFLICT(user_id) DO UPDATE SET target_role_id = :role_id");
    query.bindValue(":id", userId);
    query.bindValue(":role_id", roleId);

    if (!query.exec()) {
        qWarning() << "Failed to set target role for user:" << query.lastError().text();
        return false;
    }
    return true;
}

int UserRepository::getTargetRole(int userId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT target_role_id FROM student_profiles WHERE user_id = :id");
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

bool UserRepository::emailExistsForOtherUser(const std::string& email, int userId) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE email = :email AND id != :id");
    query.bindValue(":email", QString::fromStdString(email));
    query.bindValue(":id", userId);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool UserRepository::emailExists(const std::string& email) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE email = :email");
    query.bindValue(":email", QString::fromStdString(email));

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

std::string UserRepository::getPasswordHash(const std::string& email) {
    QSqlDatabase& db = DatabaseManager::instance().database();

    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE email = :email");
    query.bindValue(":email", QString::fromStdString(email));

    if (query.exec() && query.next()) {
        return query.value(0).toString().toStdString();
    }

    return "";
}
