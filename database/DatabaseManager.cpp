#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QDebug>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager s_instance;
    return s_instance;
}

DatabaseManager::DatabaseManager() : m_initialized(false) {}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

QString DatabaseManager::determineDatabaseFilePath() const {
    QString appDataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!appDataDir.isEmpty()) {
        QDir dir(appDataDir);
        if (dir.mkpath(".")) {
            QString testFile = dir.filePath(".write_test");
            QFile f(testFile);
            if (f.open(QIODevice::WriteOnly)) {
                f.close();
                f.remove();
                return dir.filePath("skillscope.db");
            }
        }
    }

    // Fallback: Local application data directory
    QDir localDir(QDir::currentPath() + "/data");
    if (!localDir.exists()) {
        localDir.mkpath(".");
    }
    return localDir.filePath("skillscope.db");
}

bool DatabaseManager::openDatabase(const QString& customPath) {
    if (m_db.isOpen()) {
        return true;
    }

    if (customPath.isEmpty()) {
        m_dbPath = determineDatabaseFilePath();
    } else {
        m_dbPath = customPath;
        QFileInfo fi(m_dbPath);
        QDir().mkpath(fi.absolutePath());
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE", "SkillScopeConnection");
    m_db.setDatabaseName(m_dbPath);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qWarning() << "Failed to open database:" << m_lastError;
        return false;
    }

    // Enable SQLite foreign-key enforcement
    QSqlQuery pragmaQuery(m_db);
    if (!pragmaQuery.exec("PRAGMA foreign_keys = ON;")) {
        qWarning() << "Failed to enable foreign keys:" << pragmaQuery.lastError().text();
    }

    if (!initializeSchema()) {
        return false;
    }

    if (!seedSampleData()) {
        return false;
    }

    m_initialized = true;
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
    QSqlDatabase::removeDatabase("SkillScopeConnection");
    m_initialized = false;
}

bool DatabaseManager::isOpen() const {
    return m_db.isOpen();
}

QSqlDatabase& DatabaseManager::database() {
    return m_db;
}

QString DatabaseManager::lastError() const {
    return m_lastError;
}

QString DatabaseManager::getDatabasePath() const {
    return m_dbPath;
}

bool DatabaseManager::executeQuery(const QString& queryStr) {
    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        m_lastError = query.lastError().text();
        qWarning() << "SQL Execution Error:" << m_lastError << "\nQuery:" << queryStr;
        return false;
    }
    return true;
}

bool DatabaseManager::initializeSchema() {
    // 1. users table
    const QString createUsers = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            user_type TEXT NOT NULL,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // 2. student_profiles table
    const QString createStudentProfiles = R"(
        CREATE TABLE IF NOT EXISTS student_profiles (
            user_id INTEGER PRIMARY KEY,
            department TEXT NOT NULL,
            institution TEXT NOT NULL,
            FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
        );
    )";

    // 3. skills table
    const QString createSkills = R"(
        CREATE TABLE IF NOT EXISTS skills (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT UNIQUE NOT NULL,
            category TEXT NOT NULL
        );
    )";

    // 4. student_skills table
    const QString createStudentSkills = R"(
        CREATE TABLE IF NOT EXISTS student_skills (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            proficiency_level INTEGER NOT NULL CHECK (proficiency_level >= 0 AND proficiency_level <= 3),
            UNIQUE(student_id, skill_id),
            FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE
        );
    )";

    // 5. job_roles table
    const QString createJobRoles = R"(
        CREATE TABLE IF NOT EXISTS job_roles (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT UNIQUE NOT NULL,
            description TEXT NOT NULL
        );
    )";

    // 6. job_role_skills table
    const QString createJobRoleSkills = R"(
        CREATE TABLE IF NOT EXISTS job_role_skills (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            job_role_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            required_level INTEGER NOT NULL CHECK (required_level >= 1 AND required_level <= 3),
            importance TEXT NOT NULL,
            UNIQUE(job_role_id, skill_id),
            FOREIGN KEY (job_role_id) REFERENCES job_roles(id) ON DELETE CASCADE,
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE
        );
    )";

    // 7. learning_resources table
    const QString createLearningResources = R"(
        CREATE TABLE IF NOT EXISTS learning_resources (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            skill_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            resource_type TEXT NOT NULL,
            url TEXT NOT NULL,
            description TEXT,
            extra_metric INTEGER DEFAULT 0,
            extra_str TEXT DEFAULT '',
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE
        );
    )";

    // 8. learning_tasks table
    const QString createLearningTasks = R"(
        CREATE TABLE IF NOT EXISTS learning_tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            start_date TEXT NOT NULL,
            due_date TEXT NOT NULL,
            status TEXT NOT NULL,
            progress INTEGER NOT NULL CHECK (progress >= 0 AND progress <= 100),
            FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE
        );
    )";

    // 9. analysis_history table
    const QString createAnalysisHistory = R"(
        CREATE TABLE IF NOT EXISTS analysis_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            student_id INTEGER NOT NULL,
            job_role_id INTEGER NOT NULL,
            match_percentage REAL NOT NULL,
            analysis_date DATETIME DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (student_id) REFERENCES users(id) ON DELETE CASCADE,
            FOREIGN KEY (job_role_id) REFERENCES job_roles(id) ON DELETE CASCADE
        );
    )";

    // 10. analysis_gaps table
    const QString createAnalysisGaps = R"(
        CREATE TABLE IF NOT EXISTS analysis_gaps (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            analysis_id INTEGER NOT NULL,
            skill_id INTEGER NOT NULL,
            current_level INTEGER NOT NULL,
            required_level INTEGER NOT NULL,
            gap_size INTEGER NOT NULL,
            importance TEXT NOT NULL,
            FOREIGN KEY (analysis_id) REFERENCES analysis_history(id) ON DELETE CASCADE,
            FOREIGN KEY (skill_id) REFERENCES skills(id) ON DELETE CASCADE
        );
    )";

    const QStringList queries = {
        createUsers,
        createStudentProfiles,
        createSkills,
        createStudentSkills,
        createJobRoles,
        createJobRoleSkills,
        createLearningResources,
        createLearningTasks,
        createAnalysisHistory,
        createAnalysisGaps
    };

    for (const QString& q : queries) {
        if (!executeQuery(q)) {
            return false;
        }
    }

    // Safe backward-compatible migrations for student_profiles
    QSqlQuery checkCols("PRAGMA table_info(student_profiles)", m_db);
    bool hasDegree = false;
    bool hasGradYear = false;
    bool hasTargetRole = false;
    while (checkCols.next()) {
        QString colName = checkCols.value("name").toString();
        if (colName == "degree") hasDegree = true;
        if (colName == "graduation_year") hasGradYear = true;
        if (colName == "target_role_id") hasTargetRole = true;
    }

    if (!hasDegree) {
        executeQuery("ALTER TABLE student_profiles ADD COLUMN degree TEXT DEFAULT ''");
    }
    if (!hasGradYear) {
        executeQuery("ALTER TABLE student_profiles ADD COLUMN graduation_year INTEGER DEFAULT 2026");
    }
    if (!hasTargetRole) {
        executeQuery("ALTER TABLE student_profiles ADD COLUMN target_role_id INTEGER DEFAULT 0");
    }

    // Safe backward-compatible migrations for learning_tasks
    QSqlQuery checkTaskCols("PRAGMA table_info(learning_tasks)", m_db);
    bool hasCompletionDate = false;
    bool hasPriorityScore = false;
    while (checkTaskCols.next()) {
        QString colName = checkTaskCols.value("name").toString();
        if (colName == "completion_date") hasCompletionDate = true;
        if (colName == "priority_score") hasPriorityScore = true;
    }

    if (!hasCompletionDate) {
        executeQuery("ALTER TABLE learning_tasks ADD COLUMN completion_date TEXT DEFAULT ''");
    }
    if (!hasPriorityScore) {
        executeQuery("ALTER TABLE learning_tasks ADD COLUMN priority_score REAL DEFAULT 0.0");
    }

    return true;
}

bool DatabaseManager::seedSampleData() {
    // Helper to ensure a skill exists without duplicates
    auto ensureSkill = [&](const QString& name, const QString& category) -> int {
        QSqlQuery q(m_db);
        q.prepare("SELECT id FROM skills WHERE name = :name");
        q.bindValue(":name", name);
        if (q.exec() && q.next()) {
            return q.value(0).toInt();
        }
        QSqlQuery ins(m_db);
        ins.prepare("INSERT INTO skills (name, category) VALUES (:name, :category)");
        ins.bindValue(":name", name);
        ins.bindValue(":category", category);
        if (ins.exec()) {
            return ins.lastInsertId().toInt();
        }
        return 0;
    };

    struct SkillEntry {
        QString name;
        QString category;
    };
    const QList<SkillEntry> allSkills = {
        {"C++", "Programming Languages"},
        {"OOP", "Software Engineering"},
        {"Data Structures", "Computer Science"},
        {"Algorithms", "Computer Science"},
        {"STL", "Programming Languages"},
        {"SQL", "Database Systems"},
        {"Git", "Tools & Version Control"},
        {"Problem Solving", "Core Competencies"},
        {"HTML", "Web Development"},
        {"CSS", "Web Development"},
        {"JavaScript", "Web Development"},
        {"Python", "Programming Languages"},
        {"Network Security", "Cybersecurity"},
        {"Cryptography", "Cybersecurity"},
        {"Linux", "Operating Systems & Tools"},
        {"Docker", "DevOps & Tools"},
        {"Database Design", "Database Systems"},
        {"Communication", "Professional Skills"},
        {"Teamwork", "Professional Skills"}
    };

    for (const auto& item : allSkills) {
        ensureSkill(item.name, item.category);
    }

    // Helper to ensure a job role exists without duplicates
    auto ensureRole = [&](const QString& title, const QString& desc) -> int {
        QSqlQuery q(m_db);
        q.prepare("SELECT id FROM job_roles WHERE title = :title");
        q.bindValue(":title", title);
        if (q.exec() && q.next()) {
            return q.value(0).toInt();
        }
        QSqlQuery ins(m_db);
        ins.prepare("INSERT INTO job_roles (title, description) VALUES (:title, :description)");
        ins.bindValue(":title", title);
        ins.bindValue(":description", desc);
        if (ins.exec()) {
            return ins.lastInsertId().toInt();
        }
        return 0;
    };

    // Helper to ensure a role requirement exists without duplicates
    auto ensureReq = [&](int roleId, const QString& skillName, int requiredLevel, const QString& importance) {
        QSqlQuery sq(m_db);
        sq.prepare("SELECT id FROM skills WHERE name = :name");
        sq.bindValue(":name", skillName);
        if (!sq.exec() || !sq.next()) return;
        int skillId = sq.value(0).toInt();

        QSqlQuery rq(m_db);
        rq.prepare("INSERT INTO job_role_skills (job_role_id, skill_id, required_level, importance) "
                   "VALUES (:role_id, :skill_id, :required_level, :importance) "
                   "ON CONFLICT(job_role_id, skill_id) DO UPDATE SET "
                   "required_level = :required_level, importance = :importance");
        rq.bindValue(":role_id", roleId);
        rq.bindValue(":skill_id", skillId);
        rq.bindValue(":required_level", requiredLevel);
        rq.bindValue(":importance", importance);
        rq.exec();
    };

    // 1. C++ Software Developer
    int cppDevId = ensureRole("C++ Software Developer",
        "Designs and builds high-performance native desktop systems, multi-threaded architectures, and low-latency software.");
    ensureReq(cppDevId, "C++", 3, "High");
    ensureReq(cppDevId, "OOP", 3, "High");
    ensureReq(cppDevId, "Data Structures", 3, "High");
    ensureReq(cppDevId, "Algorithms", 2, "High");
    ensureReq(cppDevId, "STL", 3, "High");
    ensureReq(cppDevId, "Git", 2, "Medium");
    ensureReq(cppDevId, "Problem Solving", 2, "High");
    ensureReq(cppDevId, "Linux", 2, "Medium");

    // Also support previously seeded "C++ Developer"
    int legacyCppId = ensureRole("C++ Developer",
        "Focuses on high-performance native desktop applications, game engines, and low-latency systems.");
    ensureReq(legacyCppId, "C++", 3, "High");
    ensureReq(legacyCppId, "OOP", 3, "High");
    ensureReq(legacyCppId, "Data Structures", 3, "High");
    ensureReq(legacyCppId, "Algorithms", 2, "High");
    ensureReq(legacyCppId, "STL", 3, "High");
    ensureReq(legacyCppId, "Git", 2, "Medium");
    ensureReq(legacyCppId, "Problem Solving", 2, "High");

    // 2. Full Stack Developer
    int fullStackId = ensureRole("Full Stack Developer",
        "Architects responsive web frontends, REST APIs, database queries, and modular full-stack application systems.");
    ensureReq(fullStackId, "HTML", 3, "High");
    ensureReq(fullStackId, "CSS", 3, "High");
    ensureReq(fullStackId, "JavaScript", 3, "High");
    ensureReq(fullStackId, "SQL", 2, "High");
    ensureReq(fullStackId, "Git", 2, "High");
    ensureReq(fullStackId, "Python", 2, "Medium");
    ensureReq(fullStackId, "Database Design", 2, "Medium");
    ensureReq(fullStackId, "Problem Solving", 2, "High");

    // Also support "Software Developer" and "Web Developer"
    int swDevId = ensureRole("Software Developer",
        "Builds scalable, maintainable application backends, desktop software, and modular enterprise systems.");
    ensureReq(swDevId, "OOP", 3, "High");
    ensureReq(swDevId, "Data Structures", 2, "High");
    ensureReq(swDevId, "Algorithms", 2, "High");
    ensureReq(swDevId, "Git", 2, "High");
    ensureReq(swDevId, "SQL", 2, "Medium");
    ensureReq(swDevId, "Problem Solving", 2, "High");

    int webDevId = ensureRole("Web Developer",
        "Designs and implements modern interactive web user interfaces, responsive layouts, and full-stack solutions.");
    ensureReq(webDevId, "HTML", 3, "High");
    ensureReq(webDevId, "CSS", 3, "High");
    ensureReq(webDevId, "JavaScript", 3, "High");
    ensureReq(webDevId, "Git", 2, "Medium");
    ensureReq(webDevId, "SQL", 2, "Medium");

    // 3. Data Analyst
    int dataAnalystId = ensureRole("Data Analyst",
        "Analyzes complex datasets, formulates SQL data queries, and builds data transformation workflows using Python.");
    ensureReq(dataAnalystId, "SQL", 3, "High");
    ensureReq(dataAnalystId, "Python", 3, "High");
    ensureReq(dataAnalystId, "Data Structures", 2, "Medium");
    ensureReq(dataAnalystId, "Algorithms", 1, "Low");
    ensureReq(dataAnalystId, "Database Design", 2, "High");
    ensureReq(dataAnalystId, "Problem Solving", 2, "High");

    // 4. Cybersecurity Analyst
    int cyberId = ensureRole("Cybersecurity Analyst",
        "Monitors threat vectors, conducts security vulnerability assessments, and protects infrastructure using network security and cryptography.");
    ensureReq(cyberId, "Network Security", 3, "High");
    ensureReq(cyberId, "Cryptography", 2, "High");
    ensureReq(cyberId, "Linux", 2, "High");
    ensureReq(cyberId, "Python", 2, "Medium");
    ensureReq(cyberId, "SQL", 2, "Medium");
    ensureReq(cyberId, "Problem Solving", 2, "High");

    // 3. Seed Learning Resources idempotently
    {
        QMap<QString, int> skillIds;
        QSqlQuery sq("SELECT id, name FROM skills", m_db);
        while (sq.next()) {
            skillIds[sq.value("name").toString()] = sq.value("id").toInt();
        }

        auto ensureResource = [&](const QString& skillName, const QString& title, const QString& type,
                                  const QString& url, const QString& desc, int metric, const QString& extra) {
            if (!skillIds.contains(skillName)) return;
            int sId = skillIds[skillName];
            QSqlQuery checkQ(m_db);
            checkQ.prepare("SELECT id FROM learning_resources WHERE skill_id = :skill_id AND title = :title");
            checkQ.bindValue(":skill_id", sId);
            checkQ.bindValue(":title", title);
            if (checkQ.exec() && checkQ.next()) return; // Already exists

            QSqlQuery q(m_db);
            q.prepare("INSERT INTO learning_resources (skill_id, title, resource_type, url, description, extra_metric, extra_str) "
                      "VALUES (:skill_id, :title, :resource_type, :url, :description, :extra_metric, :extra_str)");
            q.bindValue(":skill_id", sId);
            q.bindValue(":title", title);
            q.bindValue(":resource_type", type);
            q.bindValue(":url", url);
            q.bindValue(":description", desc);
            q.bindValue(":extra_metric", metric);
            q.bindValue(":extra_str", extra);
            q.exec();
        };

        // C++
        ensureResource("C++", "C++ Modern Design and RAII Core Guidelines", "Video", "https://youtube.com/watch?v=modern-cpp", "Deep dive into modern C++ features, memory safety, and RAII.", 45, "");
        ensureResource("C++", "CppReference.com Comprehensive Core Language Guide", "Article", "https://en.cppreference.com/w/cpp", "Authoritative reference for C++ syntax and idioms.", 25, "");
        ensureResource("C++", "C++ OOP and Pointers Mastery Lab", "Practice", "https://exercism.org/tracks/cpp", "Solve hands-on exercises in classes, pointers and memory management.", 15, "Exercism");

        // OOP
        ensureResource("OOP", "Object-Oriented Design Principles (SOLID)", "Video", "https://youtube.com/watch?v=solid-principles", "Learn SRP, OCP, LSP, ISP, and DIP with real-world C++ patterns.", 35, "");
        ensureResource("OOP", "Refactoring Guru: Design Patterns and OOP Foundations", "Article", "https://refactoring.guru/design-patterns", "Visual guide to creational, structural, and behavioral patterns.", 30, "");
        ensureResource("OOP", "Design Pattern Implementation Katas", "Practice", "https://github.com/solid-katas", "Refactor procedural designs into modular object-oriented solutions.", 10, "GitHub Classroom");

        // Data Structures
        ensureResource("Data Structures", "Trees, Graphs, and Hash Tables in C++", "Video", "https://youtube.com/watch?v=dsa-trees", "In-depth visual walkthrough of nonlinear data structures.", 60, "");
        ensureResource("Data Structures", "Open Data Structures in C++ Text Edition", "Article", "https://opendatastructures.org", "Formal mathematical analysis of arrays, linked structures, and heaps.", 40, "");
        ensureResource("Data Structures", "LeetCode Essential 75 Data Structures Track", "Practice", "https://leetcode.com/studyplan/leetcode-75", "Targeted practice on high-frequency interview data structures.", 20, "LeetCode");

        // Algorithms
        ensureResource("Algorithms", "Sorting, Searching, and Dynamic Programming", "Video", "https://youtube.com/watch?v=algo-dp", "Algorithmic strategies with step-by-step complexity derivations.", 50, "");
        ensureResource("Algorithms", "GeeksforGeeks Algorithm Design Paradigms", "Article", "https://www.geeksforgeeks.org/fundamentals-of-algorithms", "Comprehensive overview of greedy, divide-and-conquer, and DP algorithms.", 20, "");
        ensureResource("Algorithms", "HackerRank Problem Solving Intermediate Track", "Practice", "https://hackerrank.com/domains/algorithms", "Problem-solving challenges focusing on asymptotic optimization.", 25, "HackerRank");

        // STL
        ensureResource("STL", "Mastering the C++ Standard Template Library", "Video", "https://youtube.com/watch?v=stl-deepdive", "Hands-on guide to vectors, maps, algorithms, and priority queues.", 40, "");
        ensureResource("STL", "STL Containers Complexity and Iterator Invalidation Rules", "Article", "https://cppreference.com/w/cpp/container", "Detailed reference covering time complexities and usage pitfalls.", 15, "");
        ensureResource("STL", "C++ STL Coding Drills and Custom Comparators", "Practice", "https://codeforces.com/problemset", "Practice using STL iterators and heaps under execution time constraints.", 12, "Codeforces");

        // SQL
        ensureResource("SQL", "Relational Database Design and SQL Queries", "Video", "https://youtube.com/watch?v=sql-crash-course", "Relational database concepts, normalization, and complex JOIN operations.", 45, "");
        ensureResource("SQL", "PostgreSQL and SQLite SQL Tutorial for Developers", "Article", "https://sqlbolt.com", "Interactive lesson on SELECT, JOIN, subqueries, and indexing.", 30, "");
        ensureResource("SQL", "SQLZoo Interactive Query Workbench", "Practice", "https://sqlzoo.net", "Interactive query challenges from basic SELECTs to recursive CTEs.", 18, "SQLZoo");

        // Git
        ensureResource("Git", "Git & GitHub Version Control Essentials", "Video", "https://youtube.com/watch?v=git-essentials", "Branching, merging, rebasing, and collaborative pull requests.", 30, "");
        ensureResource("Git", "Pro Git Book (Official Free Edition)", "Article", "https://git-scm.com/book/en/v2", "The definitive guide to Git internals and workflows.", 35, "");
        ensureResource("Git", "Learn Git Branching Sandbox", "Practice", "https://learngitbranching.js.org", "Interactive visual tree simulation of Git branches and rebases.", 20, "LearnGitBranching");

        // Python
        ensureResource("Python", "Python 3 Deep Dive for Software Engineers", "Video", "https://youtube.com/watch?v=python-deepdive", "Core object model, generators, decorators, and OOP patterns in Python.", 50, "");
        ensureResource("Python", "The Python Standard Library Documentation", "Article", "https://docs.python.org/3/library/", "Comprehensive language reference and idioms.", 40, "");
        ensureResource("Python", "Python Data Manipulation and Scripting Exercises", "Practice", "https://exercism.org/tracks/python", "Hands-on problems from basic syntax to advanced algorithms.", 20, "Exercism");

        // Linux
        ensureResource("Linux", "Linux Command Line and Shell Scripting Masterclass", "Video", "https://youtube.com/watch?v=linux-cli", "Master bash, process management, POSIX signals, and permissions.", 45, "");
        ensureResource("Linux", "The Linux Command Line (William Shotts)", "Article", "https://linuxcommand.org/tlcl.php", "Complete text guide on terminal navigation and pipelines.", 35, "");
        ensureResource("Linux", "OverTheWire Bandit Wargame Labs", "Practice", "https://overthewire.org/wargames/bandit/", "Security-focused CLI challenges teaching Linux fundamentals.", 34, "OverTheWire");

        // Network Security
        ensureResource("Network Security", "TCP/IP, Firewalls, and Network Defense Architecture", "Video", "https://youtube.com/watch?v=network-security", "Deep dive into network protocols, IDS/IPS, and security zones.", 55, "");
        ensureResource("Network Security", "NIST Network Security Guidelines & Standards", "Article", "https://csrc.nist.gov/publications", "Official federal guidelines on network architecture defense.", 30, "");
        ensureResource("Network Security", "TryHackMe Pre-Security Network Labs", "Practice", "https://tryhackme.com/module/network-fundamentals", "Hands-on virtual network analysis and packet capturing.", 15, "TryHackMe");

        // Cryptography
        ensureResource("Cryptography", "Applied Cryptography, RSA, and Modern Ciphers", "Video", "https://youtube.com/watch?v=crypto-foundations", "Mathematical principles of symmetric, asymmetric, and hashing algorithms.", 60, "");
        ensureResource("Cryptography", "Cryptographic Right Answers Guidelines", "Article", "https://latacora.micro.blog/2018/04/03/cryptographic-right-answers.html", "Practical modern guidance on cipher suites and public key crypto.", 25, "");
        ensureResource("Cryptography", "Cryptopals Crypto Challenges Track", "Practice", "https://cryptopals.com", "Implement real cryptographic attacks to understand security proofs.", 20, "Cryptopals");

        // Docker
        ensureResource("Docker", "Containerization with Docker from Scratch", "Video", "https://youtube.com/watch?v=docker-fundamentals", "Image creation, multi-stage Dockerfiles, networking, and volumes.", 40, "");
        ensureResource("Docker", "Official Docker Documentation & Best Practices", "Article", "https://docs.docker.com/get-started/", "Production guidance for container security and optimization.", 20, "");
        ensureResource("Docker", "Play with Docker Interactive Labs", "Practice", "https://labs.play-with-docker.com/", "Browser-based sandbox for multi-container orchestration.", 10, "Docker Labs");

        // Database Design
        ensureResource("Database Design", "Relational Schema Normalization (1NF to BCNF)", "Video", "https://youtube.com/watch?v=db-normalization", "Master entity-relationship modeling and functional dependencies.", 45, "");
        ensureResource("Database Design", "Database Design and Normalization Guide", "Article", "https://www.geeksforgeeks.org/database-normalization-introduction/", "Step-by-step normalization examples and schema patterns.", 25, "");
        ensureResource("Database Design", "Schema Modeling and Indexing Drills", "Practice", "https://sqlfiddle.com", "Design relational schemas, test constraints and optimize query plans.", 12, "SQLFiddle");

        // Web technologies: HTML, CSS, JavaScript
        ensureResource("HTML", "Modern Semantic HTML5 and Accessibility (a11y)", "Video", "https://youtube.com/watch?v=html5-semantics", "Structured semantic markup, forms, and accessibility standards.", 30, "");
        ensureResource("HTML", "MDN Web Docs: HTML Elements Reference", "Article", "https://developer.mozilla.org/en-US/docs/Web/HTML", "The standard reference for semantic web elements.", 20, "");
        ensureResource("HTML", "FreeCodeCamp Responsive Web Design Track", "Practice", "https://freecodecamp.org", "Interactive browser challenges for HTML5 structure.", 25, "FreeCodeCamp");

        ensureResource("CSS", "CSS Grid, Flexbox, and Modern Responsive Layouts", "Video", "https://youtube.com/watch?v=css-grid-flex", "Build responsive interfaces without layout libraries.", 40, "");
        ensureResource("CSS", "CSS-Tricks Complete Guide to Flexbox & Grid", "Article", "https://css-tricks.com/snippets/css/a-guide-to-flexbox/", "Visual cheatsheets and reference guides.", 20, "");
        ensureResource("CSS", "Flexbox Froggy and Grid Garden Drills", "Practice", "https://flexboxfroggy.com", "Fun interactive puzzle games to master CSS alignment.", 28, "Codepip");

        ensureResource("JavaScript", "Modern JavaScript (ES6+) Core Fundamentals", "Video", "https://youtube.com/watch?v=es6-javascript", "Closures, promises, async/await, and event loop mechanics.", 50, "");
        ensureResource("JavaScript", "The Modern JavaScript Tutorial (javascript.info)", "Article", "https://javascript.info", "From fundamentals to advanced topics including memory management.", 45, "");
        ensureResource("JavaScript", "JavaScript30 30-Day Vanilla Coding Challenge", "Practice", "https://javascript30.com", "Build 30 vanilla JavaScript projects without external libraries.", 30, "WesBos");

        // Core competencies & professional skills
        ensureResource("Problem Solving", "Competitive Programming and Algorithmic Thinking", "Video", "https://youtube.com/watch?v=problem-solving-cs", "Patterns for breaking down complex algorithmic requirements.", 45, "");
        ensureResource("Problem Solving", "How to Think Like a Programmer", "Article", "https://www.freecodecamp.org/news/how-to-think-like-a-programmer/", "Frameworks for technical problem decomposition.", 20, "");
        ensureResource("Problem Solving", "Project Euler Mathematical Coding Challenges", "Practice", "https://projecteuler.net", "Challenging math and computation problems.", 25, "Project Euler");

        ensureResource("Communication", "Technical Writing and Architectural Documentation", "Video", "https://youtube.com/watch?v=tech-writing", "Documenting technical architectures and design decisions clearly.", 30, "");
        ensureResource("Communication", "Google Technical Writing Course Materials", "Article", "https://developers.google.com/tech-writing", "Clear prose, API design docs, and code reviews.", 25, "");
        ensureResource("Communication", "Design Document Peer Review Workshop", "Practice", "https://github.com/tech-writing-practice", "Write RFCs and review architectural trade-offs.", 5, "GitHub");

        ensureResource("Teamwork", "Agile, Git Workflows, and Code Review Best Practices", "Video", "https://youtube.com/watch?v=agile-teamwork", "Branching strategies, collaborative estimation, and constructive reviews.", 30, "");
        ensureResource("Teamwork", "Conventional Commits and PR Best Practices", "Article", "https://www.conventionalcommits.org", "Standardized team communication via version control.", 15, "");
        ensureResource("Teamwork", "Open Source Contribution Simulation", "Practice", "https://firstcontributions.github.io", "Collaborate on real repositories with pull request workflows.", 5, "FirstContributions");
    }

    // 4. Seed Demo User if users table is empty
    QSqlQuery checkUsers("SELECT COUNT(*) FROM users", m_db);
    if (checkUsers.next() && checkUsers.value(0).toInt() == 0) {
        // Create demo student account
        const QString demoEmail = "student@skillscope.edu";
        const QString demoPass = "student123";
        const QString salt = "SkillScopeSalt_2026";
        const QString hash = QString(QCryptographicHash::hash((salt + demoPass).toUtf8(), QCryptographicHash::Sha256).toHex());

        QSqlQuery insertUser(m_db);
        insertUser.prepare("INSERT INTO users (name, email, password_hash, user_type) "
                           "VALUES (:name, :email, :password_hash, :user_type)");
        insertUser.bindValue(":name", "Alex Morgan");
        insertUser.bindValue(":email", demoEmail);
        insertUser.bindValue(":password_hash", hash);
        insertUser.bindValue(":user_type", "Student");
        if (insertUser.exec()) {
            int userId = insertUser.lastInsertId().toInt();

            // Insert student profile
            QSqlQuery insertProfile(m_db);
            insertProfile.prepare("INSERT INTO student_profiles (user_id, department, institution) "
                                  "VALUES (:user_id, :department, :institution)");
            insertProfile.bindValue(":user_id", userId);
            insertProfile.bindValue(":department", "Computer Science & Engineering");
            insertProfile.bindValue(":institution", "State University of Technology");
            insertProfile.exec();

            // Seed initial student skills for demo student
            QMap<QString, int> skillIds;
            QSqlQuery sq("SELECT id, name FROM skills", m_db);
            while (sq.next()) {
                skillIds[sq.value("name").toString()] = sq.value("id").toInt();
            }

            auto addStudentSkill = [&](const QString& skillName, int level) {
                if (!skillIds.contains(skillName)) return;
                QSqlQuery ssk(m_db);
                ssk.prepare("INSERT INTO student_skills (student_id, skill_id, proficiency_level) "
                            "VALUES (:student_id, :skill_id, :proficiency_level)");
                ssk.bindValue(":student_id", userId);
                ssk.bindValue(":skill_id", skillIds[skillName]);
                ssk.bindValue(":proficiency_level", level);
                ssk.exec();
            };

            addStudentSkill("C++", 2);
            addStudentSkill("OOP", 1);
            addStudentSkill("Data Structures", 1);
            addStudentSkill("Algorithms", 0);
            addStudentSkill("STL", 1);
            addStudentSkill("SQL", 2);
            addStudentSkill("Git", 2);
            addStudentSkill("Problem Solving", 1);
            addStudentSkill("HTML", 2);
            addStudentSkill("CSS", 1);
            addStudentSkill("JavaScript", 1);
            addStudentSkill("Python", 2);

            // Add an initial learning task
            QSqlQuery insertTask(m_db);
            insertTask.prepare("INSERT INTO learning_tasks (student_id, skill_id, title, start_date, due_date, status, progress) "
                               "VALUES (:student_id, :skill_id, :title, :start_date, :due_date, :status, :progress)");
            insertTask.bindValue(":student_id", userId);
            insertTask.bindValue(":skill_id", skillIds["C++"]);
            insertTask.bindValue(":title", "Modern C++ Memory Management & RAII");
            insertTask.bindValue(":start_date", "2026-11-15");
            insertTask.bindValue(":due_date", "2026-11-25");
            insertTask.bindValue(":status", "In Progress");
            insertTask.bindValue(":progress", 45);
            insertTask.exec();
        }
    }

    return true;
}
