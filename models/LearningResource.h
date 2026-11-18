#ifndef LEARNINGRESOURCE_H
#define LEARNINGRESOURCE_H

#include <string>

/**
 * @brief Abstract base class representing a learning resource for a skill.
 */
class LearningResource {
public:
    LearningResource(int id, int skillId, const std::string& title, const std::string& url);
    virtual ~LearningResource() = default;

    int getId() const;
    void setId(int id);

    int getSkillId() const;
    void setSkillId(int skillId);

    std::string getTitle() const;
    void setTitle(const std::string& title);

    std::string getUrl() const;
    void setUrl(const std::string& url);

    std::string getDifficulty() const;
    void setDifficulty(const std::string& difficulty);

    // Pure virtual methods
    virtual std::string resourceType() const = 0;
    virtual std::string learningDescription() const = 0;
    virtual std::string getDuration() const = 0;

protected:
    int m_id;
    int m_skillId;
    std::string m_title;
    std::string m_url;
    std::string m_difficulty;
};

/**
 * @brief Derived class for video-based learning resources.
 */
class VideoResource : public LearningResource {
public:
    VideoResource(int id, int skillId, const std::string& title, const std::string& url,
                  int durationMinutes, const std::string& difficulty = "Intermediate");
    ~VideoResource() override = default;

    int getDurationMinutes() const;
    void setDurationMinutes(int durationMinutes);

    std::string resourceType() const override;
    std::string learningDescription() const override;
    std::string getDuration() const override;

private:
    int m_durationMinutes;
};

/**
 * @brief Derived class for reading and tutorial articles.
 */
class ArticleResource : public LearningResource {
public:
    ArticleResource(int id, int skillId, const std::string& title, const std::string& url,
                    int readTimeMinutes, const std::string& difficulty = "Beginner");
    ~ArticleResource() override = default;

    int getReadTimeMinutes() const;
    void setReadTimeMinutes(int readTimeMinutes);

    std::string resourceType() const override;
    std::string learningDescription() const override;
    std::string getDuration() const override;

private:
    int m_readTimeMinutes;
};

/**
 * @brief Derived class for interactive coding exercises and practice problems.
 */
class PracticeResource : public LearningResource {
public:
    PracticeResource(int id, int skillId, const std::string& title, const std::string& url,
                     const std::string& platform, int exerciseCount,
                     const std::string& difficulty = "Advanced");
    ~PracticeResource() override = default;

    std::string getPlatform() const;
    void setPlatform(const std::string& platform);

    int getExerciseCount() const;
    void setExerciseCount(int count);

    std::string resourceType() const override;
    std::string learningDescription() const override;
    std::string getDuration() const override;

private:
    std::string m_platform;
    int m_exerciseCount;
};

#endif // LEARNINGRESOURCE_H
