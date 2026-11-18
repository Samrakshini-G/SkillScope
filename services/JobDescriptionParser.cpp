#include "JobDescriptionParser.h"
#include <algorithm>
#include <cctype>
#include <regex>
#include <set>

static std::string toLower(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower;
}

static std::string escapeRegex(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '+' || c == '.' || c == '*' || c == '?' || c == '^' ||
            c == '$' || c == '(' || c == ')' || c == '[' || c == ']' ||
            c == '{' || c == '}' || c == '|' || c == '\\') {
            escaped += '\\';
        }
        escaped += c;
    }
    return escaped;
}

JobDescriptionParser::JobDescriptionParser() {
    initializeDefaultAliases();
}

void JobDescriptionParser::initializeDefaultAliases() {
    m_aliases.clear();

    m_aliases["C++"] = {"c++", "cpp", "c plus plus", "modern c++"};
    m_aliases["OOP"] = {"oop", "object oriented programming", "object-oriented programming", "object oriented"};
    m_aliases["Data Structures"] = {"data structures", "data structure", "dsa"};
    m_aliases["Algorithms"] = {"algorithms", "algorithm", "algorithmic", "algo"};
    m_aliases["STL"] = {"stl", "standard template library"};
    m_aliases["SQL"] = {"sql", "structured query language", "relational database", "rdbms"};
    m_aliases["Git"] = {"git", "github", "gitlab", "version control", "vcs"};
    m_aliases["Problem Solving"] = {"problem solving", "problem-solving", "analytical skills", "troubleshooting"};
    m_aliases["HTML"] = {"html", "html5"};
    m_aliases["CSS"] = {"css", "css3"};
    m_aliases["JavaScript"] = {"javascript", "js", "ecmascript"};
    m_aliases["Python"] = {"python", "py"};
    m_aliases["Network Security"] = {"network security", "firewalls", "firewall", "ids/ips", "network defense"};
    m_aliases["Cryptography"] = {"cryptography", "encryption", "ciphers", "ssl/tls"};
    m_aliases["Linux"] = {"linux", "unix", "bash", "shell scripting"};
    m_aliases["Docker"] = {"docker", "containers", "containerization"};
    m_aliases["Database Design"] = {"database design", "database schema", "normalization", "er modeling"};
    m_aliases["Communication"] = {"communication", "verbal communication", "written communication"};
    m_aliases["Teamwork"] = {"teamwork", "collaboration", "cross-functional"};
}

void JobDescriptionParser::addAlias(const std::string& canonicalSkillName, const std::string& alias) {
    std::string lowerAlias = toLower(alias);
    auto& list = m_aliases[canonicalSkillName];
    if (std::find(list.begin(), list.end(), lowerAlias) == list.end()) {
        list.push_back(lowerAlias);
    }
}

std::map<std::string, std::vector<std::string>> JobDescriptionParser::getAliasDictionary() const {
    return m_aliases;
}

bool JobDescriptionParser::containsPhrase(const std::string& text, const std::string& phrase) const {
    if (text.empty() || phrase.empty()) return false;

    std::string lowerText = toLower(text);
    std::string lowerPhrase = toLower(phrase);

    std::string escaped = escapeRegex(lowerPhrase);

    // Build word boundary regex pattern
    std::string pattern;
    if (std::isalnum(static_cast<unsigned char>(lowerPhrase.front()))) {
        pattern += "\\b";
    }
    pattern += escaped;
    if (std::isalnum(static_cast<unsigned char>(lowerPhrase.back()))) {
        pattern += "\\b";
    }

    try {
        std::regex re(pattern, std::regex_constants::icase);
        return std::regex_search(lowerText, re);
    } catch (...) {
        return lowerText.find(lowerPhrase) != std::string::npos;
    }
}

int JobDescriptionParser::detectProficiencyLevel(const std::string& text, const std::string& matchedTerm) const {
    std::string lowerText = toLower(text);
    std::string lowerTerm = toLower(matchedTerm);

    size_t pos = lowerText.find(lowerTerm);
    if (pos == std::string::npos) {
        return 2; // Default requirement level
    }

    // Inspect surrounding context window (60 chars before and after)
    size_t start = (pos > 60) ? pos - 60 : 0;
    size_t len = std::min(lowerText.length() - start, lowerTerm.length() + 120);
    std::string window = lowerText.substr(start, len);

    // Advanced Level 3 indicators
    static const std::vector<std::string> l3Keywords = {
        "advanced", "expert", "senior", "mastery", "in-depth", "deep understanding",
        "5+ years", "4+ years", "3+ years", "level 3", "lead", "architect"
    };
    for (const auto& kw : l3Keywords) {
        if (window.find(kw) != std::string::npos) {
            return 3;
        }
    }

    // Beginner Level 1 indicators
    static const std::vector<std::string> l1Keywords = {
        "basic", "fundamental", "beginner", "entry level", "familiarity with",
        "junior", "level 1", "exposure to", "intern"
    };
    for (const auto& kw : l1Keywords) {
        if (window.find(kw) != std::string::npos) {
            return 1;
        }
    }

    // Intermediate Level 2 (Default documented level)
    return 2;
}

std::vector<DetectedSkill> JobDescriptionParser::parseJobDescription(const std::string& text) {
    std::vector<DetectedSkill> results;
    if (text.empty()) return results;

    std::vector<Skill> dbSkills = m_skillRepo.getAllSkills();
    std::map<std::string, int> nameToId;
    for (const auto& s : dbSkills) {
        nameToId[s.getName()] = s.getId();
    }

    std::set<std::string> alreadyDetected;

    // Scan dictionary aliases
    for (const auto& [canonicalName, aliases] : m_aliases) {
        if (alreadyDetected.find(canonicalName) != alreadyDetected.end()) {
            continue;
        }

        // Check canonical name first
        if (containsPhrase(text, canonicalName)) {
            alreadyDetected.insert(canonicalName);
            int skillId = (nameToId.find(canonicalName) != nameToId.end()) ? nameToId[canonicalName] : 0;
            int level = detectProficiencyLevel(text, canonicalName);

            DetectedSkill ds;
            ds.skillId = skillId;
            ds.skillName = canonicalName;
            ds.matchedAlias = canonicalName;
            ds.suggestedProficiency = level;
            ds.importance = (level >= 3) ? "High" : (level == 2 ? "Medium" : "Low");
            ds.isRecognized = (skillId > 0);
            results.push_back(ds);
            continue;
        }

        // Check aliases
        for (const auto& alias : aliases) {
            if (containsPhrase(text, alias)) {
                alreadyDetected.insert(canonicalName);
                int skillId = (nameToId.find(canonicalName) != nameToId.end()) ? nameToId[canonicalName] : 0;
                int level = detectProficiencyLevel(text, alias);

                DetectedSkill ds;
                ds.skillId = skillId;
                ds.skillName = canonicalName;
                ds.matchedAlias = alias;
                ds.suggestedProficiency = level;
                ds.importance = (level >= 3) ? "High" : (level == 2 ? "Medium" : "Low");
                ds.isRecognized = (skillId > 0);
                results.push_back(ds);
                break; // Stop at first matched alias to prevent duplicates
            }
        }
    }

    return results;
}

std::vector<Skill> JobDescriptionParser::extractSkillsFromText(const std::string& text) {
    std::vector<Skill> skills;
    auto detected = parseJobDescription(text);

    for (const auto& item : detected) {
        if (item.isRecognized && item.skillId > 0) {
            auto optSkill = m_skillRepo.getSkillById(item.skillId);
            if (optSkill.has_value()) {
                skills.push_back(optSkill.value());
            } else {
                skills.emplace_back(item.skillId, item.skillName, "General");
            }
        }
    }
    return skills;
}

std::vector<std::string> JobDescriptionParser::extractUnrecognizedKeywords(const std::string& text) {
    std::vector<std::string> unrecognized;
    if (text.empty()) return unrecognized;

    // List of common industry tech keywords not yet tracked in DB
    static const std::vector<std::string> emergingTech = {
        "Rust", "Go", "Golang", "Kubernetes", "AWS", "Azure", "GCP",
        "React", "Angular", "Vue", "TypeScript", "Machine Learning", "GraphQL", "Redis"
    };

    for (const auto& term : emergingTech) {
        if (containsPhrase(text, term)) {
            unrecognized.push_back(term);
        }
    }

    return unrecognized;
}

std::vector<JobRequirement> JobDescriptionParser::convertToRequirements(const std::vector<DetectedSkill>& detectedList) const {
    std::vector<JobRequirement> reqs;
    for (const auto& d : detectedList) {
        if (d.isRecognized && d.skillId > 0) {
            reqs.emplace_back(d.skillId, d.skillName, d.suggestedProficiency, d.importance);
        }
    }
    return reqs;
}

std::vector<std::pair<JobRole, int>> JobDescriptionParser::matchRolesFromText(const std::string& text) {
    std::vector<Skill> detectedSkills = extractSkillsFromText(text);
    std::vector<JobRole> allRoles = m_careerRepo.getAllJobRoles();
    std::vector<std::pair<JobRole, int>> roleScores;

    for (const auto& role : allRoles) {
        auto reqs = m_careerRepo.getRequirementsForRole(role.getId());
        int score = 0;
        for (const auto& req : reqs) {
            for (const auto& detected : detectedSkills) {
                if (req.getSkillId() == detected.getId()) {
                    score += req.getImportanceWeight();
                }
            }
        }
        roleScores.emplace_back(role, score);
    }

    std::sort(roleScores.begin(), roleScores.end(),
              [](const std::pair<JobRole, int>& a, const std::pair<JobRole, int>& b) {
                  return a.second > b.second;
              });

    return roleScores;
}
