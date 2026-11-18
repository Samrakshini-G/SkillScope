#ifndef SKILL_H
#define SKILL_H

#include <string>

/**
 * @brief Represents a technical skill with category.
 */
class Skill {
public:
    Skill();
    Skill(int id, const std::string& name, const std::string& category);

    int getId() const;
    void setId(int id);

    std::string getName() const;
    void setName(const std::string& name);

    std::string getCategory() const;
    void setCategory(const std::string& category);

private:
    int m_id;
    std::string m_name;
    std::string m_category;
};

#endif // SKILL_H
