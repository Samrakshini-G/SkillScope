#ifndef JOBROLE_H
#define JOBROLE_H

#include <string>

/**
 * @brief Represents a target career role.
 */
class JobRole {
public:
    JobRole();
    JobRole(int id, const std::string& title, const std::string& description);

    int getId() const;
    void setId(int id);

    std::string getTitle() const;
    void setTitle(const std::string& title);

    std::string getDescription() const;
    void setDescription(const std::string& description);

private:
    int m_id;
    std::string m_title;
    std::string m_description;
};

#endif // JOBROLE_H
