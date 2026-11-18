#ifndef USER_H
#define USER_H

#include <string>

/**
 * @brief Abstract base class representing a user in the SkillScope system.
 */
class User {
public:
    User(int id, const std::string& name, const std::string& email);
    virtual ~User() = default;

    // Getters and setters
    int getId() const;
    void setId(int id);

    std::string getName() const;
    void setName(const std::string& name);

    std::string getEmail() const;
    void setEmail(const std::string& email);

    // Pure virtual methods
    virtual std::string userType() const = 0;
    virtual std::string dashboardMessage() const = 0;

protected:
    int m_id;
    std::string m_name;
    std::string m_email;
};

#endif // USER_H
